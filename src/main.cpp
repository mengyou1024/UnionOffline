#include "common/common.hpp"
#include "morose_config.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QPluginLoader>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QScreen>
#include <SingleApplication.h>
#include <UnionType>

int main(int argc, char* argv[]) {
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
    // 高DPI适配策略
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    // 设置QMl渲染引擎使用OPENGL
#if (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0))
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
#endif
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
    QObject::connect(&app, &QApplication::aboutToQuit, &app, []() {
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
