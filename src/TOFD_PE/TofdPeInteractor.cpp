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

    void TofdPeInteractor::removeThroughWaveEvent(qreal x, qreal y, qreal w, qreal h) {
        if (m_data != nullptr) {
            m_data->removeThroughWaveEvent(x, y, w, h);
        }
    }

    void TofdPeInteractor::pullThroughWaveEvent(qreal x, qreal y, qreal w, qreal h) {
        if (m_data != nullptr) {
            m_data->pullThroughWaveEvent(x, y, w, h);
        }
    }

    bool TofdPeInteractor::openFile(const QString& fileName) {
        MOROSE_TEST_TIME_QUICK("open file:" + fileName);
        auto READ_FUNC = Union::TOFD_PE::TofdPeFileSelector::Instance().GetReadFunction(fileName.toStdWString());
        if (!READ_FUNC.has_value()) {
            return false;
        }
        m_data = (READ_FUNC.value())(fileName.toStdWString());
        return m_data != nullptr;
    }

    int TofdPeInteractor::getLines() const {
        if (m_data != nullptr) {
            return m_data->getTofdLines();
        } else {
            return -1;
        }
    }

    int TofdPeInteractor::getSubLines() const {
        if (m_data && m_data->hasPe()) {
            return m_data->getPeLines();
        } else {
            return -1;
        }
    }

    int TofdPeInteractor::getMaxLines() const {
        return std::max(getLines(), getSubLines());
    }

    bool TofdPeInteractor::reportExport(QString filePath) const {
        qDebug(TAG) << "reportExport, filePath:" << filePath;
        return false;
    }

    const std::vector<uint8_t>& TofdPeInteractor::getTofdData() const {
        return m_data->getTofdData();
    }

    const std::vector<uint8_t>& TofdPeInteractor::getPeData() const {
        return m_data->getPeData();
    }

    qreal TofdPeInteractor::getDelay() const {
        if (m_data == nullptr) {
            return 0;
        }
        return m_data->getTofdDelay();
    }

    qreal TofdPeInteractor::getRange() const {
        if (m_data == nullptr) {
            return 0;
        }
        return m_data->getTofdRange();
    }

    qreal TofdPeInteractor::getSubRange() const {
        if (m_data == nullptr || !m_data->hasPe()) {
            return 0;
        }
        return m_data->getPeRange();
    }

    qreal TofdPeInteractor::getSubAngle() const {
        if (m_data == nullptr || !m_data->hasPe()) {
            return 0;
        }
        return m_data->getPeAngle();
    }

    void TofdPeInteractor::throughWaveEvent(qreal x, qreal y, qreal w, qreal h, bool isPull) {
        if (m_data != nullptr) {
            m_data->backup();
        }
        if (isPull) {
            pullThroughWaveEvent(x, y, w, h);
        } else {
            removeThroughWaveEvent(x, y, w, h);
        }
        emit updatePrivateData();
    }

    void TofdPeInteractor::rollBack() {
        qDebug(TAG) << "撤销";
        if (m_data != nullptr) {
            m_data->rollback();
        }
    }

} // namespace TOFD_PE
