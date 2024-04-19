#include "AScanInteractor.hpp"
#include "../common/common.hpp"
#include <QLoggingCategory>
#include <QQmlProperty>
#include <QValueAxis>
#include <QtCore>
#include <Yo/File>

static Q_LOGGING_CATEGORY(TAG, "AScanIntr");

using namespace Union::Base;
using namespace Union::AScan;
constexpr auto SCAN_LINE_WIDTH = 1.5;
constexpr auto GATE_LINE_WIDTH = 2.5;

bool AScanInteractor::getReplayVisible() const {
    return replayVisible;
}

void AScanInteractor::setReplayVisible(bool newReplayVisible) {
    if (replayVisible == newReplayVisible) {
        return;
    }
    replayVisible = newReplayVisible;
    emit replayVisibleChanged();
}

QString AScanInteractor::getDate() const {
    return date;
}

void AScanInteractor::setDate(const QString& newDate) {
    if (date == newDate) {
        return;
    }
    date = newDate;
    emit dateChanged();
}

int AScanInteractor::getSoftGain() const {
    return softGain;
}

void AScanInteractor::setSoftGain(int newSoftGain) {
    if (softGain == newSoftGain) {
        return;
    }
    softGain = newSoftGain;
    emit softGainChanged();
}

int AScanInteractor::getReplayValue() const {
    return replayValue;
}

void AScanInteractor::setReplayValue(int newReplayValue) {
    if (replayValue == newReplayValue) {
        return;
    }
    replayValue = newReplayValue;
    emit replayValueChanged();
}

QQuickItem* AScanInteractor::getChartView() const {
    return chartView;
}

void AScanInteractor::setChartView(QQuickItem* newChartView) {
    if (chartView == newChartView) {
        return;
    }
    chartView = newChartView;
    emit chartViewChanged();
}

bool AScanInteractor::reportExportClicked(QString _fileName, QQuickItemGrabResult* img) {
    qDebug(TAG) << __FUNCTION__;
    if (!checkAScanCursorValid()) {
        return false;
    }
    // TODO: 生成数据
    QVariantMap vmp = {
        {QObject::tr("检测单位"), ""},
        {QObject::tr("报告编写"), ""},
        {QObject::tr("委托单位"), ""},
        {QObject::tr("检测日期"), QString::fromStdString(ascan->getDate(getAScanCursor()))},
        {QObject::tr("名称"), ""},
        {QObject::tr("编号"), ""},
        {QObject::tr("表面热处理"), ""},
        {QObject::tr("材质"), ""},
        {QObject::tr("表面粗糙度"), ""},
        {QObject::tr("探头型号"), QString::fromStdWString(ascan->getProbe(getAScanCursor()))},
        {QObject::tr("晶片尺寸"), QString::fromStdString(ascan->getProbeChipShape(getAScanCursor()))},
        {QObject::tr("前沿"), QString::number(ascan->getFrontDistance(getAScanCursor()), 'f', 1)},
        {QObject::tr("探头K值"), QString::number(Probe::Degree2K(ascan->getAngle(getAScanCursor())), 'f', 2)},
        {QObject::tr("频率"), QString::number(ascan->getProbeFrequence(getAScanCursor()), 'f', 1)},
        {QObject::tr("折射角"), QString::number(ascan->getAngle(getAScanCursor()), 'f', 2)},
        {QObject::tr("零点"), QString::number(ascan->getZeroPointBias(getAScanCursor()), 'f', 1)},
        {QObject::tr("仪器型号"), QString::fromStdString(ascan->getInstrumentName())},
        {QObject::tr("灵敏度"), QString::number(ascan->getPerformance(getAScanCursor()).sensitivity, 'f', 1)},
        {QObject::tr("回波抑制"), QString::number(ascan->getSupression(getAScanCursor()))},
        {QObject::tr("回波延时"), QString::number(ascan->getSamplingDelay(getAScanCursor()), 'f', 1)},
        {QObject::tr("声程范围"), QString::number(ascan->getAxisLen(getAScanCursor()), 'f', 1)},
        {QObject::tr("声速"), QString::number(ascan->getSoundVelocity(getAScanCursor()), 'f', 0)},
        {QObject::tr("距离"), gateValue[0].toObject()["dist_c"].toString()},
        {QObject::tr("水平"), gateValue[0].toObject()["dist_a"].toString()},
        {QObject::tr("垂直"), gateValue[0].toObject()["dist_b"].toString()},
        {QObject::tr("当量"), gateValue[0].toObject()["equi"].toString()},
        {QObject::tr("长度"), ""},
        {QObject::tr("高度"), ""},
        {QObject::tr("等级"), ""},
        {QObject::tr("探伤结果"), ""},
        {QObject::tr("探伤标准"), ""},
        {QObject::tr("探伤人员"), ""},
        {QObject::tr("负责人员"), ""},
        {QObject::tr("备注"), ""},
    };
    auto result = Yo::File::Render::Excel::Render("excel_templates/T_报表生成.xlsx", _fileName, vmp);
    if (!result) {
        return result;
    }
    if (img) {
        QXlsx::Document doc(_fileName);
        qDebug(TAG) << "saveImage return:" << doc.insertImage(18, 0, img->image().scaled(667, 339, Qt::AspectRatioMode::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation));
        result = doc.save();
    }
    return result;
}

bool AScanInteractor::performanceClicked(QString _fileName) {
    qDebug(TAG) << __FUNCTION__;
    if (!checkAScanCursorValid()) {
        return false;
    }
    const auto& performance = ascan->getPerformance(getAScanCursor());
    QVariantMap vmp         = {
        {QObject::tr("仪器型号"), QString::fromStdString(ascan->getInstrumentName())},
        {QObject::tr("水平线性"), QString::number(performance.horizontalLinearity, 'f', 2)},
        {QObject::tr("垂直线性"), QString::number(performance.verticalLinearity, 'f', 2)},
        {QObject::tr("分辨力"), QString::number(performance.resolution, 'f', 1)},
        {QObject::tr("动态范围"), QString::number(performance.dynamicRange, 'f', 1)},
        {QObject::tr("灵敏度余量"), QString::number(performance.sensitivity, 'f', 1)},
        {QObject::tr("检测单位"), ""},
    };
    return Yo::File::Render::Excel::Render("excel_templates/T_仪器性能.xlsx", _fileName, vmp);
}

void AScanInteractor::gainValueModified(qreal val) {
    qDebug(TAG) << __FUNCTION__ << "val" << val;
}

void AScanInteractor::replayStartClicked(bool isStart) {
    qDebug(TAG) << __FUNCTION__ << "isStart" << isStart;
}

void AScanInteractor::replaySpeedClicked(int val) {
    qDebug(TAG) << __FUNCTION__ << "val" << val;
    replaySpeed = val;
}

void AScanInteractor::lastFrameClicked() {
    qDebug(TAG) << __FUNCTION__;
}

void AScanInteractor::nextFrameClicked() {
    qDebug(TAG) << __FUNCTION__;
}

void AScanInteractor::timeSliderMoved(qreal val) {
    qDebug(TAG) << __FUNCTION__ << "val" << val;
    setAScanCursor(static_cast<int>(val));
}

void AScanInteractor::seriesRemoved(QAbstractSeries* series) {
    qDebug(TAG) << __FUNCTION__;
    qDebug(TAG) << series;
    qDebug(TAG) << series->attachedAxes();
    for (auto& axis : series->attachedAxes()) {
        series->detachAxis(axis);
        axis->deleteLater();
    }
}

QVariantMap AScanInteractor::getTechnologicalParameter() {
    if (!checkAScanCursorValid()) {
        return {};
    }

    QVariantMap gainPrarameter = {
        {QObject::tr("基本增益"), QString::number(ascan->getBaseGain(getAScanCursor()),                'f', 1) + " dB"},
        {QObject::tr("扫查增益"), QString::number(ascan->getScanGain(getAScanCursor()),                'f', 1) + " dB"},
        {QObject::tr("表面补偿"), QString::number(ascan->getSurfaceCompentationGain(getAScanCursor()), 'f', 1) + " dB"},
    };

    QVariantMap probeParameter = {
        {QObject::tr("探头类型"), QString::fromStdWString(ascan->getProbe(getAScanCursor()))},
        {QObject::tr("探头频率"), QString::number(ascan->getProbeFrequence(getAScanCursor()), 'f', 1) + " MHz"},
        {QObject::tr("晶片尺寸"), QString::fromStdString(ascan->getProbeChipShape(getAScanCursor()))},
    };

    QVariantMap basicParameter = {
        {QObject::tr("声程"), QString::number(ascan->getAxisLen(getAScanCursor()), 'f', 1) + " mm"},
        {QObject::tr("前沿"), QString::number(ascan->getFrontDistance(getAScanCursor()), 'f', 1) + " mm"},
        {QObject::tr("零点"), QString::number(ascan->getZeroPointBias(getAScanCursor()), 'f', 1) + " ns"},
        {QObject::tr("延时"), QString::number(ascan->getSamplingDelay(getAScanCursor()), 'f', 1) + " mm"},
        {QObject::tr("声速"), QString::number(ascan->getSoundVelocity(getAScanCursor()), 'f', 0) + " m/s"},
        {QObject::tr("通道"), QString::number(ascan->getChannel(getAScanCursor()))},
        {QObject::tr("K值"), QString::number(Probe::Degree2K(ascan->getAngle(getAScanCursor())), 'f', 2)},
        {QObject::tr("抑制"), QString::number(ascan->getSupression(getAScanCursor())) + "%"},
        {QObject::tr("角度"), QString::number(ascan->getAngle(getAScanCursor()), 'f', 1) + "°"},
    };
    return {
        {QObject::tr("增益参数"), gainPrarameter},
        {QObject::tr("探头信息"), probeParameter},
        {QObject::tr("基本信息"), basicParameter},
    };
}

QVariantList AScanInteractor::getFileNameList() {
    if (ascan == nullptr) {
        return {};
    }
    auto         fileName_list = ascan->getFileNameList();
    QVariantList ret;
    for (const auto& fileName : fileName_list) {
        ret.push_back(QVariant(QString::fromStdWString(fileName)));
    }
    return ret;
}

void AScanInteractor::setFileNameIndex(int idx) {
    if (ascan != nullptr) {
        aScanCursor = 0; // 这里直接赋值而不是使用`setAScanCursor`是为了不触发相应的信号
        ascan->setFileNameIndex(idx);
        setAScanCursorMax(ascan->getDataSize() - 1);
        setDate(QString::fromStdString(ascan->getDate(getAScanCursor())));
        setReplayValue(0);
        changeDataCursor();
    }
}

void AScanInteractor::changeDataCursor() {
    if (std::cmp_less(getAScanCursor(), ascan ? ascan->getDataSize() : 0)) {
        MOROSE_TEST_TIME_QUICK("update all series");
        // 1. 更新A扫曲线
        updateAScanSeries();
        // 2. 更新波门曲线
        updateGateSeries<2>(ascan->getGate(getAScanCursor()));
        // 3. 更新DAC曲线
        updateQuadraticCurveSeries(QuadraticCurveSeriesType::DAC);
        // 4. 更新AVG曲线
        updateQuadraticCurveSeries(QuadraticCurveSeriesType::AVG);
        // 5. 更新AScan图的波门信息显示
        setGateValue(CreateGateValue());
        for (auto i = 0; i < seriesCount(); i++) {
            qDebug(TAG) << series(i)->name();
        }
        // 6. 更新显示的声程模式
        switch (ascan->getDistanceMode(getAScanCursor())) {
            case DistanceMode_Y:
                setDistanceMode("Y");
                break;
            case DistanceMode_X:
                setDistanceMode("X");
                break;
            case DistanceMode_S:
                setDistanceMode("S");
                break;
            default:
                setDistanceMode("N");
        }
    }
}

void AScanInteractor::updateCurrentFrame() {
    updateAScanSeries(ascan->getAScanSeriesData(getAScanCursor(), softGain), {ascan->getAxisBias(getAScanCursor()), 0}, {ascan->getAxisLen(getAScanCursor()), 100.0});
    updateQuadraticCurveSeries(QuadraticCurveSeriesType::DAC);
    updateQuadraticCurveSeries(QuadraticCurveSeriesType::AVG);
    setGateValue(CreateGateValue());
}

const QJsonArray AScanInteractor::getGateValue() const {
    return gateValue;
}

void AScanInteractor::setGateValue(const QJsonArray& newGateValue) {
    if (gateValue == newGateValue) {
        return;
    }
    gateValue = newGateValue;
    emit gateValueChanged();
}

int AScanInteractor::getAScanCursor() const {
    return aScanCursor;
}

void AScanInteractor::setAScanCursor(int newAScanCursor) {
    if (aScanCursor == newAScanCursor) {
        return;
    }
    aScanCursor = newAScanCursor;
    emit aScanCursorChanged();
}

int AScanInteractor::getAScanCursorMax() const {
    return aScanCursorMax;
}

void AScanInteractor::setAScanCursorMax(int newAScanCursorMax) {
    if (aScanCursorMax == newAScanCursorMax) {
        return;
    }
    aScanCursorMax = newAScanCursorMax;
    emit aScanCursorMaxChanged();
}

QString AScanInteractor::getDistanceMode() const {
    return distanceMode;
}

void AScanInteractor::setDistanceMode(const QString& newDistanceMode) {
    if (distanceMode == newDistanceMode)
        return;
    distanceMode = newDistanceMode;
    emit distanceModeChanged();
}

bool AScanInteractor::checkAScanCursorValid() {
    if (!std::cmp_greater(ascan ? ascan->getDataSize() : 0, getAScanCursorMax()) || !(getAScanCursorMax() >= 0)) {
        return false;
    }
    return true;
}

AScanInteractor::AScanInteractor() {
    QJsonObject gate1;
    gate1.insert("amp", "-");
    gate1.insert("dist_a", "-");
    gate1.insert("dist_b", "-");
    gate1.insert("dist_c", "-");
    gate1.insert("equi", "-");
    QJsonObject gate2;
    gate2.insert("amp", "-");
    gate2.insert("dist_a", "-");
    gate2.insert("dist_b", "-");
    gate2.insert("dist_c", "-");
    gate2.insert("equi", "-");

    QJsonArray arr;
    arr.append(gate1);
    arr.append(gate2);
    gateValue = arr;

    connect(this, &AScanInteractor::aScanCursorChanged, this, &AScanInteractor::changeDataCursor);
    connect(this, &AScanInteractor::softGainChanged, this, &AScanInteractor::updateCurrentFrame);
}

AScanInteractor::~AScanInteractor() {
    disconnect(this, &AScanInteractor::aScanCursorChanged, this, &AScanInteractor::changeDataCursor);
    disconnect(this, &AScanInteractor::softGainChanged, this, &AScanInteractor::updateCurrentFrame);
    qDebug() << __FUNCTION__;
}

void AScanInteractor::setDefaultValue() {
    ascan = {};
    setDate("");
    setAScanCursorMax(0);
    setAScanCursor(0);
    replaySpeed = 0;
    removeAllSeries();
    setDistanceMode("N");
}

bool AScanInteractor::openFile(QString _fileName) {
    setReplayVisible(false);
    auto func = Union::AScan::AScanFileSelector::Instance().GetReadFunction(_fileName.toStdWString());
    if (!func.has_value()) {
        QFileInfo info(_fileName);
        qWarning(TAG) << "can not find readFunc of :" << info.suffix();
        return false;
    }
    ascan = (func.value())(_fileName.toStdWString());
    if (!ascan) {
        qDebug() << "Readfile error:" << _fileName;
        return false;
    }
    if (ascan->getDataSize() > 1) {
        setReplayVisible(true);
    } else if (ascan->getDataSize() == 1) {
        setReplayVisible(false);
    } else {
        qWarning(TAG) << "no data on file:" << _fileName;
        return false;
    }
    qDebug(TAG) << "time:" << QString::fromStdString(ascan->getDate(getAScanCursor()));
    setDate(QString::fromStdString(ascan->getDate(getAScanCursor())));
    setAScanCursorMax(ascan->getDataSize() - 1);
    if (getAScanCursor() == 0) {
        changeDataCursor();
    }
    setAScanCursor(0);
    return true;
}

QAbstractSeries* AScanInteractor::createSeries(QAbstractSeries::SeriesType type, QString name, QAbstractAxis* axisX, QAbstractAxis* axisY) {
    QAbstractSeries* ret = nullptr;

    bool res = QMetaObject::invokeMethod(chartView, "createSeries", Q_RETURN_ARG(QAbstractSeries*, ret),
                                         Q_ARG(int, type),
                                         Q_ARG(QString, name),
                                         Q_ARG(QAbstractAxis*, axisX), Q_ARG(QAbstractAxis*, axisY));
    if (!res) {
        qCritical(TAG) << "invok method `createSeries` error with parameter:" << type << name << axisX << axisY;
    }
    if (ret) {
        ret->setUseOpenGL(true);
    }
    return ret;
}

void AScanInteractor::removeAllSeries() {
    for (int i = 0; i < seriesCount(); i++) {
        auto _series = series(i);
        for (auto& axis : _series->attachedAxes()) {
            _series->detachAxis(axis);
            axis->deleteLater();
        }
    }
    QMetaObject::invokeMethod(chartView, "removeAllSeries");
}

QLineSeries* AScanInteractor::createAScanSeries(QPointF pt, QSizeF sz) {
    auto CreateAScanAxis = [](QString fmt, qreal min = 0.0, qreal max = 1.0) {
        QValueAxis* axis = new QValueAxis;
        axis->setMin(min);
        axis->setMax(max);
        axis->setMinorTickCount(4);
        axis->setTickCount(6);
        axis->setLabelFormat(fmt);
        axis->setGridLinePen(QPen(QColor(0xAEAEAE), 1, Qt::SolidLine));
        axis->setMinorGridLinePen(QPen(QColor(0xD0D0D0), 1, Qt::DashLine));
        auto font = axis->labelsFont();
        font.setPointSize(18);
        axis->setLabelsFont(font);
        return axis;
    };
    QValueAxis* axisX = CreateAScanAxis("%.1fmm", pt.x(), pt.x() + sz.width());
    QValueAxis* axisY = CreateAScanAxis("%d%%", pt.y(), pt.y() + sz.height());
    auto        ret   = (QLineSeries*)createSeries(QAbstractSeries::SeriesTypeLine, ASCAN_SERIES_NAME, axisX, axisY);
    auto        pen   = ret->pen();
    pen.setColor(QColor(0x336666));
    pen.setWidthF(SCAN_LINE_WIDTH);
    ret->setPen(pen);
    return ret;
}

void AScanInteractor::updateAScanSeries(const QList<QPointF>& data, QPointF pt, QSizeF sz) {
    auto aScanSeries = (QLineSeries*)series(QString(ASCAN_SERIES_NAME));
    if (!aScanSeries) {
        aScanSeries = createAScanSeries(pt, sz);
    }
    auto axias = aScanSeries->attachedAxes();
    axias.at(0)->setMin(pt.x());
    axias.at(0)->setMax(pt.x() + sz.width());
    axias.at(1)->setMin(pt.y());
    axias.at(1)->setMax(pt.y() + sz.height());
    aScanSeries->replace(data);
}

void AScanInteractor::updateAScanSeries(void) {
    updateAScanSeries(ascan->getAScanSeriesData(getAScanCursor(), softGain), {ascan->getAxisBias(getAScanCursor()), 0}, {ascan->getAxisLen(getAScanCursor()), 100.0});
}

QLineSeries* AScanInteractor::createQuadraticCurveSeries(const QString& name, QPointF pt, QSizeF sz) {
    auto CreateAVGAxis = [](qreal min = 0.0, qreal max = 1.0) {
        QValueAxis* axis = new QValueAxis;
        axis->setMin(min);
        axis->setMax(max);
        axis->setVisible(false);
        return axis;
    };
    QValueAxis* axisX = CreateAVGAxis(pt.x(), pt.x() + sz.width());
    QValueAxis* axisY = CreateAVGAxis(pt.y(), pt.y() + sz.height());
    auto        ret   = (QLineSeries*)createSeries(QAbstractSeries::SeriesTypeLine, name, axisX, axisY);
    auto        pen   = ret->pen();
    pen.setWidthF(SCAN_LINE_WIDTH);
    ret->setPen(pen);
    return ret;
}

void AScanInteractor::updateQuadraticCurveSeries(QuadraticCurveSeriesType type) {
    auto CHECK_VALID = [&]() {
        // 检查数据指针是否有效
        if (!checkAScanCursorValid()) {
            return false;
        }
        // 检查曲线是否有值
        switch (type) {
            case QuadraticCurveSeriesType::DAC: {
                auto has_value = ascan->getDAC(getAScanCursor()).has_value();
                if (!has_value) {
                    for (auto i = 0; i < 4; i++) {
                        auto temp = (QLineSeries*)series(QString(DAC_SERIES_NAME).arg(getDACSeriesSubName(i)));
                        if (temp) {
                            temp->setVisible(false);
                        }
                    }
                }
                return has_value;
            }
            case QuadraticCurveSeriesType::AVG: {
                auto has_value = ascan->getAVG(getAScanCursor()).has_value();
                if (!has_value) {
                    for (auto i = 0; i < 4; i++) {
                        auto temp = (QLineSeries*)series(QString(AVG_SERIES_NAME).arg(getAVGSeriesSubName(i)));
                        if (temp) {
                            temp->setVisible(false);
                        }
                    }
                }
                return has_value;
            }
            default:
                throw std::runtime_error("QuadraticCurveSeriesType error");
        }
    };
    if (!CHECK_VALID()) {
        return;
    }

    // 获取曲线名称
    const auto lineName = [&](int index) -> QString {
        switch (type) {
            case QuadraticCurveSeriesType::DAC:
                return QString(DAC_SERIES_NAME).arg(getDACSeriesSubName(index));
            case QuadraticCurveSeriesType::AVG:
                return QString(AVG_SERIES_NAME).arg(getAVGSeriesSubName(index));
            default:
                throw std::runtime_error("QuadraticCurveSeriesType error");
        }
    };
    // 获取isSubLine
    const auto isSubLine = [&]() {
        switch (type) {
            case QuadraticCurveSeriesType::DAC:
                return ascan->getDAC(getAScanCursor())->isSubline;
            case QuadraticCurveSeriesType::AVG:
                return ascan->getAVG(getAScanCursor())->isSubline;
            default:
                throw std::runtime_error("QuadraticCurveSeriesType error");
        }
    }();
    // 获取曲线表达式
    const auto lineExpr = [&]() {
        switch (type) {
            case QuadraticCurveSeriesType::DAC:
                return ascan->getDACLineExpr(getAScanCursor());
            case QuadraticCurveSeriesType::AVG:
                return ascan->getAVGLineExpr(getAScanCursor());
            default:
                throw std::runtime_error("QuadraticCurveSeriesType error");
        }
    }();
    // 获取增益修改
    const auto modifyGain = [&](int index) {
        double xlBias = 0.0;
        double _bGain = 0.0;
        switch (type) {
            case QuadraticCurveSeriesType::DAC:
                _bGain = ascan->getDAC(getAScanCursor())->baseGain;
                break;
            case QuadraticCurveSeriesType::AVG:
                _bGain = ascan->getAVG(getAScanCursor())->baseGain;
                break;
            default:
                throw std::runtime_error("QuadraticCurveSeriesType error");
        }
        switch (index) {
            case 1:
                xlBias = ascan->getDACStandard(getAScanCursor()).rlBias;
                break;
            case 2:
                xlBias = ascan->getDACStandard(getAScanCursor()).slBias;
                break;
            case 3:
                xlBias = ascan->getDACStandard(getAScanCursor()).elBias;
        }
        switch (type) {
            case QuadraticCurveSeriesType::DAC:
                return ascan->getBaseGain(getAScanCursor()) + ascan->getScanGain(getAScanCursor()) - _bGain + xlBias + softGain;
            case QuadraticCurveSeriesType::AVG:
                return ascan->getBaseGain(getAScanCursor()) + ascan->getScanGain(getAScanCursor()) +
                       ascan->getSurfaceCompentationGain(getAScanCursor()) - _bGain + xlBias + softGain +
                       ascan->getAVG(getAScanCursor())->biasGain;
            default:
                throw std::runtime_error("QuadraticCurveSeriesType error");
        }
    };

    std::vector<QLineSeries*>   lines  = {};
    std::vector<QList<QPointF>> pts    = {};
    std::vector<int>            indexs = {};
    if (!isSubLine) {
        lines.resize(1);
        pts.resize(1);
        indexs = {0};
    } else {
        lines.resize(3);
        pts.resize(3);
        indexs = {1, 2, 3};
    }

    // 隐藏未使用的曲线
    for (auto i = 0; std::cmp_less(i, 4); i++) {
        auto line = (QLineSeries*)series(lineName(i));
        if (line) {
            auto find_result = std::find(indexs.begin(), indexs.end(), i);
            if (find_result == indexs.end()) {
                line->setVisible(false);
            }
        }
    }

    // 查找曲线
    for (auto i = 0; std::cmp_less(i, lines.size()); i++) {
        lines[i] = (QLineSeries*)series(lineName(indexs[i]));
        if (!lines[i]) {
            lines[i] = createQuadraticCurveSeries(lineName(indexs[i]), {0.0, 0.0}, {static_cast<qreal>(ascan->getScanData(getAScanCursor()).size()), 200.0});
        }
        lines[i]->setVisible();
    }

    MOROSE_TEST_TIME_QUICK("update QuadraticCurve series");
    // 填充数据
    for (int i = 0; std::cmp_less(i, ascan->getScanData(getAScanCursor()).size()); i++) {
        for (auto j = 0; std::cmp_less(j, pts.size()); j++) {
            pts[j].push_back(QPointF(i, Union::CalculateGainOutput(lineExpr(i), modifyGain(indexs[j]))));
        }
    }
    // 替换曲线数据
    for (auto i = 0; std::cmp_less(i, pts.size()); i++) {
        lines[i]->replace(pts[i]);
    }
}

QLineSeries* AScanInteractor::createGateSeries(int index) {
    auto CreateGateAxis = []() {
        QValueAxis* axis = new QValueAxis;
        axis->setMin(0);
        axis->setMax(1.0);
        axis->setVisible(false);
        return axis;
    };
    QValueAxis* axisX = CreateGateAxis();
    QValueAxis* axisY = CreateGateAxis();
    auto        line  = (QLineSeries*)createSeries(QAbstractSeries::SeriesTypeLine, QString(GATE_SERIES_NAME).arg(index), axisX, axisY);
    auto        pen   = line->pen();
    pen.setWidthF(GATE_LINE_WIDTH);
    if (index == 0) {
        pen.setColor({255, 0, 0});
    } else {
        pen.setColor({0, 255, 0});
    }
    line->setPen(pen);
    return line;
}

void AScanInteractor::updateGateSeries(Union::Base::Gate gate, int index) {
    QString gateName = QString(GATE_SERIES_NAME).arg(index);
    auto    line     = (QLineSeries*)series(gateName);

    if (!line) {
        line = createGateSeries(index);
    }
    QList<QPointF> gateList = {};
    if (gate.enable) {
        constexpr auto midify_bias = 0.005;
        gateList.append({gate.pos, gate.height + midify_bias});
        gateList.append({gate.pos + midify_bias, gate.height});
        gateList.append({gate.pos + gate.width - midify_bias, gate.height});
        gateList.append({gate.pos + gate.width, gate.height + midify_bias});
        line->replace(gateList);
    } else {
        line->clear();
    }
}

QJsonArray AScanInteractor::CreateGateValue() {
    std::array<QVariantMap, 2> _gateValue = {};
    for (auto i = 0; std::cmp_less(i, _gateValue.size()); i++) {
        _gateValue[i] = {
            {"amp",    "-"},
            {"dist_a", "-"},
            {"dist_b", "-"},
            {"dist_c", "-"},
            {"equi",   "-"},
        };
    }
    if (!checkAScanCursorValid()) {
        return QJsonArray::fromVariantList({_gateValue[0], _gateValue[1]});
    }
    for (auto i = 0; std::cmp_less(i, _gateValue.size()); i++) {
        auto info = ascan->getGateResult(getAScanCursor(), i);
        if (!info) {
            continue;
        }
        auto [loc, _amp] = info.value();
        double amp       = _amp;
        amp              = Union::CalculateGainOutput(amp, softGain);
        if (amp > 255.0) {
            amp = 255.0;
        }
        QString               _a = "-";
        QString               _b = "-";
        QString               _c = "-";
        std::optional<double> a  = std::nullopt;
        std::optional<double> b  = std::nullopt;
        std::optional<double> c  = std::nullopt;
        switch (ascan->getDistanceMode(getAScanCursor())) {
            case Union::AScan::DistanceMode::DistanceMode_Y: {
                b = Union::ValueMap(loc, ascan->getAxisRange(getAScanCursor()));
                if (std::abs(ascan->getAngle(getAScanCursor())) > 0.0001) {
                    a = b.value() / Union::Base::Probe::Degree2K(ascan->getAngle(getAScanCursor()));
                    c = b.value() / std::sin(Union::Base::Probe::Degree2Rd(ascan->getAngle(getAScanCursor())));
                }
                break;
            }
            case Union::AScan::DistanceMode::DistanceMode_X: {
                a = Union::ValueMap(loc, ascan->getAxisRange(getAScanCursor()));
                if (std::abs(ascan->getAngle(getAScanCursor())) > 0.0001) {
                    b = Union::Base::Probe::Degree2K(ascan->getAngle(getAScanCursor())) * b.value();
                    c = b.value() / std::cos(Union::Base::Probe::Degree2Rd(ascan->getAngle(getAScanCursor())));
                }
                break;
            }
            case Union::AScan::DistanceMode::DistanceMode_S: {
                c = Union::ValueMap(loc, ascan->getAxisRange(getAScanCursor()));
                if (std::abs(ascan->getAngle(getAScanCursor())) > 0.0001) {
                    a = c.value() * std::cos(Union::Base::Probe::Degree2Rd(ascan->getAngle(getAScanCursor())));
                    b = c.value() * std::sin(Union::Base::Probe::Degree2Rd(ascan->getAngle(getAScanCursor())));
                }
                break;
            }
        }

        if (a) {
            _a = QString::number(a.value(), 'f', 1);
        }

        if (b) {
            _b = QString::number(b.value(), 'f', 1);
        }

        if (c) {
            _c = QString::number(c.value(), 'f', 1);
        }

        QString _equi = "-";
        if (ascan->getDAC(getAScanCursor()) && b.has_value()) {
            auto r_amp      = Union::CalculateGainOutput(_amp, ascan->getSurfaceCompentationGain(getAScanCursor()));
            auto lineExpr   = ascan->getDACLineExpr(getAScanCursor());
            auto slValue    = lineExpr((b.value() - ascan->getAxisBias(getAScanCursor())) / ascan->getAxisLen(getAScanCursor()) * ascan->getScanData(getAScanCursor()).size());
            auto modifyGain = ascan->getBaseGain(getAScanCursor()) + ascan->getScanGain(getAScanCursor()) +
                              ascan->getSurfaceCompentationGain(getAScanCursor()) - ascan->getDAC(getAScanCursor())->baseGain +
                              ascan->getDACStandard(getAScanCursor()).slBias;
            slValue = Union::CalculateGainOutput(slValue, modifyGain);
            _equi   = QString::asprintf("SL%+.1fdB", Union::CalculatedGain(slValue, r_amp));
        } else if (ascan->getAVG(getAScanCursor()) && b.has_value()) {
            auto r_amp      = Union::CalculateGainOutput(_amp, ascan->getSurfaceCompentationGain(getAScanCursor()));
            auto lineExpr   = ascan->getAVGLineExpr(getAScanCursor());
            auto slValue    = lineExpr((b.value() - ascan->getAxisBias(getAScanCursor())) / ascan->getAxisLen(getAScanCursor()) * ascan->getScanData(getAScanCursor()).size());
            auto modifyGain = ascan->getBaseGain(getAScanCursor()) + ascan->getScanGain(getAScanCursor()) +
                              ascan->getSurfaceCompentationGain(getAScanCursor()) - ascan->getAVG(getAScanCursor())->baseGain +
                              ascan->getDACStandard(getAScanCursor()).slBias;
            slValue = Union::CalculateGainOutput(slValue, modifyGain);
            _equi   = QString::asprintf("Φ%+.1fdB", Union::CalculatedGain(slValue, r_amp));
        }

        auto _gate_amp = amp / 2.0;
        if (_gate_amp > 100.0) {
            _gate_amp = 100.0;
        }

        _gateValue[i] = {
            {"amp", QString::number(_gate_amp, 'f', 1)},
            {"dist_a", _a},
            {"dist_b", _b},
            {"dist_c", _c},
            {"equi", _equi},
        };
    }

    return QJsonArray::fromVariantList({_gateValue[0], _gateValue[1]});
}

template <int N>
void AScanInteractor::updateGateSeries(std::array<Union::Base::Gate, N> gate) {
    for (auto i = 0; std::cmp_less(i, gate.size()); i++) {
        updateGateSeries(gate[i], i);
    }
}

QAbstractSeries* AScanInteractor::series(const QString& name) {
    QAbstractSeries* ret = nullptr;
    QMetaObject::invokeMethod(chartView, "series", Q_RETURN_ARG(QAbstractSeries*, ret), Q_ARG(QString, name));
    return ret;
}

QAbstractSeries* AScanInteractor::series(int index) {
    QAbstractSeries* ret = nullptr;
    QMetaObject::invokeMethod(chartView, "series", Q_RETURN_ARG(QAbstractSeries*, ret), Q_ARG(int, index));
    return ret;
}

int AScanInteractor::seriesCount() const {
    return chartView->property("count").toInt();
}
