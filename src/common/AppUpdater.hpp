#pragma once

#include "UpgradeInterface.hpp"
#include <QQuickItem>
#include <shared_mutex>

namespace Morose::Utils {

    class AppUpdater : public QQuickItem {
        Q_OBJECT
        QML_ELEMENT

        int                m_channel          = Gitee;
        std::atomic<qreal> m_downloadProgress = 0;

    public:
        enum UpdateChannel {
            Gitee = 0,
            PrivateServer,
        };
        Q_ENUM(UpdateChannel)

        Q_INVOKABLE void doDownloadFile();
        Q_INVOKABLE void doUpgrade();

        static AppUpdater* Instance() noexcept;

        int   channel() const;
        void  setChannel(int newChannel);
        qreal downloadProgress() const;
        void  setDownloadProgress(qreal newDownloadProgress);

    signals:

        void newVersionFound(QString ver, QString msg);
        void versionCheckFailed(QString ver);
        void versionCheckDone();
        void channelChanged();
        void downloadProgressChanged();
        void downloadSuccess();

    private:
        AppUpdater();
        Q_DISABLE_COPY_MOVE(AppUpdater);

        std::shared_ptr<UpgradeInterface> m_upgradeInterface = nullptr;
        std::shared_mutex                 m_paramMutex       = {};
        QString                           m_downloadFilename = {};

        void checkVersion();
        void initInterface();

        Q_PROPERTY(int channel READ channel WRITE setChannel NOTIFY channelChanged FINAL)
        Q_PROPERTY(qreal downloadProgress READ downloadProgress WRITE setDownloadProgress NOTIFY downloadProgressChanged FINAL)
    };
} // namespace Morose::Utils
