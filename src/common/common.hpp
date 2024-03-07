#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlContext>
#include <QtCore>

namespace Morose {
    void logMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);
    void registerVariable(QQmlContext* context);
    void registeAllAScanFileSelector();
    void registNameFilter(QQmlContext* context);

    QJsonObject& loadGlobalEnvironment();
    QJsonObject& getGlobalEnvironment();
} // namespace Morose
