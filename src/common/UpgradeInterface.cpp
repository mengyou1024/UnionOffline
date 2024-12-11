#include "UpgradeInterface.hpp"
#include "morose_config.h"
#include <QLoggingCategory>
#include <QStandardPaths>
#include <QSystemTrayIcon>

[[maybe_unused]] static Q_LOGGING_CATEGORY(TAG, "Upgrade.Intf");

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
        } else {
            qCCritical(TAG) << "create version error";
        }
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

    QSystemTrayIcon& GetGlobakSystemTrayIcon() {
        static QSystemTrayIcon inst(QIcon(":/img/morose.ico"));
        static std::once_flag  showFlag;
        std::call_once(showFlag, [&]() {
            inst.show();
        });
        qCDebug(TAG) << "get instance id:" << &inst;
        return inst;
    }

} // namespace Morose::Utils
