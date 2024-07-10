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
         * @return
         */
        virtual Version getRemoteInstallerVersion() const = 0;

        /**
         * @brief 获取远程安装包的升级信息
         * @return
         */
        virtual QString getRemoteInstallerUpgradeInfo() const = 0;

        /**
         * @brief 获取远程安装包的下载地址
         * @return
         */
        virtual QString getRemoteInstallerDownloadUrl() const = 0;

        /**
         * @brief 下载远程安装包
         * @param file
         * @return
         */
        virtual bool downloadRemoteInstaller(QFile* file) const = 0;
    };

    class UpgradeInterfaceFactory : public QObject {
        Q_OBJECT

        using _SP_UI   = std::shared_ptr<UpgradeInterface>;
        using _F_V     = std::optional<std::function<bool(Version)>>;
        using _F_D     = std::optional<std::function<bool(QString)>>;
        using _SP_FILE = std::shared_ptr<QFile>;

        _SP_UI   m_interface                 = nullptr;
        _F_V     m_actionFoundNewVersion     = std::nullopt;
        _F_D     m_actionDownloadInstallerOk = std::nullopt;
        _SP_FILE m_file                      = nullptr;
        QThread* m_downloadThread            = nullptr;

        UpgradeInterfaceFactory() = default;

    public:
        Q_DISABLE_COPY_MOVE(UpgradeInterfaceFactory);

        enum class UpgradeInterfaceType {
            Gitee,
        };

        static UpgradeInterfaceFactory* Instance();

        void createInterface(UpgradeInterfaceType type);
        void checkForUpgrade();

    private:
        void startDownload();
    };

    QSystemTrayIcon& GetGlobakSystemTrayIcon();

} // namespace Morose::Utils
