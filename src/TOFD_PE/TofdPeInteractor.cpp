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
        qDebug(TAG) << "去直通波";
        auto& _data = m_data;
        if (_data.has_value()) {
            for (auto i = static_cast<int>(x * _data->line + 1.5); std::cmp_less_equal(i, static_cast<int>((x + w) * _data->line + 1.5)); i++) {
                for (auto j = static_cast<int>(y * 500.0 + 1.5); std::cmp_less_equal(j, static_cast<int>((y + h) * 500.0 + 1.5)); j++) {
                    _data->data[i * 500 + j] += -_data->data[static_cast<int>(x * _data->line + 0.5) * 500 + j] + 128;
                }
            }
        }
    }

    void TofdPeInteractor::pullThroughWaveEvent(qreal x, qreal y, qreal w, qreal h) {
        // TODO: 实现函数
        qDebug(TAG) << "拉直通波";
    }

    const std::optional<Union::__TOFD_PE::Data>& TofdPeInteractor::getData() const {
        return m_data;
    }

    const std::optional<Union::__TOFD_PE::SubData>& TofdPeInteractor::getSubData() const {
        return m_data->subData;
    }

    bool TofdPeInteractor::openFile(const QString& fileName) {
        MOROSE_TEST_TIME_QUICK("open file:" + fileName);
        m_data       = Union::__TOFD_PE::Data::FromFile(fileName);
        m_dataMirror = std::nullopt;
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

    int TofdPeInteractor::getMaxLines() const {
        return std::max(getLines(), getSubLines());
    }

    bool TofdPeInteractor::reportExport(QString filePath) const {
        qDebug(TAG) << "reportExport, filePath:" << filePath;
        return false;
    }

    qreal TofdPeInteractor::getDelay() const {
        if (!m_data.has_value()) {
            return 0;
        }
        return m_data->delay / 100.0;
    }

    qreal TofdPeInteractor::getRange() const {
        if (!m_data.has_value()) {
            return 0;
        }
        return m_data->range / 10.0;
    }

    qreal TofdPeInteractor::getSubRange() const {
        if (!m_data.has_value() || !m_data->subData.has_value()) {
            return 0;
        }
        return m_data->subData->range / 300.0;
    }

    qreal TofdPeInteractor::getSubAngle() const {
        if (!m_data.has_value() || !m_data->subData.has_value()) {
            return 0;
        }
        return m_data->subData->angle / 10.0;
    }

    void TofdPeInteractor::throughWaveEvent(qreal x, qreal y, qreal w, qreal h, bool isPull) {
        if (!m_dataMirror.has_value()) {
            m_dataMirror = m_data;
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
        if (m_dataMirror.has_value()) {
            m_data = m_dataMirror.value();
            emit updatePrivateData();
        }
    }

} // namespace TOFD_PE
