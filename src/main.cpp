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

int main(int argc, char* argv[]) {
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
    SingleApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/img/morose.ico"));

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
    QDir _userTempDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation));
    _userTempDir.setNameFilters({"UnionOffline*"});
    for (auto& it : (_userTempDir.entryInfoList())) {
        if (it.isDir()) {
            qDebug() << "remove cache dir:" << it;
            QDir(it.absoluteFilePath()).removeRecursively();
        } else if (it.isFile()) {
            qDebug() << "remove cache file:" << it;
            QFile(it.absoluteFilePath()).remove();
        }
    }

    // 设置日志过滤规则
    QSettings logSetting("setting.ini", QSettings::IniFormat);
    logSetting.beginGroup("Rules");
    auto filter = logSetting.value("filterRules");
    if (filter.isNull()) {
#if defined(QT_DEBUG)
        logSetting.setValue("filterRules", "");
#else
        logSetting.setValue("filterRules", "*.debug=false");
#endif
        filter = logSetting.value("filterRules");
    }
    QLoggingCategory::setFilterRules(filter.toString());
    logSetting.endGroup();
    logSetting.sync();

    qInfo() << std::string(80, '-').c_str();
    qInfo() << "application start, version: " APP_VERSION;
    qInfo() << "app start param:";
    for (auto i = 0; i < argc; i++) {
        qInfo() << argv[i];
    }
    qInfo() << std::string(80, '-').c_str();
    qDebug() << "ssl build version:" << QSslSocket::sslLibraryBuildVersionString();
    qDebug() << "supprot ssl:" << QSslSocket::supportsSsl();
    qDebug() << "ssl lib version:" << QSslSocket::sslLibraryVersionString();

    using Morose::Utils::UpgradeInterfaceFactory;
#if (!defined(QT_DEBUG) && MOROSE_ENABLE_UPGRADE) | MOROSE_DEBUG_UPGRADE
    QSettings upgradeSetting("setting.ini", QSettings::IniFormat);
    upgradeSetting.beginGroup("Upgrade");
    auto check_upgrade = upgradeSetting.value("checkUpgrade");
    if (check_upgrade.isNull()) {
        upgradeSetting.setValue("checkUpgrade", true);
        upgradeSetting.endGroup();
        upgradeSetting.sync();
        upgradeSetting.beginGroup("Upgrade");
        check_upgrade = upgradeSetting.value("checkUpgrade");
    }
    if (check_upgrade.toBool()) {
        auto inst = UpgradeInterfaceFactory::Instance();
        QObject::connect(inst, &UpgradeInterfaceFactory::instanceReady, inst, [=]() {
            inst->checkForUpgrade();
            QObject::disconnect(inst, &UpgradeInterfaceFactory::instanceReady, nullptr, nullptr);
        });
        std::thread upgrade_thread([=]() {
            inst->createInterface(UpgradeInterfaceFactory::UpgradeInterfaceType::Gitee);
        });
        upgrade_thread.detach();
    }
#endif

    QTemporaryDir tempDir;
    QSettings     cacheSetting("setting.ini", QSettings::IniFormat);
    cacheSetting.beginGroup("Cache");
    cacheSetting.setValue("dir", tempDir.path());
    qInfo() << "register cache dir:" << tempDir.path();

    // 加载QML、注册环境变量
    const QUrl            url("qrc:/qml/main.qml");
    QQmlApplicationEngine engine;
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    Morose::registerVariable(engine.rootContext());
    Morose::loadGlobalEnvironment();

    engine.load(url);
    QObject::connect(&app, &QApplication::aboutToQuit, &app, [&tempDir]() {
        QDir temp_dir(tempDir.path());
        temp_dir.removeRecursively();
        qInfo() << "application quit";
    });
    auto rootObjs   = engine.rootObjects();
    auto mainWindow = qobject_cast<QQuickWindow*>(rootObjs.first());
    QObject::connect(&app, &SingleApplication::instanceStarted, mainWindow, [mainWindow]() {
        mainWindow->setFlag(Qt::WindowStaysOnTopHint, true);
        if (mainWindow->windowState() == Qt::WindowMaximized) {
            mainWindow->resize(mainWindow->minimumSize());
            mainWindow->setX((QGuiApplication::primaryScreen()->geometry().width() - mainWindow->minimumSize().width()) / 2);
            mainWindow->setY((QGuiApplication::primaryScreen()->geometry().height() - mainWindow->minimumSize().height()) / 2);
        }
        mainWindow->showNormal();
        mainWindow->setFlag(Qt::WindowStaysOnTopHint, false);
        mainWindow->requestActivate();
        mainWindow->raise();
    });
    return app.exec();
}
