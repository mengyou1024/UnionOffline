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

    const QJsonObject TofdPeInteractor::getTofdParam() const {
        if (m_data != nullptr) {
            return m_data->getTofdParam();
        }
        return QJsonObject();
    }

    const QJsonObject TofdPeInteractor::getPeParam() const {
        if (m_data != nullptr) {
            return m_data->getPeParam();
        }
        return QJsonObject();
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
        m_file.clear();
        MOROSE_TEST_TIME_QUICK("open file:" + fileName);
        auto READ_FUNC = Union::TOFD_PE::TofdPeFileSelector::Instance().GetReadFunction(fileName.toStdWString());
        if (!READ_FUNC.has_value()) {
            return false;
        }
        m_data = (READ_FUNC.value())(fileName.toStdWString());
        if (m_data != nullptr) {
            m_file = fileName;
            return true;
        }
        return false;
    }

    int TofdPeInteractor::getAScanSize() const {
        if (m_data != nullptr) {
            return m_data->getAScanSize();
        }
        return -1;
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

    bool TofdPeInteractor::reportExport(QString filePath, QQuickItemGrabResult* img) const {
        using Union::TOFD_PE::TofdPeIntf;
        qDebug(TAG) << "reportExport, filePath:" << filePath;
        if (!m_file.isEmpty() || m_data == nullptr) {
            QVariantMap map = {
                {"FileName", m_file},
                {"Volitage", m_data->getEmitVoltage()},
                {"RepeatFreq", QString::number(m_data->getRepeatFreq()) + "Hz"},
                {"ScanMode", m_data->getScanMode()},
                {"TriggerMode", m_data->getTriggerMode()},
                {"ScanIncrement", QString::number(m_data->getScanIncrement(), 'f', 2) + "mm"},
                {"WorkPieceThickness", QString::number(m_data->getThicknessOfWorkPiece(), 'f', 1) + "mm"},
                {"WeldWidth", QString::number(m_data->getTofdWeldWidth(), 'f', 1) + "mm"},
                {"ChannelParam", "(发射1 接收1)"},
                {"PCS", QString::number(m_data->getPCS(), 'f', 1) + "mm"},
                {"TofdGain", QString::number(m_data->getTofdGain(), 'f', 1) + "dB"},
                {"TofdRange", QString::number(TofdPeIntf::mm2us(m_data->getTofdRange()), 'f', 1) + "μs"},
                {"TofdDelay", QString::number(m_data->getTofdDelay(), 'f', 1) + "μs"},
                {"FilterBand", m_data->getFilterBand()},
                {"DetectionMode", m_data->getDetectionMode()},
                {"ProbeType", "一发一收"},
                {"TofdProbeSize", m_data->getTofdProbeSize()},
                {"TofdFreq", QString::number(m_data->getTofdProbeFreq(), 'f', 1) + "MHz"},
                {"TofdAngle", QString::number(m_data->getTofdAngle(), 'f', 1) + "°"},
                {"TofdFront", QString::number(m_data->getTofdProbeFront(), 'f', 1) + "mm"},
                {"TofdZeroPoint", QString::number(m_data->getTofdZeroPoint(), 'f', 2) + "μs"},
                {"PeGain", QString::number(m_data->getPeGain(), 'f', 1) + "dB"},
                {"PeRange", QString::number(m_data->getPeRange(), 'f', 1) + "mm"},
                {"PeFront", QString::number(m_data->getPeProbeFront(), 'f', 1) + "mm"},
                {"PeZeroPoint", QString::number(m_data->getPeZeroPoint(), 'f', 2) + "μs"},
                {"PeFreq", QString::number(m_data->getPeProbeFreq(), 'f', 1) + "MHz"},
                {"PeAngle", QString::number(m_data->getPeAngle(), 'f', 1) + "°"},
                {"PeProbeSize", m_data->getPeCrystalPlate()},
            };

            auto result = Yo::File::Render::Excel::Render("excel_templates/TOFD_PE/T_报表生成.xlsx", filePath, map);
            if (!result) {
                return false;
            }
            if (img) {
                QXlsx::Document doc(filePath);
                qDebug(TAG) << "Svae image return:" << doc.insertImage(13, 0, img->image());
                return doc.save();
            }
        }
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
        emit updatePrivateData();
    }

} // namespace TOFD_PE
