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
        std::thread([this]() {
            initInterface();
            checkVersion();
        }).detach();
    }

    void AppUpdater::initInterface() {
#if (!defined(QT_DEBUG) && MOROSE_ENABLE_UPGRADE) || MOROSE_DEBUG_UPGRADE
        QSettings upgradeSetting("setting.ini", QSettings::IniFormat);
        upgradeSetting.beginGroup("Upgrade");
        auto check_upgrade = upgradeSetting.value("checkUpgrade");
        upgradeSetting.endGroup();
        if (check_upgrade.isNull() || check_upgrade.toBool() == false) {
            return;
        }

        while (!uiInitOK()) {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1000ms);
        }

        switch (channel()) {
            case Gitee: {
                constexpr auto URL = "https://gitee.com/mengyou1024/UnionOfflineInstaller/releases/latest";
                try {
                    std::unique_lock lock(m_paramMutex);
                    m_upgradeInterface = std::make_unique<Morose::Utils::UpgradeImpl::UpgradeFromGitee>(URL);
                    lock.unlock();
                } catch (const std::exception& e) {
                    qCWarning(TAG) << e.what();
                }
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
            return;
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

    bool AppUpdater::manualChecking() const {
        return m_manualChecking;
    }

    void AppUpdater::setManualChecking(bool newManualChecking) {
        if (m_manualChecking == newManualChecking)
            return;
        m_manualChecking = newManualChecking;
        emit manualCheckingChanged();
    }

    bool AppUpdater::uiInitOK() const {
        return m_uiInitOK;
    }

    void AppUpdater::setUiInitOK(bool newUiInitOK) {
        if (m_uiInitOK == newUiInitOK)
            return;
        m_uiInitOK = newUiInitOK;
        emit uiInitOKChanged();
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

    void AppUpdater::doManualCheckVersion() {
        if (!manualChecking()) {
            qCInfo(TAG) << "do manual check version";
            std::thread([this]() {
                setManualChecking(true);
                // 重新初始化接口
                initInterface();
                std::shared_lock lock(m_paramMutex);
                if (m_upgradeInterface == nullptr) {
                    emit versionCheckFailed(QObject::tr("无法检查更新:接口为空"));
                    emit manualCheckVersionResult({}, {}, {}, false);
                    setManualChecking(false);
                    return;
                }

                auto remove_version = m_upgradeInterface->getRemoteInstallerVersion();
                auto message        = m_upgradeInterface->getRemoteInstallerUpgradeInfo();
                auto app_version    = Version::AppVersion();
                bool need_update    = remove_version > app_version;

                setManualChecking(false);
                emit manualCheckVersionResult(app_version, remove_version, message, need_update);
            }).detach();

        } else {
            qCWarning(TAG) << "already in checking version";
        }
    }

    AppUpdater* AppUpdater::Instance() noexcept {
        static AppUpdater inst;
        return &inst;
    }

} // namespace Morose::Utils
