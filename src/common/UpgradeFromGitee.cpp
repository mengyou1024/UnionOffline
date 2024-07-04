#include "UpgradeFromGitee.hpp"
#include "GlobalCppProgress.hpp"
#include <QLoggingCategory>
#include <QNetworkAccessManager>
#include <QNetworkReply>

static Q_LOGGING_CATEGORY(TAG, "Upgrade.Gitee");

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
        eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << "statusCode:" << statusCode;
        QString                   str = reply->readAll();
        static QRegularExpression reg_download_url(".+(/mengyou1024/UnionOfflineInstaller/.+?\\.exe)");
        auto                      match_download_url = reg_download_url.match(str);
        if (match_download_url.hasMatch()) {
            m_downloadUrl = "https://gitee.com" + match_download_url.captured(1);
            qDebug(TAG) << "remote download url:" << m_downloadUrl;
        }
        static QRegularExpression reg_version(".+(v\\d+\\.\\d+\\.\\d+)");
        auto                      match_version = reg_version.match(m_downloadUrl);
        if (match_version.hasMatch()) {
            m_remoteVersion = Version(match_version.captured(1));
            qDebug(TAG) << "remote version:" << m_remoteVersion.getVersonString();
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

    bool UpgradeFromGitee::downloadRemoteInstaller(QFile* file) const {
        auto            manager = std::make_unique<QNetworkAccessManager>();
        QNetworkRequest req_download;
        req_download.setUrl(m_downloadUrl);
        req_download.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
        auto       reply = (*manager).get(req_download);
        QEventLoop eventLoop;
        auto       ret  = std::make_shared<bool>(false);
        bool*      pRet = ret.get();
        QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
        GlobalCppProgress::Instance()->setEnable(true);
        QObject::connect(reply, &QNetworkReply::downloadProgress, &eventLoop, [reply, file, pRet](qint64 recvd, qint64 total) {
            if (total > 0) {
                if (recvd == total) {
                    *pRet = true;
                    GlobalCppProgress::Instance()->setEnable(false);
                    qDebug(TAG) << "download installer done";
                }
                if (file != nullptr) {
                    qDebug(TAG) << "download:" << recvd << "/" << total;
                    GlobalCppProgress::Instance()->setProgress((qreal)recvd / (qreal)total);
                    file->write(reply->readAll());
                    file->flush();
                }
            }
        });
        eventLoop.exec();
        return *ret;
    }

} // namespace Morose::Utils::UpgradeImpl
