#include "GlobalCppProgress.hpp"

namespace Morose::Utils {
    qreal GlobalCppProgress::progress() const {
        return m_progress;
    }

    void GlobalCppProgress::setProgress(qreal newProgress) {
        if (qFuzzyCompare(m_progress, newProgress))
            return;
        m_progress = newProgress;
        emit progressChanged();
    }

    bool GlobalCppProgress::enable() const {
        return m_enable;
    }

    void GlobalCppProgress::setEnable(bool newEnable) {
        if (m_enable == newEnable)
            return;
        m_enable = newEnable;
        emit enableChanged();
    }

    GlobalCppProgress::GlobalCppProgress(QObject* parent) :
    QObject{parent} {}

    GlobalCppProgress* GlobalCppProgress::Instance() {
        static GlobalCppProgress inst;
        return &inst;
    }

} // namespace Morose::Utils
