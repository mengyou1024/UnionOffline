#include "TofdPeInteractor.hpp"
#include "../common/common.hpp"
#include <QLoggingCategory>
#include <QQmlProperty>
#include <QValueAxis>
#include <QtCore>
#include <Yo/File>

static Q_LOGGING_CATEGORY(TAG, "TOFD_PE.INTR");

namespace TOFD_PE {
    qreal TofdPeInteractor::tofdSpace() const {
        return m_tofdSpace;
    }

    void TofdPeInteractor::setTofdSpace(qreal newTofdSpace) {
        if (qFuzzyCompare(m_tofdSpace, newTofdSpace))
            return;
        m_tofdSpace = newTofdSpace;
        emit tofdSpaceChanged();
    }

    qreal TofdPeInteractor::peSpace() const {
        return m_peSpace;
    }

    void TofdPeInteractor::setPeSpace(qreal newPeSpace) {
        if (qFuzzyCompare(m_peSpace, newPeSpace))
            return;
        m_peSpace = newPeSpace;
        emit peSpaceChanged();
    }

    const std::optional<Union::__TOFD_PE::Data>& TofdPeInteractor::getData() const {
        return m_data;
    }

    const std::optional<Union::__TOFD_PE::SubData>& TofdPeInteractor::getSubData() const {
        return m_data->subData;
    }

    bool TofdPeInteractor::openFile(const QString& fileName) {
        MOROSE_TEST_TIME_QUICK("open file:" + fileName);
        m_data = Union::__TOFD_PE::Data::FromFile(fileName);
        return m_data.has_value();
    }

    int TofdPeInteractor::getLines() const {
        if (m_data.has_value()) {
            return m_data->line + 1;
        } else {
            return -1;
        }
    }

    int TofdPeInteractor::getSubLines() const {
        if (m_data.has_value() && m_data->subData.has_value()) {
            return m_data->subData->line + 1;
        } else {
            return -1;
        }
    }

    bool TofdPeInteractor::reportExport(QString filePath) {
        qDebug(TAG) << "reportExport, filePath:" << filePath;
        return false;
    }

} // namespace TOFD_PE
