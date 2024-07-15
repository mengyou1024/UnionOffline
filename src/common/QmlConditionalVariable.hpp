#pragma once

#include <QQuickItem>

namespace Morose::Utils {

    class QmlConditionalVariable : public QQuickItem {
        Q_OBJECT
        QML_ELEMENT
    public:
        Q_INVOKABLE void waitCondition(int timeout_ms = 500);
    signals:
        void conditionRlease();
    };
} // namespace Morose::Utils
