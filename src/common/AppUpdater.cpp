#include "AppUpdater.hpp"
#include "UpgradeFromGitee.hpp"
#include "morose_config.h"
#include <QLoggingCategory>

[[maybe_unused]] static Q_LOGGING_CATEGORY(TAG, "AppUpgrader");

namespace Morose::Utils {

    int AppUpdater::channel() const {
        std::shared_lock lock(m_paramMutex);
        return m_channel;
    }

    void AppUpdater::setChannel(int newChannel) {
        if (m_channel == newChannel) {
            return;
        }
        std::unique_lock lock(m_paramMutex);
        m_channel = newChannel;
        emit channelChanged();
    }

    AppUpdater::AppUpdater() {
        initInterface();
    }

    void AppUpdater::initInterface() {
#if (!defined(QT_DEBUG) && MOROSE_ENABLE_UPGRADE) || MOROSE_DEBUG_UPGRADE
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
        if (check_upgrade.toBool() == false) {
            return;
        }

        switch (channel()) {
            case Gitee: {
                auto thread = std::thread([this]() {
                    constexpr auto URL = "https://gitee.com/mengyou1024/UnionOfflineInstaller/releases/latest";
                    try {
                        std::unique_lock lock(m_paramMutex);
                        m_upgradeInterface = std::make_unique<Morose::Utils::UpgradeImpl::UpgradeFromGitee>(URL);
                        lock.unlock();
                        checkVersion();
                    } catch (const std::exception& e) {
                        qCWarning(TAG) << e.what();
                    }
                });
                thread.detach();
                break;
            }
            default:
                m_upgradeInterface = nullptr;
                break;
        }
#endif
    }

    void AppUpdater::checkVersion() {
#if (!defined(QT_DEBUG) && MOROSE_ENABLE_UPGRADE) || MOROSE_DEBUG_UPGRADE
        std::shared_lock lock(m_paramMutex);
        if (m_upgradeInterface == nullptr) {
            emit versionCheckFailed(QObject::tr("无法检查更新:接口为空"));
        }

        auto remove_version = m_upgradeInterface->getRemoteInstallerVersion();
        auto message        = m_upgradeInterface->getRemoteInstallerUpgradeInfo();
    #if MOROSE_DEBUG_UPGRADE
        emit newVersionFound(remove_version.getVersonString(), message);
    #else
        if (remove_version > Version::AppVersion()) {
            emit newVersionFound(remove_version.getVersonString(), message);
        }
    #endif
#endif
    }

    qreal AppUpdater::downloadProgress() const {
        return m_downloadProgress;
    }

    void AppUpdater::setDownloadProgress(qreal newDownloadProgress) {
        if (qFuzzyCompare(m_downloadProgress, newDownloadProgress))
            return;
        m_downloadProgress = newDownloadProgress;
        emit downloadProgressChanged();
    }

    void AppUpdater::doDownloadFile() {
        std::thread thread([this] {
            std::shared_lock lock(m_paramMutex);
            m_downloadFilename.clear();
            if (m_upgradeInterface == nullptr) {
                emit versionCheckFailed(QObject::tr("无法下载更新:接口为空"));
            }
            setDownloadProgress(0);
            auto    remote_version = m_upgradeInterface->getRemoteInstallerVersion().getVersonString();
            QString filename       = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/UnionOffline-Setup-" + remote_version + ".exe";
            QFile   file(filename);
            file.open(QIODevice::WriteOnly);
            if (m_upgradeInterface->downloadRemoteInstaller(file, [this](qreal val) { setDownloadProgress(val); })) {
                m_downloadFilename = filename;
                emit downloadSuccess();
            }
            file.close();
        });
        thread.detach();
    }

    void AppUpdater::doUpgrade() {
        QProcess::startDetached(m_downloadFilename, {});
        qApp->postEvent(qApp, new QEvent(QEvent::Quit), Qt::LowEventPriority);
    }

    AppUpdater* AppUpdater::Instance() noexcept {
        static AppUpdater inst;
        return &inst;
    }

} // namespace Morose::Utils
