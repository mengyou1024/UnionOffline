#include "TofdPeInteractor.hpp"
#include "../common/common.hpp"
#include <ExcelRender.hpp>
#include <QLoggingCategory>
#include <QQmlProperty>
#include <QValueAxis>
#include <QtCore>

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

    bool TofdPeInteractor::hasPe() const {
        if (m_data != nullptr) {
            return m_data->hasPe();
        }
        return false;
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

    double TofdPeInteractor::getTofdDepth(double val) const {
        if (m_data == nullptr) {
            return Union::KeepDecimals<1>(Union::ValueMap(val, {0.0, 100.0}));
        }
        if (m_adjsutDepthFunc == std::nullopt) {
            if (m_adjsutDepthFunc == std::nullopt) {
                double half_pcs = m_data->getPCS() / 2;
                double time_mm  = val * m_data->getTofdRange() + m_data->getTofdDelay();
                double sign     = 1.0;
                if (half_pcs > time_mm) {
                    sign = -1.0;
                }
                return Union::KeepDecimals<1>(sign * std::sqrt(std::abs(std::pow(time_mm, 2) - std::pow(half_pcs, 2))));
            } else {
                return m_adjsutDepthFunc.value()(val);
            }
        } else {
            return m_adjsutDepthFunc.value()(val);
        }
    }

    double TofdPeInteractor::getVerticalAxisZeroPoint() const {
        if (m_data == nullptr) {
            return 0.0;
        }
        if (m_getVerticalAixsZero == std::nullopt) {
            double half_pcs = m_data->getPCS() / 2;
            return (half_pcs - m_data->getTofdDelay()) / m_data->getTofdRange();
        }
        return m_getVerticalAixsZero.value()();
    }

    void TofdPeInteractor::removeThroughWaveEvent(qreal x, qreal y, qreal w, qreal h) {
        if (m_data != nullptr) {
            try {
                m_data->removeThroughWaveEvent(x, y, w, h);
            } catch (std::exception& e) {
                qCCritical(TAG) << "去直通波:" << e.what();
            }
        }
    }

    void TofdPeInteractor::pullThroughWaveEvent(qreal x, qreal y, qreal w, qreal h) {
        if (m_data != nullptr) {
            try {
                m_data->pullThroughWaveEvent(x, y, w, h);
            } catch (std::exception& e) {
                qCCritical(TAG) << "拉直通波" << e.what();
            }
        }
    }

    QString TofdPeInteractor::fileName() const {
        return m_fileName;
    }

    void TofdPeInteractor::setFileName(const QString& newFileName) {
        if (m_fileName == newFileName)
            return;
        m_fileName = newFileName;
        emit fileNameChanged();
    }

    bool TofdPeInteractor::openFile(const QString& fileName) {
        m_file.clear();
        MOROSE_TEST_TIME_QUICK("open file:" + fileName);
        auto READ_FUNC = Union::TOFD_PE::TofdPeFileSelector::Instance()->GetReadFunction(fileName.toStdWString());
        if (!READ_FUNC.has_value()) {
            qCCritical(TAG) << "can't find read interface, file suffix:" << QFileInfo(fileName).suffix();
            return false;
        }
        try {
            for (const auto& func : READ_FUNC.value()) {
                m_data = func(fileName.toStdWString());
                if (m_data != nullptr) {
                    break;
                }
            }
        } catch (std::exception& e) {
            qCCritical(TAG) << e.what();
            m_data = nullptr;
        }
        m_getVerticalAixsZero = std::nullopt;
        m_adjsutDepthFunc     = std::nullopt;
        if (m_data != nullptr) {
            m_file = fileName;
            setFileName(QFileInfo(fileName).baseName());
            return true;
        }
        setFileName("");
        qCCritical(TAG) << "read file error, fileName:" << fileName;
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
        qCDebug(TAG) << "reportExport, filePath:" << filePath;
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
            QMap<QString, QImage> images = {};
            if (img) {
                images.insert("TOFD/PE", img->image());
            }

            auto result = Yo::File::Render::Excel::Render("excel_templates/TOFD_PE/T_报表生成.xlsx", filePath, map, images);
            qCInfo(TAG) << "保存报表:" << filePath;
            return result;
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

    void TofdPeInteractor::adjustDepth(qreal depth) {
        if (m_data == nullptr) {
            return;
        }

        m_getVerticalAixsZero = [=] { return depth; };

        m_adjsutDepthFunc = [=, this](double val) -> double {
            double distance   = depth * m_data->getTofdRange() + m_data->getTofdDelay();
            double current_mm = val * m_data->getTofdRange() + m_data->getTofdDelay();
            double sign       = 1.0;
            if (distance > current_mm) {
                sign = -1.0;
            }
            return Union::KeepDecimals<1>(sign * std::sqrt(std::abs(std::pow(current_mm, 2) - std::pow(distance, 2))));
        };
        emit updatePrivateData();
    }

    void TofdPeInteractor::rollBack() {
        qCDebug(TAG) << "撤销";
        if (m_data != nullptr) {
            m_data->rollback();
        }
        m_adjsutDepthFunc     = std::nullopt;
        m_getVerticalAixsZero = std::nullopt;
        emit updatePrivateData();
    }

} // namespace TOFD_PE
