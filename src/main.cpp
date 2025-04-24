#include "common/UpgradeInterface.hpp"
#include "common/common.hpp"
#include "morose_config.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFont>
#include <QGuiApplication>
#include <QPluginLoader>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QScreen>
#include <QSslSocket>
#include <QStandardPaths>
#include <SingleApplication.h>
#include <UnionType>

#ifdef Q_OS_WIN32
    #include <Dbghelp.h>
    #include <Windows.h>
    #include <stacktrace>
    #include <tchar.h>

    #pragma comment(lib, "Dbghelp.lib")

void CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS* pException) {
    // 创建Dump文件
    HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    // Dump信息
    MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
    dumpInfo.ExceptionPointers = pException;
    dumpInfo.ThreadId          = GetCurrentThreadId();
    dumpInfo.ClientPointers    = TRUE;
    // 写入Dump文件内容
    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
    CloseHandle(hDumpFile);
}

LONG ApplicationCrashHandler(EXCEPTION_POINTERS* pException) {
    QString file_dump = qApp->applicationDirPath() + "/crash/" + QDateTime::currentDateTime().toString("yyyy-MM-dd/hh-mm-ss") + ".dmp";
    if (auto file_dump_dir = QFileInfo(file_dump).dir(); !file_dump_dir.exists()) {
        file_dump_dir.mkpath(file_dump_dir.path());
    }

    auto stack_trace = std::stacktrace::current();
    qCritical(std::to_string(stack_trace).c_str());

    CreateDumpFile(file_dump.toStdWString().c_str(), pException);
    FatalAppExit(0xFFFFFFFF, _T("*** Unhandled Exception ***"));

    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

int main(int argc, char* argv[]) {
#ifdef Q_OS_WIN32
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
#endif

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    // Qt5默认未开启高DPI模式, 需要手动打开
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    // 高DPI适配策略
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
#else
    QQuickWindow::setSceneGraphBackend(QSGRendererInterface::OpenGL);
#endif
    SingleApplication app(argc, argv, true);

    app.setWindowIcon(QIcon(":/img/morose.ico"));

    if (!app.isPrimary()) {
        app.sendMessage(app.arguments().join('|').toUtf8());
        return 0;
    }

    QDir::setCurrent(qApp->applicationDirPath());

    QDir logDir;
    if (!logDir.exists("log")) {
        logDir.mkdir("log");
    }
    auto font = app.font();
    font.setFamily("Microsoft YaHei UI");
    app.setFont(font);

    // 注册日志处理回调函数
    qInstallMessageHandler(Morose::logMessageHandler);

    // 删除缓存目录
    QDir user_temp_dir(QStandardPaths::writableLocation(QStandardPaths::TempLocation));
    user_temp_dir.setNameFilters({"UnionOffline*"});
    for (auto& it : (user_temp_dir.entryInfoList())) {
        if (it.isDir()) {
            qDebug() << "remove cache dir:" << it;
            QDir(it.absoluteFilePath()).removeRecursively();
        } else if (it.isFile()) {
            qDebug() << "remove cache file:" << it;
            QFile(it.absoluteFilePath()).remove();
        }
    }

    // 设置日志过滤规则
    QSettings log_setting("setting.ini", QSettings::IniFormat);
    log_setting.beginGroup("Rules");
    auto filter = log_setting.value("filterRules");
    if (filter.isNull()) {
#if defined(QT_DEBUG)
        log_setting.setValue("filterRules", "");
#else
        log_setting.setValue("filterRules", "*.debug=false");
#endif
        filter = log_setting.value("filterRules");
    }
    QLoggingCategory::setFilterRules(filter.toString());
    log_setting.endGroup();
    log_setting.sync();

    qInfo() << std::string(80, '-').c_str();
    qInfo() << "application start, version: " APP_VERSION;
    qInfo() << "app start param:";

    for (auto&& arg : app.arguments()) {
        qInfo() << arg;
    }

    qInfo() << std::string(80, '-').c_str();
    qDebug() << "ssl build version:" << QSslSocket::sslLibraryBuildVersionString();
    qDebug() << "supprot ssl:" << QSslSocket::supportsSsl();
    qDebug() << "ssl lib version:" << QSslSocket::sslLibraryVersionString();

    QTemporaryDir temp_dir;
    QSettings     cache_setting("setting.ini", QSettings::IniFormat);
    cache_setting.beginGroup("Cache");
    cache_setting.setValue("dir", temp_dir.path());
    qInfo() << "register cache dir:" << temp_dir.path();

    // 加载QML、注册环境变量
    const QUrl            url("qrc:/qml/MainWnd.qml");
    QQmlApplicationEngine engine;
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    // 注册编译日期和编译时间
    engine.rootContext()->setContextProperty("MOROSE_APP_BUILD_DATE", QString::fromStdString(Morose::FormatCompilerDATEString(__DATE__)));
    engine.rootContext()->setContextProperty("MOROSE_APP_BUILD_TIME", QString::fromStdString(__TIME__));
#ifdef BUILD_BY_WORKFLOW
    engine.rootContext()->setContextProperty("MOROSE_APP_BUILD_BY_WORKFLOW", 1);
#else
    engine.rootContext()->setContextProperty("MOROSE_APP_BUILD_BY_WORKFLOW", 0);
#endif
    Morose::registerVariable(engine.rootContext());
    Morose::loadGlobalEnvironment();

    engine.load(url);
    QObject::connect(&app, &QApplication::aboutToQuit, &app, [temp_dir_path = temp_dir.path()]() {
        QDir temp_dir(temp_dir_path);
        qDebug() << "remove directory:" << temp_dir_path;
        temp_dir.removeRecursively();
        qInfo() << "application quit";
    });
    auto root_objs   = engine.rootObjects();
    auto main_window = qobject_cast<QQuickWindow*>(root_objs.first());

    QObject::connect(&app, &SingleApplication::instanceStarted, main_window, [main_window]() {
        main_window->setFlag(Qt::WindowStaysOnTopHint, true);
        if (main_window->windowState() == Qt::WindowMaximized) {
            main_window->resize(main_window->minimumSize());
            main_window->setX((QGuiApplication::primaryScreen()->geometry().width() - main_window->minimumSize().width()) / 2);
            main_window->setY((QGuiApplication::primaryScreen()->geometry().height() - main_window->minimumSize().height()) / 2);
        }
        main_window->showNormal();
        main_window->setFlag(Qt::WindowStaysOnTopHint, false);
        main_window->requestActivate();
        main_window->raise();
    });

    if (app.arguments().size() > 1) {
        auto open_file = QFile(app.arguments().at(1));
        if (open_file.exists()) {
            QMetaObject::invokeMethod(main_window, "extern_open_file", Q_ARG(QVariant, app.arguments().at(1)));
        }
    }

    QObject::connect(&app, &SingleApplication::receivedMessage, main_window, [main_window](int, QByteArray message) {
        auto args = QString(message).split('|');

        if (args.size() > 1) {
            QMetaObject::invokeMethod(main_window, "extern_open_file", Q_ARG(QVariant, args.at(1)));
        }

        main_window->alert(500);
        main_window->setFlag(Qt::WindowStaysOnTopHint, true);
        bool show_maximized = false;
        if (main_window->windowState() == Qt::WindowMaximized) {
            main_window->resize(main_window->minimumSize());
            main_window->setX((QGuiApplication::primaryScreen()->geometry().width() - main_window->minimumSize().width()) / 2);
            main_window->setY((QGuiApplication::primaryScreen()->geometry().height() - main_window->minimumSize().height()) / 2);
            show_maximized = true;
        }
        main_window->setFlag(Qt::WindowStaysOnTopHint, false);
        if (show_maximized) {
            main_window->showMaximized();
        } else {
            main_window->showNormal();
        }
        main_window->requestActivate();
        main_window->raise();
    });
    return app.exec();
}
