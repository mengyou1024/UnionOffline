#include "UpgradeFromGitee.hpp"
#include <QLoggingCategory>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <union_common.hpp>

[[maybe_unused]] static Q_LOGGING_CATEGORY(TAG, "Upgrade.Gitee");

namespace Morose::Utils::UpgradeImpl {

    UpgradeFromGitee::UpgradeFromGitee(QString url) {
        auto            manager = std::make_unique<QNetworkAccessManager>();
        QNetworkRequest req;
        req.setUrl(QUrl(url));
        req.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json;charset=UTF-8"));
        req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
        auto       reply = (*manager).get(req);
        QEventLoop eventLoop;
        QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
        eventLoop.exec(QEventLoop::AllEvents);
        [[maybe_unused]]
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode != 200) {
            throw std::runtime_error("无法访问服务器");
        }
        QString                   str = reply->readAll();
        static QRegularExpression reg_download_url(".+(/mengyou1024/UnionOfflineInstaller/.+?\\.exe)");
        auto                      match_download_url = reg_download_url.match(str);
        if (match_download_url.hasMatch()) {
            m_downloadUrl = "https://gitee.com" + match_download_url.captured(1);
            qCDebug(TAG) << "remote download url:" << m_downloadUrl;
        }
        static QRegularExpression reg_version(".+(v\\d+\\.\\d+\\.\\d+)");
        auto                      match_version = reg_version.match(m_downloadUrl);
        if (match_version.hasMatch()) {
            m_remoteVersion = Version(match_version.captured(1));
            qCDebug(TAG) << "remote version:" << m_remoteVersion.getVersonString();
        }

        static QRegularExpression reg_update_info(".+(/mengyou1024/UnionOfflineInstaller/.+?update_info\\.md)");
        auto                      match_update_info_url = reg_update_info.match(str);
        if (match_update_info_url.hasMatch()) {
            auto update_info_url = "https://gitee.com" + match_update_info_url.captured(1);
            qCDebug(TAG) << "remote update info url:" << update_info_url;
            QTemporaryFile file;
            if (file.open()) {
                qDebug(TAG).noquote() << "download update info file:" << file.fileName();
            } else {
                throw std::runtime_error("创建临时文件, 用于保存update info");
            }
            if (downloadRemoteFile(update_info_url, file)) {
                file.flush();
                file.seek(0);
                m_upgradeInfo = file.readAll();
                qCDebug(TAG) << "update info:" << m_upgradeInfo;
            }
            file.close();
        }
    }

    Version UpgradeFromGitee::getRemoteInstallerVersion() const {
        return m_remoteVersion;
    }

    QString UpgradeFromGitee::getRemoteInstallerUpgradeInfo() const {
        return m_upgradeInfo;
    }

    QString UpgradeFromGitee::getRemoteInstallerDownloadUrl() const {
        return m_downloadUrl;
    }

    bool UpgradeFromGitee::downloadRemoteInstaller(QFile& file, std::function<void(qreal)> progress) const {
        return downloadRemoteFile(m_downloadUrl, file, progress);
    }

    bool UpgradeFromGitee::downloadRemoteFile(const QString& url, QFile& file, std::optional<std::function<void(qreal)>> progress) const {
        auto            manager = std::make_unique<QNetworkAccessManager>();
        QNetworkRequest req_download;
        req_download.setUrl(url);
        req_download.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
        auto       reply = (*manager).get(req_download);
        QEventLoop eventLoop;
        bool       return_value = false;
        QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
        QObject::connect(reply, &QNetworkReply::downloadProgress, &eventLoop, [reply, &file, &return_value, progress](qint64 recvd, qint64 total) {
            auto download_done_rate = Union::KeepDecimals<3>(static_cast<double>(recvd) / total);
            qCDebug(TAG).nospace() << "download: " << download_done_rate * 100.0 << "% " << recvd << "/" << total;

            if (progress.has_value()) {
                progress.value()(download_done_rate);
            }

            if (recvd == total) {
                return_value = true;
            }

            file.write(reply->readAll());
            file.flush();
        });
        eventLoop.exec();
        return return_value;
    }

} // namespace Morose::Utils::UpgradeImpl
