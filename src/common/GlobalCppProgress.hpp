#pragma once

#include <QObject>

namespace Morose::Utils {

    class GlobalCppProgress : public QObject {
        Q_OBJECT
        qreal m_progress = 0;
        bool  m_enable   = false;
        explicit GlobalCppProgress(QObject* parent = nullptr);

    public:
        Q_DISABLE_COPY_MOVE(GlobalCppProgress);

        static GlobalCppProgress* Instance();

        qreal progress() const;
        void  setProgress(qreal newProgress);

        bool enable() const;
        void setEnable(bool newEnable);

    signals:
        void progressChanged();

        void enableChanged();

    private:
        Q_PROPERTY(qreal progress READ progress WRITE setProgress NOTIFY progressChanged FINAL)
        Q_PROPERTY(bool enable READ enable WRITE setEnable NOTIFY enableChanged FINAL)
    };
} // namespace Morose::Utils
