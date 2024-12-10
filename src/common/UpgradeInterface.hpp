#pragma once
#include <QFile>
#include <QSystemTrayIcon>
#include <QtCore>

namespace Morose::Utils {
    class Version {
    private:
        quint64 m_data = {};

    public:
        Version() = default;
        explicit Version(int64_t major, int64_t minor = 0, int64_t patch = 0);
        explicit Version(QString version);

        QString getVersonString() const;

        auto operator<=>(const Version&) const = default;

        static const Version& AppVersion();

    private:
        qlonglong getMajor() const;
        qlonglong getMinor() const;
        qlonglong getPatch() const;
    };

    class UpgradeInterface {
    public:
        virtual ~UpgradeInterface() noexcept = default;
        /**
         * @brief 获取远程安装包版本
         * @return Version
         */
        virtual Version getRemoteInstallerVersion() const = 0;

        /**
         * @brief 获取远程安装包的升级信息
         * @return QString
         */
        virtual QString getRemoteInstallerUpgradeInfo() const = 0;

        /**
         * @brief 获取远程安装包的下载地址
         * @return QString
         */
        virtual QString getRemoteInstallerDownloadUrl() const = 0;

        /**
         * @brief 下载远程安装包
         * @param file 下载文件句柄
         * @param progress 下载进度
         * @return bool
         */
        virtual bool downloadRemoteInstaller(QFile* file, std::function<void(qreal)> progress) const = 0;
    };

    QSystemTrayIcon& GetGlobakSystemTrayIcon();

} // namespace Morose::Utils
