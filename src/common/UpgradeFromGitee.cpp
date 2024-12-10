#include "UpgradeFromGitee.hpp"
#include <QLoggingCategory>
#include <QNetworkAccessManager>
#include <QNetworkReply>

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

    bool UpgradeFromGitee::downloadRemoteInstaller(QFile* file, std::function<void(qreal)> progress) const {
        auto            manager = std::make_unique<QNetworkAccessManager>();
        QNetworkRequest req_download;
        req_download.setUrl(m_downloadUrl);
        req_download.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
        auto       reply = (*manager).get(req_download);
        QEventLoop eventLoop;
        auto       ret  = std::make_shared<bool>(false);
        bool*      pRet = ret.get();
        QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
        QObject::connect(reply, &QNetworkReply::downloadProgress, &eventLoop, [reply, file, pRet, progress](qint64 recvd, qint64 total) {
            if (total > 0) {
                if (recvd == total) {
                    *pRet = true;
                    progress(1);
                    qCDebug(TAG) << "download installer done";
                }
                if (file != nullptr) {
                    qCDebug(TAG) << "download:" << recvd << "/" << total;
                    progress((qreal)recvd / (qreal)total);
                    file->write(reply->readAll());
                    file->flush();
                }
            }
        });
        eventLoop.exec();
        return *ret;
    }

} // namespace Morose::Utils::UpgradeImpl
