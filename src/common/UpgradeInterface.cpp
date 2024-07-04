#include "UpgradeInterface.hpp"
#include "../morose_config.h"
#include "UpgradeFromGitee.hpp"
#include <QLoggingCategory>
#include <QStandardPaths>
#include <QSystemTrayIcon>

static Q_LOGGING_CATEGORY(TAG, "Upgrade.Intf");

namespace Morose::Utils {

    Version::Version(int64_t major, int64_t minor, int64_t patch) {
        m_data = ((major & 0xFFFFll) << (32 + 16)) | ((minor & 0xFFFFll) << 32) | (patch & 0xFFFFFFFFll);
    }

    Version::Version(QString version) {
        static QRegularExpression reg("v?(\\d+)\\.(\\d+)\\.(\\d+)");
        auto                      match = reg.match(version);
        if (match.hasMatch()) {
            auto major = match.captured(1).toLongLong();
            auto minor = match.captured(2).toLongLong();
            auto patch = match.captured(3).toLongLong();
            m_data     = ((major & 0xFFFFll) << 48) | ((minor & 0xFFFFll) << 32) | (patch & 0xFFFFFFFFll);
        }
#if defined(QT_DEBUG)
        else {
            qFatal("create version error");
        }
#endif
    }

    QString Version::getVersonString() const {
        return QString("v%1.%2.%3").arg(getMajor()).arg(getMinor()).arg(getPatch());
    }

    const Version& Version::AppVersion() {
        static auto inst = Version(APP_VERSION);
        return inst;
    }

    qlonglong Version::getMajor() const {
        return (m_data & (0xFFFFull << 48)) >> 48;
    }

    qlonglong Version::getMinor() const {
        return (m_data & (0xFFFFull << 32)) >> 32;
    }

    qlonglong Version::getPatch() const {
        return (m_data & 0xFFFFFFFF);
    }

    UpgradeInterfaceFactory& UpgradeInterfaceFactory::Instance() {
        static UpgradeInterfaceFactory inst;
        return inst;
    }

    void UpgradeInterfaceFactory::createInterface(UpgradeInterfaceType type) {
        switch (type) {
            case UpgradeInterfaceType::Gitee: {
                constexpr auto URL = "https://gitee.com/mengyou1024/UnionOfflineInstaller/releases/latest";
                m_interface        = std::make_unique<Morose::Utils::UpgradeImpl::UpgradeFromGitee>(URL);
                break;
            }
            default:
                m_interface = nullptr;
                break;
        }
    }

    void UpgradeInterfaceFactory::checkForUpgrade() {
        if (m_interface == nullptr) {
            return;
        }
        // 1. 检查远程版本是否大于当前版本
#if !MOROSE_DEBUG_UPGRADE
        if (m_interface->getRemoteInstallerVersion() <= Version::AppVersion()) {
            return;
        }
#endif

        // 2. 弹出通知, 更新可用
        QString version_change;
        version_change = Version::AppVersion().getVersonString() + "-->" + m_interface->getRemoteInstallerVersion().getVersonString();
        version_change += "\n点击开始下载";
        auto& tray = GetGlobakSystemTrayIcon();
        tray.showMessage(QObject::tr("更新可用"), version_change);
        QObject::connect(
            &tray, &QSystemTrayIcon::messageClicked, this, [&tray, this]() {
                qDebug(TAG) << "开始下载";
                QString msg = "版本号:" + UpgradeInterfaceFactory::Instance().m_interface->getRemoteInstallerVersion().getVersonString();
                QObject::disconnect(&tray, &QSystemTrayIcon::messageClicked, nullptr, nullptr);
                tray.showMessage(QObject::tr("开始后台下载升级程序"), msg, QSystemTrayIcon::Information, 3000);
                m_downloadThread = QThread::create([this]() {
                    // 3. 下载安装包
                    startDownload();
                    m_file = nullptr;
                });
                m_downloadThread->start();
            });
    }

    void UpgradeInterfaceFactory::startDownload() {
        auto    remote_version = m_interface->getRemoteInstallerVersion().getVersonString();
        QString fileName       = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/UnionOfflineInstaller-" + remote_version + ".exe";
        auto&   tray           = GetGlobakSystemTrayIcon();
        m_file                 = std::make_shared<QFile>(fileName);
        m_file->open(QIODevice::WriteOnly);
        if (m_interface->downloadRemoteInstaller(m_file.get())) {
            tray.showMessage(QObject::tr("下载完成"), QObject::tr("点击开始安装"));
            QObject::connect(&tray, &QSystemTrayIcon::messageClicked, this, [this, &tray, fileName]() {
                m_downloadThread->wait();
                m_downloadThread->deleteLater();
                QProcess::startDetached(fileName, {});
                QObject::disconnect(&tray, &QSystemTrayIcon::messageClicked, nullptr, nullptr);
                qApp->postEvent(qApp, new QEvent(QEvent::Quit), Qt::LowEventPriority);
            });
        }
    }

    QSystemTrayIcon& GetGlobakSystemTrayIcon() {
        static QSystemTrayIcon inst(QIcon(":/img/morose.ico"));
        static std::once_flag  showFlag;
        std::call_once(showFlag, [&]() {
            inst.show();
        });
        return inst;
    }

} // namespace Morose::Utils
