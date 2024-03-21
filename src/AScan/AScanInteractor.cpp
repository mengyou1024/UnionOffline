#include "AScanInteractor.hpp"
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

#if defined(QT_DEBUG)
struct _TEST_TIME {
    QElapsedTimer mstimer;
    QString       msg;
    _TEST_TIME(const QString& _msg) :
    msg(_msg) {
        mstimer.start();
    }
    ~_TEST_TIME() {
        auto spend = mstimer.nsecsElapsed();
        qDebug(TAG) << "test time:" << QString::number(spend / 1000000.0, 'f', 2).toStdString().c_str() << "ms," << msg.toStdString().c_str();
    }
};
#else
struct _TEST_TIME {
    _TEST_TIME(const QString&) {}
    ~_TEST_TIME() {}
};
#endif

#define TEST_TIME_QUICK(x) auto __##__LINE__##__FILE = _TEST_TIME(x)

#define TEST_TIME_START(x) \
    {                      \
        auto __##__LINE__##__FILE = _TEST_TIME(x)

#define TEST_TIME_END() }

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

QString AScanInteractor::getFileName() const {
    return fileName;
}

void AScanInteractor::setFileName(const QString& newFileName) {
    if (fileName == newFileName) {
        return;
    }
    fileName = newFileName;
    emit fileNameChanged();
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
    updateAScanSeries(ascan.data[aScanCurosr]);
    updateQuadraticCurveSeries(QuadraticCurveSeriesType::DAC);
    updateQuadraticCurveSeries(QuadraticCurveSeriesType::AVG);
    setGateValue(CreateGateValue());
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
    const auto& ch = ascan.data[getAScanCursorMax()];
    // TODO: 生成数据
    QVariantMap vmp = {
        {QObject::tr("检测单位"), ""},
        {QObject::tr("报告编写"), ""},
        {QObject::tr("委托单位"), ""},
        {QObject::tr("检测日期"), QString::fromStdString(ascan.time)},
        {QObject::tr("名称"), ""},
        {QObject::tr("编号"), ""},
        {QObject::tr("表面热处理"), ""},
        {QObject::tr("材质"), ""},
        {QObject::tr("表面粗糙度"), ""},
        {QObject::tr("探头型号"), QString::fromStdWString(ascan.probe)},
        {QObject::tr("晶片尺寸"), QString::fromStdString(ascan.probeChipShape)},
        {QObject::tr("探头类型"), QString::fromStdWString(ascan.probe)},
        {QObject::tr("前沿"), QString::number(ascan.frontDistance, 'f', 1)},
        {QObject::tr("探头K值"), QString::number(Probe::Degree2K(ascan.angle), 'f', 2)},
        {QObject::tr("频率"), QString::number(ascan.probeFrequence, 'f', 1)},
        {QObject::tr("折射角"), QString::number(ascan.angle, 'f', 2)},
        {QObject::tr("零点"), QString::number(ascan.zeroPointBias, 'f', 1)},
        {QObject::tr("仪器型号"), QString::fromStdString(ascan.instrumentName)},
        {QObject::tr("灵敏度"), QString::number(ascan.performance.sensitivity, 'f', 1)},
        {QObject::tr("回波抑制"), QString::number(ch.suppression)},
        {QObject::tr("回波延时"), QString::number(ascan.samplingDelay, 'f', 1)},
        {QObject::tr("声程范围"), QString::number(ch.axisLen, 'f', 1)},
        {QObject::tr("声速"), QString::number(ascan.soundVelocity, 'f', 0)},
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
    QVariantMap vmp = {
        {QObject::tr("仪器型号"), QString::fromStdString(ascan.instrumentName)},
        {QObject::tr("水平线性"), QString::number(ascan.performance.horizontalLinearity, 'f', 2)},
        {QObject::tr("垂直线性"), QString::number(ascan.performance.verticalLinearity, 'f', 2)},
        {QObject::tr("分辨力"), QString::number(ascan.performance.resolution, 'f', 1)},
        {QObject::tr("动态范围"), QString::number(ascan.performance.dynamicRange, 'f', 1)},
        {QObject::tr("灵敏度余量"), QString::number(ascan.performance.sensitivity, 'f', 1)},
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
    setAScanCurosr((size_t)(val));
}

void AScanInteractor::seriesRemoved(QAbstractSeries* series) {
    qDebug(TAG) << __FUNCTION__;
    qDebug(TAG) << series;
    qDebug(TAG) << series->attachedAxes();
    for (auto axis : series->attachedAxes()) {
        series->detachAxis(axis);
        axis->deleteLater();
    }
}

QVariantMap AScanInteractor::getTechnologicalParameter() {
    if (!checkAScanCursorValid()) {
        return {};
    }

    const auto& ch = ascan.data[getAScanCursorMax()];

    QVariantMap gainPrarameter = {
        {QObject::tr("基本增益"), QString::number(ch.baseGain,                'f', 1) + " dB"},
        {QObject::tr("扫查增益"), QString::number(ch.scanGain,                'f', 1) + " dB"},
        {QObject::tr("表面补偿"), QString::number(ch.surfaceCompentationGain, 'f', 1) + " dB"},
    };

    QVariantMap probeParameter = {
        {QObject::tr("探头类型"), QString::fromStdWString(ascan.probe)},
        {QObject::tr("探头频率"), QString::number(ascan.probeFrequence, 'f', 1) + " MHz"},
        {QObject::tr("晶片尺寸"), QString::fromStdString(ascan.probeChipShape)},
    };

    QVariantMap basicParameter = {
        {QObject::tr("声程"), QString::number(ch.axisLen, 'f', 1) + " mm"},
        {QObject::tr("前沿"), QString::number(ascan.frontDistance, 'f', 1) + " mm"},
        {QObject::tr("零点"), QString::number(ascan.zeroPointBias, 'f', 1) + " ns"},
        {QObject::tr("延时"), QString::number(ascan.samplingDelay, 'f', 1) + " mm"},
        {QObject::tr("声速"), QString::number(ascan.soundVelocity, 'f', 0) + " m/s"},
        {QObject::tr("通道"), QString::number(ascan.channel)},
        {QObject::tr("K值"), QString::number(Probe::Degree2K(ascan.angle), 'f', 2)},
        {QObject::tr("抑制"), QString::number(ch.suppression) + "%"},
        {QObject::tr("角度"), QString::number(ascan.angle, 'f', 1) + "°"},
    };
    return {
        {QObject::tr("增益参数"), gainPrarameter},
        {QObject::tr("探头信息"), probeParameter},
        {QObject::tr("基本信息"), basicParameter},
    };
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

size_t AScanInteractor::getAScanCurosr() const {
    return aScanCurosr;
}

void AScanInteractor::setAScanCurosr(size_t newAScanCurosr) {
    if (newAScanCurosr < ascan.data.size()) {
        TEST_TIME_QUICK("update all series");
        // 1. 更新A扫曲线
        updateAScanSeries(ascan.data[newAScanCurosr]);
        // 2. 更新波门曲线
        updateGateSeries<2>(ascan.data[newAScanCurosr].gate);
        // 3. 更新DAC曲线
        updateQuadraticCurveSeries(QuadraticCurveSeriesType::DAC);
        // 4. 更新AVG曲线
        updateQuadraticCurveSeries(QuadraticCurveSeriesType::AVG);
        // 5. 更新AScan图的波门信息显示
        setGateValue(CreateGateValue());
        // 6. 更新显示的声程模式
        switch (ascan.data[newAScanCurosr].distanceMode) {
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
    if (aScanCurosr == newAScanCurosr) {
        return;
    }
    aScanCurosr = newAScanCurosr;
    emit aScanCurosrChanged();
}

size_t AScanInteractor::getAScanCursorMax() const {
    return aScanCursorMax;
}

void AScanInteractor::setAScanCursorMax(size_t newAScanCursorMax) {
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
    if (!(ascan.data.size() > getAScanCursorMax()) || !(getAScanCursorMax() >= 0)) {
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
}

AScanInteractor::~AScanInteractor() {
    qDebug() << __FUNCTION__;
}

void AScanInteractor::setDefaultValue() {
    ascan = {};
    setFileName("");
    setDate("");
    setAScanCursorMax(0);
    setAScanCurosr(0);
    replaySpeed = 0;
    removeAllSeries();
    setDistanceMode("N");
}

bool AScanInteractor::openFile(QString _fileName) {
    setReplayVisible(false);
    auto func = Union::AScan::AScanFileSelector::GetReadFunction(_fileName.toStdWString());
    if (!func) {
        QFileInfo info(_fileName);
        qWarning(TAG) << "can not find readFunc of :" << info.suffix();
    }
    auto _ascan = (func.value())(_fileName.toStdWString());
    if (!_ascan) {
        qDebug() << "Convert to standard type return nullopt";
        return false;
    }
    ascan = *_ascan;
    if (ascan.data.size() > 1) {
        setReplayVisible(true);
    } else if (ascan.data.size() == 1) {
        setReplayVisible(false);
    } else {
        qWarning(TAG) << "no data on file:" << _fileName;
        return false;
    }
    QFileInfo fileInfo(_fileName);
    setFileName(fileInfo.fileName());
    setDate(QString::fromStdString(_ascan->time));
    setAScanCursorMax(this->ascan.data.size() - 1);
    setAScanCurosr(0);
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
        for (auto axis : _series->attachedAxes()) {
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

void AScanInteractor::updateAScanSeries(const Union::AScan::AScanData& data) {
    updateAScanSeries(Union::AScan::AScanType::GetAScanSeriesData(data, softGain), {data.axisBias, 0}, {data.axisLen, 100.0});
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
            case QuadraticCurveSeriesType::DAC:
                return ascan.data[getAScanCurosr()].dac.has_value();
            case QuadraticCurveSeriesType::AVG:
                return ascan.data[getAScanCurosr()].avg.has_value();
            default:
                throw std::runtime_error("QuadraticCurveSeriesType error");
        }
    };
    if (!CHECK_VALID()) {
        return;
    }

    const auto& chData = ascan.data[getAScanCurosr()];
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
                return chData.dac->isSubline;
            case QuadraticCurveSeriesType::AVG:
                return chData.avg->isSubline;
            default:
                throw std::runtime_error("QuadraticCurveSeriesType error");
        }
    }();
    // 获取曲线表达式
    const auto lineExpr = [&]() {
        switch (type) {
            case QuadraticCurveSeriesType::DAC:
                return ascan.data[getAScanCurosr()].getDACLineExpr();
            case QuadraticCurveSeriesType::AVG:
                return ascan.data[getAScanCurosr()].getAVGLineExpr();
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
                _bGain = chData.dac->baseGain;
                break;
            case QuadraticCurveSeriesType::AVG:
                _bGain = chData.avg->baseGain;
                break;
            default:
                throw std::runtime_error("QuadraticCurveSeriesType error");
        }
        switch (index) {
            case 1:
                xlBias = chData.std.rlBias;
                break;
            case 2:
                xlBias = chData.std.slBias;
                break;
            case 3:
                xlBias = chData.std.elBias;
        }
        switch (type) {
            case QuadraticCurveSeriesType::DAC:
                return chData.baseGain + chData.scanGain - _bGain + xlBias + softGain;
            case QuadraticCurveSeriesType::AVG:
                return chData.baseGain + chData.scanGain + chData.surfaceCompentationGain - _bGain + xlBias + softGain + chData.avg->biasGain;
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

    // 查找曲线
    for (auto i = 0; std::cmp_less(i, lines.size()); i++) {
        lines[i] = (QLineSeries*)series(lineName(indexs[i]));
        if (!lines[i]) {
            lines[i] = createQuadraticCurveSeries(lineName(indexs[i]), {0.0, 0.0}, {static_cast<qreal>(chData.ascan.size()), 200.0});
        }
    }
    TEST_TIME_QUICK("update QuadraticCurve series");
    // 填充数据
    for (int i = 0; std::cmp_less(i, chData.ascan.size()); i++) {
        for (auto j = 0; std::cmp_less(j, pts.size()); j++) {
            pts[j].emplaceBack(QPointF(i, Union::CalculateGainOutput(lineExpr(i), modifyGain(indexs[j]))));
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
    auto        index = getAScanCurosr();
    const auto& chDat = ascan.data[index];
    for (auto i = 0; std::cmp_less(i, _gateValue.size()); i++) {
        auto info = chDat.GetGateResult(i);
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
        switch (chDat.distanceMode) {
            case Union::AScan::DistanceMode::DistanceMode_Y: {
                b = Union::ValueMap(loc, chDat.GetAxisRange());
                if (std::abs(ascan.angle) > 0.0001) {
                    a = b.value() / Union::Base::Probe::Degree2K(ascan.angle);
                    c = b.value() / std::sin(Union::Base::Probe::Degree2Rd(ascan.angle));
                }
                break;
            }
            case Union::AScan::DistanceMode::DistanceMode_X: {
                a = Union::ValueMap(loc, chDat.GetAxisRange());
                if (std::abs(ascan.angle) > 0.0001) {
                    b = Union::Base::Probe::Degree2K(ascan.angle) * b.value();
                    c = b.value() / std::cos(Union::Base::Probe::Degree2Rd(ascan.angle));
                }
                break;
            }
            case Union::AScan::DistanceMode::DistanceMode_S: {
                c = Union::ValueMap(loc, chDat.GetAxisRange());
                if (std::abs(ascan.angle) > 0.0001) {
                    a = c.value() * std::cos(Union::Base::Probe::Degree2Rd(ascan.angle));
                    b = c.value() * std::sin(Union::Base::Probe::Degree2Rd(ascan.angle));
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
        if (ascan.data[getAScanCurosr()].dac && b.has_value()) {
            auto        r_amp      = Union::CalculateGainOutput(_amp, chDat.surfaceCompentationGain);
            const auto& _ascan     = ascan.data[getAScanCurosr()];
            auto        lineExpr   = _ascan.getDACLineExpr();
            auto        slValue    = lineExpr((b.value() - _ascan.axisBias) / _ascan.axisLen * _ascan.ascan.size());
            auto        modifyGain = _ascan.baseGain + _ascan.scanGain + _ascan.surfaceCompentationGain - _ascan.dac->baseGain + _ascan.std.slBias;
            slValue                = Union::CalculateGainOutput(slValue, modifyGain);
            _equi                  = QString::asprintf("SL%+.1fdB", Union::CalculatedGain(slValue, r_amp));
        } else if (ascan.data[getAScanCurosr()].avg && b.has_value()) {
            auto        r_amp      = Union::CalculateGainOutput(_amp, chDat.surfaceCompentationGain);
            const auto& _ascan     = ascan.data[getAScanCurosr()];
            auto        lineExpr   = _ascan.getAVGLineExpr();
            auto        slValue    = lineExpr((b.value() - _ascan.axisBias) / _ascan.axisLen * _ascan.ascan.size());
            auto        modifyGain = _ascan.baseGain + _ascan.scanGain + _ascan.surfaceCompentationGain - _ascan.avg->baseGain + _ascan.avg->biasGain;
            slValue                = Union::CalculateGainOutput(slValue, modifyGain);
            _equi                  = QString::asprintf("Φ%+.1fdB", Union::CalculatedGain(slValue, r_amp));
        }
        _gateValue[i] = {
            {"amp", QString::number(amp / 2.0, 'f', 1)},
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
