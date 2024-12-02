#pragma once
#include "UpgradeInterface.hpp"
#include <QFile>
#include <QtCore>

namespace Morose::Utils::UpgradeImpl {
    class UpgradeFromGitee : public UpgradeInterface {
        Version m_remoteVersion = {};
        QString m_upgradeInfo   = {};
        QString m_downloadUrl   = {};

    public:
        UpgradeFromGitee(QString url);

        virtual ~UpgradeFromGitee() noexcept = default;
        /**
         * @brief 获取远程安装包版本
         * @return 远程安装包版本
         */
        virtual Version getRemoteInstallerVersion() const override;

        /**
         * @brief 获取远程安装包的升级信息
         * @return 远程安装包的升级信息
         */
        virtual QString getRemoteInstallerUpgradeInfo() const override;

        /**
         * @brief 获取远程安装包的下载地址
         * @return 远程安装包的下载地址
         */
        virtual QString getRemoteInstallerDownloadUrl() const override;

        /**
         * @brief 下载远程安装包
         * @param file文件句柄
         * @return 下载成功返回true
         */
        virtual bool downloadRemoteInstaller(QFile* file) const override;
    };
} // namespace Morose::Utils::UpgradeImpl
