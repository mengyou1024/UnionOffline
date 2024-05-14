#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QQmlContext>
#include <QtCore>

namespace Morose {
#if defined(QT_DEBUG)
    struct _TEST_TIME {
        QElapsedTimer mstimer;
        QString       msg;
        _TEST_TIME(const QString& _msg) :
        msg(_msg) {
            mstimer.start();
        }
        ~_TEST_TIME() {
            auto spend = mstimer.nsecsElapsed();
            qDebug(QLoggingCategory("Morose")) << "test time:" << QString::number(spend / 1000000.0, 'f', 2).toStdString().c_str() << "ms," << msg.toStdString().c_str();
        }
    };
#else
    struct _TEST_TIME {
        _TEST_TIME(const QString&) {}
        ~_TEST_TIME() {}
    };
#endif

    void logMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);
    void registerVariable(QQmlContext* context);
    void registeAllAScanFileSelector();
    void registeAllTofdPeFileSelector();
    void registNameFilter(QQmlContext* context);

    QJsonObject& loadGlobalEnvironment();
    QJsonObject& getGlobalEnvironment();
} // namespace Morose

#define MOROSE_TEST_TIME_QUICK(x) auto __##__LINE__##__FILE = Morose::_TEST_TIME(x)

#define MOROSE_TEST_TIME_START(x) \
    {                             \
        auto __##__LINE__##__FILE = Morose::_TEST_TIME(x)

#define MOROSE_TEST_TIME_END() }
