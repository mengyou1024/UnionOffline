#include "QmlConditionalVariable.hpp"
#include <QEventLoop>
#include <QLoggingCategory>
#include <QTimer>

namespace Morose::Utils {

    void QmlConditionalVariable::waitCondition(int timeout_ms) {
        QEventLoop eventloop;
        QTimer     timeoutTimer;
        timeoutTimer.setSingleShot(true);
        timeoutTimer.setInterval(timeout_ms);
        timeoutTimer.callOnTimeout([&]() {
            qCDebug(QLoggingCategory("Conditional")) << "timeout when wait condition";
            eventloop.quit();
        });
        connect(this, &QmlConditionalVariable::conditionRlease, &eventloop, &QEventLoop::quit);
        timeoutTimer.start();
        eventloop.exec();
        timeoutTimer.stop();
        disconnect(this, &QmlConditionalVariable::conditionRlease, nullptr, nullptr);
    }

} // namespace Morose::Utils
