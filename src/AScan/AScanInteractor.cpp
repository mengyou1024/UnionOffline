#include "AScanInteractor.hpp"
#include <ExcelRender.hpp>
#include <QLoggingCategory>
#include <QQmlProperty>
#include <QValueAxis>
#include <QtCore>

static Q_LOGGING_CATEGORY(TAG, "AScanIntr");

using namespace ::Union::Common;
using namespace ::Union::UniversalApparatus::AScan;
constexpr auto SCAN_LINE_WIDTH = 1.5;
constexpr auto GATE_LINE_WIDTH = 2.5;

bool AScanInteractor::getReplayVisible() const {
    return m_replayVisible;
}

void AScanInteractor::setReplayVisible(bool newReplayVisible) {
    if (m_replayVisible == newReplayVisible) {
        return;
    }
    m_replayVisible = newReplayVisible;
    emit replayVisibleChanged();
}

QString AScanInteractor::getDate() const {
    return m_date;
}

void AScanInteractor::setDate(const QString& newDate) {
    if (m_date == newDate) {
        return;
    }
    m_date = newDate;
    emit dateChanged();
}

int AScanInteractor::getSoftGain() const {
    return m_softGain;
}

void AScanInteractor::setSoftGain(int newSoftGain) {
    if (m_softGain == newSoftGain) {
        return;
    }
    m_softGain = newSoftGain;
    emit softGainChanged();
}

int AScanInteractor::getReplayValue() const {
    return m_replayValue;
}

void AScanInteractor::setReplayValue(int newReplayValue) {
    if (m_replayValue == newReplayValue) {
        return;
    }
    m_replayValue = newReplayValue;
    emit replayValueChanged();
}

QQuickItem* AScanInteractor::getChartView() const {
    return m_chartView;
}

void AScanInteractor::setChartView(QQuickItem* newChartView) {
    if (m_chartView == newChartView) {
        return;
    }
    m_chartView = newChartView;
    emit chartViewChanged();
}

bool AScanInteractor::reportExportClicked(QString _fileName, QQuickItemGrabResult* img) {
    if (!checkAScanCursorValid()) {
        return false;
    }

    auto vmp = aScanIntf()->createReportValueMap(getAScanCursor(), getSoftGain());

    auto       excel_template   = "excel_templates/AScan/T_报表生成.xlsx";
    auto       rail_weld_spec   = dynamic_cast<Special::RailWeldDigramSpecial*>(aScanIntf().get());
    auto       mdat_type        = dynamic_cast<Union::UniversalApparatus::AScan::Instance::UnType*>(aScanIntf().get());
    const auto is_rail_weld_390 = rail_weld_spec != nullptr && mdat_type == nullptr;
    const auto is_rail_weld_t8  = mdat_type != nullptr && !mdat_type->isCMP001IsNullptr(getAScanCursor());
    if (is_rail_weld_390 || is_rail_weld_t8) {
        // 钢轨特化版本(390、T8)
        excel_template = "excel_templates/AScan/T_报表生成_RailWeldSpecial.xlsx";
    } else if (dynamic_cast<Special::CameraImageSpecial*>(aScanIntf().get())) {
        // 390N/T8带摄像头
        excel_template = "excel_templates/AScan/T_报表生成_CameraImageSpecial.xlsx";
    }

    QMap<QString, QImage> image_map = {};

    if (img) {
        image_map.insert("AScan", img->image());
    }

    auto img_ascan_interface = dynamic_cast<Special::CameraImageSpecial*>(aScanIntf().get());
    if (img_ascan_interface) {
        auto cameraImage = img_ascan_interface->getCameraImage(getAScanCursor());
        if (!cameraImage.isNull()) {
            image_map.insert("Camera", cameraImage);
        }
    }

    auto result = Union::Common::File::RenderExcel::Render(excel_template, _fileName, vmp, image_map);
    qCInfo(TAG) << "保存报表:" << _fileName;
    return result;
}

bool AScanInteractor::performanceClicked(QString _fileName) {
    if (!checkAScanCursorValid()) {
        return false;
    }
    const auto& performance = aScanIntf()->getPerformance(getAScanCursor());
    QVariantMap vmp         = {
        {QObject::tr("仪器型号"), QString::fromStdString(aScanIntf()->getInstrumentName())},
        {QObject::tr("水平线性"), QString::number(performance.horizontalLinearity, 'f', 2)},
        {QObject::tr("垂直线性"), QString::number(performance.verticalLinearity, 'f', 2)},
        {QObject::tr("分辨力"), QString::number(performance.resolution, 'f', 1)},
        {QObject::tr("动态范围"), QString::number(performance.dynamicRange, 'f', 1)},
        {QObject::tr("灵敏度余量"), QString::number(performance.sensitivity, 'f', 1)},
        {QObject::tr("检测单位"), ""},
    };
    return Union::Common::File::RenderExcel::Render("excel_templates/AScan/T_仪器性能.xlsx", _fileName, vmp, {});
}

void AScanInteractor::gainValueModified([[maybe_unused]] qreal val) {}

void AScanInteractor::replayStartClicked(bool isStart) {
    m_isPlaying = isStart;
}

void AScanInteractor::replaySpeedClicked(int val) {
    m_replaySpeed = val;
}

void AScanInteractor::lastFrameClicked() {
    qCDebug(TAG) << "";
}

void AScanInteractor::nextFrameClicked() {
    qCDebug(TAG) << "";
}

void AScanInteractor::timeSliderMoved(qreal val) {
    setAScanCursor(static_cast<int>(val));
}

void AScanInteractor::seriesRemoved(QAbstractSeries* series) {
    qCDebug(TAG) << series;
    qCDebug(TAG) << series->attachedAxes();
    for (auto& axis : series->attachedAxes()) {
        series->detachAxis(axis);
        axis->deleteLater();
    }
}

QVariantMap AScanInteractor::getTechnologicalParameter() {
    if (!checkAScanCursorValid()) {
        return {};
    }

    return aScanIntf()->createTechnologicalParameter(getAScanCursor());
}

QVariantList AScanInteractor::getFileNameList() {
    if (aScanIntf() == nullptr) {
        return {};
    }
    auto         fileName_list = aScanIntf()->getFileNameList();
    QVariantList ret;
    for (const auto& fileName : fileName_list) {
        ret.push_back(QVariant(QString::fromStdWString(fileName)));
    }
    return ret;
}

void AScanInteractor::setFileNameIndex(int idx) {
    if (aScanIntf() != nullptr) {
        auto _last_idx = aScanIntf()->getFileNameIndex();
        if (_last_idx == idx) {
            return;
        }
        m_aScanCursor = 0;
        aScanIntf()->setFileNameIndex(idx);
        setAScanCursorMax(aScanIntf()->getDataSize() - 1);
        setDate(QString::fromStdString(aScanIntf()->getDate(getAScanCursor())));
        setReplayValue(0);
        changeDataCursor();
    }
}

bool AScanInteractor::railWeldSpecial_ZeroPointInFoot() {
    auto railweld_ptr = dynamic_cast<Special::RailWeldDigramSpecial*>(aScanIntf().get());
    if (railweld_ptr != nullptr) {
        return railweld_ptr->zeroPointInFoot();
    }
    return false;
}

void AScanInteractor::changeDataCursor() {
    if (std::cmp_less(getAScanCursor(), aScanIntf() ? aScanIntf()->getDataSize() : 0)) {
        // 1. 更新A扫曲线
        updateAScanSeries();
        // 2. 更新波门曲线
        updateGateSeries<2>(aScanIntf()->getGate(getAScanCursor()));
        if (dynamic_cast<::Union::UniversalApparatus::AScan::Instance::_330_DAC_C*>(aScanIntf().get()) != nullptr) {
            update330N_DAC_AVG_Series();
        } else {
            auto cmp000 = dynamic_cast<Special::CMP000Special*>(aScanIntf().get());
            if (cmp000 != nullptr && cmp000->isSpecial000Enabled(getAScanCursor())) {
                CMP000Special_UpdateDacSeries();
            } else {
                // 3. 更新DAC曲线
                updateQuadraticCurveSeries(QuadraticCurveSeriesType::DAC);
            }
            // 4. 更新AVG曲线
            updateQuadraticCurveSeries(QuadraticCurveSeriesType::AVG);
        }

        // 5. 更新AScan图的波门信息显示
        setGateValue(CreateGateValue());
        // 6. 更新显示的声程模式
        switch (aScanIntf()->getDistanceMode(getAScanCursor())) {
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
        // 7. 更新摄像头控件
        auto camera_special = dynamic_cast<Special::CameraImageSpecial*>(aScanIntf().get());
        if (camera_special != nullptr) {
            setHasCameraImage(camera_special->showCameraImage(getAScanCursor()));
        }
        // 8. 更新B扫或C扫的坐标范围
        updateBOrCScanViewRange();
    }
}

void AScanInteractor::updateCurrentFrame() {
    // 增益发生变化
    if (aScanIntf() != nullptr) {
        updateAScanSeries(aScanIntf()->getAScanSeriesData(getAScanCursor(), m_softGain), {aScanIntf()->getAxisBias(getAScanCursor()), 0}, {aScanIntf()->getAxisLen(getAScanCursor()), 100.0});
        if (dynamic_cast<::Union::UniversalApparatus::AScan::Instance::_330_DAC_C*>(aScanIntf().get()) != nullptr) {
            update330N_DAC_AVG_Series();
        } else {
            auto cmp000 = dynamic_cast<Special::CMP000Special*>(aScanIntf().get());
            if (cmp000 != nullptr && cmp000->isSpecial000Enabled(getAScanCursor())) {
                CMP000Special_UpdateDacSeries();
            } else {
                updateQuadraticCurveSeries(QuadraticCurveSeriesType::DAC);
            }
            updateQuadraticCurveSeries(QuadraticCurveSeriesType::AVG);
        }
        setGateValue(CreateGateValue());
        updateBOrCScanView();
        updateBOrCScanViewRange();
    }
}

void AScanInteractor::updateBOrCScanView() {
    if (showCScanView()) {
        auto c_scan_sp = std::make_shared<Union::View::CScanView>();
        m_scanViewSp   = c_scan_sp;
        setScanViewHandler(m_scanViewSp.get());
        // std::vector<uint8_t> cscan_image;
        // const auto           width  = aScanIntf()->getDataSize();
        // const auto           height = std::ssize(aScanIntf()->getScanData(0));
        // cscan_image.resize(width * height);
        // std::memset(cscan_image.data(), 0, std::ssize(cscan_image));
        // for (auto x = 0; x < width; x++) {
        //     for (auto y = 0; y < height; y++) {
        //         try {
        //             cscan_image.at(y * width + x) = aScanIntf()->getScanData(x).at(y);
        //         } catch (...) {}
        //     }
        // }
        // c_scan_sp->replace(cscan_image, width, height);

    } else if (showBScanView()) {
        auto b_scan_sp = std::make_shared<Union::View::BScanView>();
        m_scanViewSp   = b_scan_sp;
        setScanViewHandler(m_scanViewSp.get());
        std::vector<uint8_t> cscan_image;
        const auto           height = aScanIntf()->getDataSize();
        int                  width  = 0;
        for (auto i = 0; i < aScanIntf()->getDataSize(); i++) {
            if (std::ssize(aScanIntf()->getScanData(i)) > width) {
                width = std::ssize(aScanIntf()->getScanData(i));
            }
        }
        cscan_image.resize(width * height);
        std::memset(cscan_image.data(), 255, std::ssize(cscan_image));
        for (int frame = 0; frame < height; frame++) {
            const auto& data     = aScanIntf()->getScanData(frame);
            const auto  dist_ptr = &cscan_image[width * frame];
            const auto  dist_sz  = width;
            memcpy(dist_ptr, data.data(), std::min<int>(dist_sz, std::ssize(data)));
        }
        b_scan_sp->replace(cscan_image, width, height);
    } else {
        setScanViewHandler(nullptr);
    }
}

void AScanInteractor::updateBOrCScanViewRange() {
    // 更新B扫
    if (showBScanView()) {
        auto b_scan_view = std::dynamic_pointer_cast<Union::View::BScanView>(m_scanViewSp);
        if (b_scan_view) {
            auto range_start = aScanIntf()->getAxisBias(getAScanCursor());
            auto range_end   = aScanIntf()->getAxisLen(getAScanCursor()) + range_start;
            b_scan_view->setHorizontalAxisRange(QPointF(range_start, range_end));
            // TODO: 设置横坐标范围
        }
    }
    // TODO: 更新C扫
}

const QJsonArray AScanInteractor::getGateValue() const {
    return m_gateValue;
}

void AScanInteractor::setGateValue(const QJsonArray& newGateValue) {
    if (m_gateValue == newGateValue) {
        return;
    }
    m_gateValue = newGateValue;
    emit gateValueChanged();
}

int AScanInteractor::getAScanCursor() const {
    return m_aScanCursor;
}

void AScanInteractor::setAScanCursor(int newAScanCursor) {
    if (m_aScanCursor == newAScanCursor) {
        return;
    }
    m_aScanCursor = newAScanCursor;
    emit aScanCursorChanged();
}

int AScanInteractor::getAScanCursorMax() const {
    return m_aScanCursorMax;
}

void AScanInteractor::setAScanCursorMax(int newAScanCursorMax) {
    if (m_aScanCursorMax == newAScanCursorMax) {
        return;
    }
    m_aScanCursorMax = newAScanCursorMax;
    emit aScanCursorMaxChanged();
}

QString AScanInteractor::getDistanceMode() const {
    return m_distanceMode;
}

void AScanInteractor::setDistanceMode(const QString& newDistanceMode) {
    if (m_distanceMode == newDistanceMode)
        return;
    m_distanceMode = newDistanceMode;
    emit distanceModeChanged();
}

QImage AScanInteractor::getCameraImage() const {
    auto img_ascan_interface = dynamic_cast<Special::CameraImageSpecial*>(aScanIntf().get());
    if (img_ascan_interface == nullptr) {
        return QImage();
    }
    return img_ascan_interface->getCameraImage(getAScanCursor());
}

QVariantList AScanInteractor::getRailWeldDot() const {
    auto railweld_ascan_interface = dynamic_cast<Special::RailWeldDigramSpecial*>(aScanIntf().get());
    if (railweld_ascan_interface) {
        return QVariantList({railweld_ascan_interface->getDotX(), railweld_ascan_interface->getDotY(), railweld_ascan_interface->getDotZ()});
    }
    return {};
}

bool AScanInteractor::getHasCameraImage() const {
    return m_hasCameraImage;
}

void AScanInteractor::setHasCameraImage(bool newHasCameraImage) {
    if (m_hasCameraImage == newHasCameraImage)
        return;
    m_hasCameraImage = newHasCameraImage;
    emit hasCameraImageChanged();
}

bool AScanInteractor::getShowRailWeldDigramSpecial() const {
    return m_showRailWeldDigramSpecial;
}

void AScanInteractor::setShowRailWeldDigramSpecial(bool newShowRailWeldDigramSpecial) {
    if (m_showRailWeldDigramSpecial == newShowRailWeldDigramSpecial)
        return;
    m_showRailWeldDigramSpecial = newShowRailWeldDigramSpecial;
    emit showRailWeldDigramSpecialChanged();
}

int AScanInteractor::getReplayTimerInterval() const {
    return m_replayTimerInterval;
}

void AScanInteractor::setReplayTimerInterval(int newReplayTimerInterval) {
    if (m_replayTimerInterval == newReplayTimerInterval)
        return;
    m_replayTimerInterval = newReplayTimerInterval;
    emit replayTimerIntervalChanged();
}

bool AScanInteractor::getReportEnabled() const {
    return m_reportEnabled;
}

void AScanInteractor::setReportEnabled(bool newReportEnabled) {
    if (m_reportEnabled == newReportEnabled)
        return;
    m_reportEnabled = newReportEnabled;
    emit reportEnabledChanged();
}

bool AScanInteractor::isGateEnable(int gate_idx) const {
    if (aScanIntf() == nullptr) {
        return false;
    }
    auto gate = aScanIntf()->getGate(getAScanCursor());
    return gate.at(gate_idx % 2).enable;
}

bool AScanInteractor::getDateEnabled() const {
    return m_dateEnabled;
}

void AScanInteractor::setDateEnabled(bool newDateEnabled) {
    if (m_dateEnabled == newDateEnabled)
        return;
    m_dateEnabled = newDateEnabled;
    emit dateEnabledChanged();
}

bool AScanInteractor::getShowCMP001Special() const {
    return m_showCMP001Special;
}

void AScanInteractor::setShowCMP001Special(bool newShowCMP001Special) {
    if (m_showCMP001Special == newShowCMP001Special)
        return;
    m_showCMP001Special = newShowCMP001Special;
    emit showCMP001SpecialChanged();
}

AScanInteractor::ASCAN_TYPE AScanInteractor::aScanIntf() const {
    return m_aScanIntf;
}

void AScanInteractor::setAScanIntf(const ASCAN_TYPE& newAScanIntf) {
    if (m_aScanIntf == newAScanIntf)
        return;
    m_aScanIntf = newAScanIntf;
    emit aScanIntfChanged();
}

bool AScanInteractor::showBScanView() const {
    return m_showBScanView;
}

void AScanInteractor::setShowBScanView(bool newShowBScanView) {
    if (m_showBScanView == newShowBScanView)
        return;
    m_showBScanView = newShowBScanView;
    emit showBScanViewChanged();
}

bool AScanInteractor::showCScanView() const {
    return m_showCScanView;
}

void AScanInteractor::setShowCScanView(bool newShowCScanView) {
    if (m_showCScanView == newShowCScanView)
        return;
    m_showCScanView = newShowCScanView;
    emit showCScanViewChanged();
}

QQuickItem* AScanInteractor::scanViewHandler() const {
    return m_scanViewHandler;
}

void AScanInteractor::setScanViewHandler(QQuickItem* newScanViewHandler) {
    if (m_scanViewHandler == newScanViewHandler)
        return;
    m_scanViewHandler = newScanViewHandler;
    emit scanViewHandlerChanged();
}

bool AScanInteractor::checkAScanCursorValid() {
    if (!std::cmp_greater(aScanIntf() ? aScanIntf()->getDataSize() : 0, getAScanCursorMax()) || !(getAScanCursorMax() >= 0)) {
        return false;
    }
    return true;
}

void AScanInteractor::update330N_DAC_AVG_Series() {
    const auto LineName = [&](int index) -> QString {
        return QString(DAC_SERIES_NAME).arg(getDACSeriesSubName(index));
    };

    std::vector<QLineSeries*>   lines  = {};
    std::vector<QList<QPointF>> pts    = {};
    std::vector<int>            indexs = {};

    auto _convert = dynamic_cast<::Union::UniversalApparatus::AScan::Instance::_330_DAC_C*>(aScanIntf().get());
    if (_convert != nullptr) {
        lines.resize(3);
        pts.resize(3);
        indexs = {1, 2, 3};
        // 隐藏未使用的曲线
        for (auto i = 0; std::cmp_less(i, DAC_AVG_SUB_NAME_MAX); i++) {
            auto line = (QLineSeries*)series(LineName(i));
            if (line) {
                auto find_result = std::find(indexs.begin(), indexs.end(), i);
                if (find_result == indexs.end()) {
                    line->setVisible(false);
                    line->deleteLater();
                }
            }
        }
        // 查找曲线
        for (auto i = 0; std::cmp_less(i, lines.size()); i++) {
            lines[i] = (QLineSeries*)series(LineName(indexs[i]));
            if (!lines[i]) {
                lines[i] = createQuadraticCurveSeries(LineName(indexs[i]));
            }
            // 重新设置DAC曲线的坐标轴范围
            lines[i]->attachedAxes().at(0)->setMin(aScanIntf()->getAxisBias(getAScanCursor()));
            lines[i]->attachedAxes().at(0)->setMax(aScanIntf()->getAxisLen(getAScanCursor()));
            lines[i]->attachedAxes().at(1)->setMin(0.0);
            lines[i]->attachedAxes().at(1)->setMax(100.0);
            lines[i]->setVisible();
        }

        for (auto i = 0; std::cmp_less(i, pts.size()); i++) {
            lines[i]->clear();
            if (_convert) {
                auto lineVector    = _convert->unResolvedGetDacLines(getAScanCursor())[i];
                auto transformFunc = [this](QPointF pt) {
                    return QPointF(pt.x(), CalculateGainOutput(pt.y(), static_cast<double>(getSoftGain())));
                };
                std::transform(lineVector.begin(), lineVector.end(), lineVector.begin(), transformFunc);
                lines[i]->replace(lineVector);
            }
        }
    }
}

void AScanInteractor::CMP000Special_UpdateDacSeries() {
    // begin define lambda ->{
    auto cmp000_data = dynamic_cast<Special::CMP000Special*>(aScanIntf().get());

    auto CheckValid = [&]() {
        // 检查数据指针是否有效
        if (!checkAScanCursorValid()) {
            return false;
        }

        if (cmp000_data == nullptr) {
            return false;
        }

        // 检查曲线是否有值
        auto has_value = aScanIntf()->getDAC(getAScanCursor()).has_value();
        if (!has_value) {
            for (auto i = 0; i < DAC_AVG_SUB_NAME_MAX; i++) {
                auto temp = (QLineSeries*)series(QString(DAC_SERIES_NAME).arg(getDACSeriesSubName(i)));
                if (temp) {
                    temp->setVisible(false);
                }
            }
        }
        return has_value;
    };

    if (!CheckValid()) {
        return;
    }

    // 获取曲线名称
    const auto LineName = [&](int index) -> QString {
        return QString(DAC_SERIES_NAME).arg(getDACSeriesSubName(index));
    };

    // 获取曲线表达式
    const auto LineExpr = [&]() {
        return aScanIntf()->getDACLineExpr(getAScanCursor());
    }();
    // 获取增益修改
    const auto ModifyGain = [&](int idx) {
        return cmp000_data->getDACLineBias(getAScanCursor(), idx) + getSoftGain();
    };
    // } <- end define lambda

    // ↓↓↓函数逻辑开始↓↓↓
    std::vector<QLineSeries*>   lines  = {};
    std::vector<QList<QPointF>> pts    = {};
    std::vector<int>            indexs = {};

    auto dac_line_number = cmp000_data->getDacLineNumber(getAScanCursor());
    lines.resize(dac_line_number);
    pts.resize(dac_line_number);
    indexs.resize(dac_line_number);
    for (auto i = 0; auto& it : indexs) {
        it = ++i;
    }

    // 隐藏未使用的曲线
    for (auto i = 0; std::cmp_less(i, DAC_AVG_SUB_NAME_MAX); i++) {
        auto line = (QLineSeries*)series(LineName(i));
        if (line) {
            auto find_result = std::find(indexs.begin(), indexs.end(), i);
            if (find_result == indexs.end()) {
                line->setVisible(false);
            }
        }
    }
    // 查找曲线
    for (auto i = 0; std::cmp_less(i, lines.size()); i++) {
        lines[i] = (QLineSeries*)series(LineName(indexs[i]));
        if (!lines[i]) {
            lines[i] = createQuadraticCurveSeries(LineName(indexs[i]));
        }
        // 重新设置DAC曲线的坐标轴范围
        lines[i]->attachedAxes().at(0)->setMin(0.0);
        lines[i]->attachedAxes().at(0)->setMax(static_cast<double>(aScanIntf()->getScanData(getAScanCursor()).size()));
        lines[i]->attachedAxes().at(1)->setMin(0.0);
        lines[i]->attachedAxes().at(1)->setMax(200.0);
        lines[i]->setVisible();
    }

    // 填充数据
    for (int i = 0; std::cmp_less(i, aScanIntf()->getScanData(getAScanCursor()).size()); i++) {
        for (auto j = 0; std::cmp_less(j, pts.size()); j++) {
            auto val = LineExpr(i);
            if (val.has_value()) {
                pts[j].push_back(QPointF(i, CalculateGainOutput(val.value(), ModifyGain(indexs[j]))));
            }
        }
    }
    // 替换曲线数据
    for (auto i = 0; std::cmp_less(i, pts.size()); i++) {
        lines[i]->replace(pts[i]);
    }
}

AScanInteractor::AScanInteractor() {
    QJsonObject gate1;
    gate1.insert("amp", "-");
    gate1.insert("→", "-");
    gate1.insert("↓", "-");
    gate1.insert("↘", "-");
    gate1.insert("equi", "-");
    QJsonObject gate2;
    gate2.insert("amp", "-");
    gate2.insert("→", "-");
    gate2.insert("↓", "-");
    gate2.insert("↘", "-");
    gate2.insert("equi", "-");

    QJsonArray arr;
    arr.append(gate1);
    arr.append(gate2);
    m_gateValue = arr;

    connect(this, &AScanInteractor::aScanCursorChanged, this, &AScanInteractor::changeDataCursor);
    connect(this, &AScanInteractor::softGainChanged, this, &AScanInteractor::updateCurrentFrame);
}

AScanInteractor::~AScanInteractor() {
    disconnect(this, &AScanInteractor::aScanCursorChanged, this, &AScanInteractor::changeDataCursor);
    disconnect(this, &AScanInteractor::softGainChanged, this, &AScanInteractor::updateCurrentFrame);
}

void AScanInteractor::setDefaultValue() {
    setAScanIntf(nullptr);
    setDate("");
    setAScanCursorMax(0);
    setAScanCursor(0);
    m_replaySpeed = 0;
    removeAllSeries();
    setDistanceMode("N");
    setShowBScanView(false);
    setShowCScanView(false);
    setReplayVisible(false);
    setHasCameraImage(false);
    setShowRailWeldDigramSpecial(false);
    setShowCMP001Special(false);
    auto READ_FUNC = AScanFileSelector::Instance()->GetReadFunction(_fileName.toStdWString());
    if (!READ_FUNC.has_value()) {
        QFileInfo info(_fileName);
        qCWarning(TAG) << "can't find read interface, file suffix" << info.suffix();
        return false;
    }
    try {
        ASCAN_TYPE _tyy_ascan_type = nullptr;
        for (const auto& func : READ_FUNC.value()) {
            _tyy_ascan_type = func(_fileName.toStdWString());
            if (_tyy_ascan_type != nullptr) {
                break;
            }
        }
        setAScanIntf(_tyy_ascan_type);
        emit aScanIntfChanged();
    } catch (std::exception& e) {
        qCCritical(TAG) << e.what();
        setAScanIntf(nullptr);
        emit aScanIntfChanged();
    }

    if (!aScanIntf()) {
        qCCritical(TAG) << "read file error, fileName:" << _fileName;
        return false;
    }
    if (aScanIntf()->getDataSize() > 1) {
        setReplayVisible(true);
        setReplayTimerInterval(aScanIntf()->getReplayTimerInterval());
    } else if (aScanIntf()->getDataSize() == 1) {
        setReplayVisible(false);
    } else {
        qCWarning(TAG) << "no data on file:" << _fileName;
        return false;
    }
    setReportEnabled(aScanIntf()->getReportEnable());
    setDateEnabled(aScanIntf()->getDateEnable());
    setDate(QString::fromStdString(aScanIntf()->getDate(getAScanCursor())));
    setAScanCursorMax(aScanIntf()->getDataSize() - 1);
    auto camera_special = dynamic_cast<Special::CameraImageSpecial*>(aScanIntf().get());
    if (camera_special != nullptr) {
        setHasCameraImage(camera_special->showCameraImage(getAScanCursor()));
    }
    if (getAScanCursor() == 0) {
        changeDataCursor();
    }
    setAScanCursor(0);
    // Special: CMP001
    auto cmp001 = dynamic_cast<Special::CMP001Special*>(aScanIntf().get());
    if (cmp001 && cmp001->isSpecial001Enabled(0)) {
        setShowCMP001Special(true);
    }
    // Special: BScanSpecial
    auto b_scan_sepcial = dynamic_cast<Union::AScan::Special::BScanSpecial*>(aScanIntf().get());
    if (b_scan_sepcial && b_scan_sepcial->isSpecialBScanEnabled() && aScanIntf()->getDataSize() > 1) {
        setShowBScanView(true);
        setReplayVisible(false);
    }
    // Special: CScanSpecial
    auto c_scan_special = dynamic_cast<Union::AScan::Special::CScanSpecial*>(aScanIntf().get());
    if (c_scan_special && c_scan_special->isSpecialCScanEnabled() && aScanIntf()->getDataSize() > 1) {
        if (showBScanView()) {
            qCWarning(TAG) << QObject::tr("同时使用B扫和C扫, B扫已被禁用");
            setShowBScanView(false);
        }
        setShowCScanView(true);
        setReplayVisible(false);
    }
    updateBOrCScanView();
    updateBOrCScanViewRange();

    // 最后更新A扫数据指针
    if (getAScanCursor() == 0) {
        changeDataCursor();
    }
    setAScanCursor(0);
    return true;
}

QAbstractSeries* AScanInteractor::createSeries(QAbstractSeries::SeriesType type, QString name, QAbstractAxis* axisX, QAbstractAxis* axisY) {
    QAbstractSeries* ret = nullptr;

    bool res = QMetaObject::invokeMethod(m_chartView, "createSeries", Q_RETURN_ARG(QAbstractSeries*, ret),
                                         Q_ARG(int, type),
                                         Q_ARG(QString, name),
                                         Q_ARG(QAbstractAxis*, axisX), Q_ARG(QAbstractAxis*, axisY));
    if (!res) {
        qCCritical(TAG) << "invok method `createSeries` error with parameter:" << type << name << axisX << axisY;
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
    QMetaObject::invokeMethod(m_chartView, "removeAllSeries");
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
    updateAScanSeries(aScanIntf()->getAScanSeriesData(getAScanCursor(), m_softGain), {aScanIntf()->getAxisBias(getAScanCursor()), 0}, {aScanIntf()->getAxisLen(getAScanCursor()), 100.0});
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
    // begin define lambda ->{
    auto CheckValid = [&]() {
        // 检查数据指针是否有效
        if (!checkAScanCursorValid()) {
            return false;
        }

        // 检查曲线是否有值
        switch (type) {
            case QuadraticCurveSeriesType::DAC: {
                auto has_value = aScanIntf()->getDAC(getAScanCursor()).has_value();
                if (!has_value) {
                    for (auto i = 0; i < DAC_AVG_SUB_NAME_MAX; i++) {
                        auto temp = (QLineSeries*)series(QString(DAC_SERIES_NAME).arg(getDACSeriesSubName(i)));
                        if (temp) {
                            temp->setVisible(false);
                        }
                    }
                }
                return has_value;
            }
            case QuadraticCurveSeriesType::AVG: {
                auto has_value = aScanIntf()->getAVG(getAScanCursor()).has_value();
                if (!has_value) {
                    for (auto i = 0; i < DAC_AVG_SUB_NAME_MAX; i++) {
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
    if (!CheckValid()) {
        return;
    }

    // 获取曲线名称
    const auto LineName = [&](int index) -> QString {
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
    const auto IsSubline = [&]() {
        switch (type) {
            case QuadraticCurveSeriesType::DAC:
                return aScanIntf()->getDAC(getAScanCursor())->isSubline;
            case QuadraticCurveSeriesType::AVG:
                return aScanIntf()->getAVG(getAScanCursor())->isSubline;
            default:
                throw std::runtime_error("QuadraticCurveSeriesType error");
        }
    };
    // 获取曲线表达式
    const auto LineExpr = [&]() {
        switch (type) {
            case QuadraticCurveSeriesType::DAC:
                return aScanIntf()->getDACLineExpr(getAScanCursor());
            case QuadraticCurveSeriesType::AVG:
                return aScanIntf()->getAVGLineExpr(getAScanCursor());
            default:
                throw std::runtime_error("QuadraticCurveSeriesType error");
        }
    }();
    // 获取增益修改
    const auto ModifyGain = [&](int idx) {
        double ret = 0.0;
        switch (idx) {
            case 1:
                ret = aScanIntf()->getDACStandard(getAScanCursor()).rlBias;
                break;
            case 2:
                ret = aScanIntf()->getDACStandard(getAScanCursor()).slBias;
                break;
            case 3:
                ret = aScanIntf()->getDACStandard(getAScanCursor()).elBias;
        }
        return ret + getSoftGain();
    };
    // } <- end define lambda

    // ↓↓↓函数逻辑开始↓↓↓
    std::vector<QLineSeries*>   lines  = {};
    std::vector<QList<QPointF>> pts    = {};
    std::vector<int>            indexs = {};

    if (!IsSubline()) {
        lines.resize(1);
        pts.resize(1);
        indexs = {0};
    } else {
        lines.resize(3);
        pts.resize(3);
        indexs = {1, 2, 3};
    }

    // 隐藏未使用的曲线
    for (auto i = 0; std::cmp_less(i, DAC_AVG_SUB_NAME_MAX); i++) {
        auto line = (QLineSeries*)series(LineName(i));
        if (line) {
            auto find_result = std::find(indexs.begin(), indexs.end(), i);
            if (find_result == indexs.end()) {
                line->setVisible(false);
            }
        }
    }
    // 查找曲线
    for (auto i = 0; std::cmp_less(i, lines.size()); i++) {
        lines[i] = (QLineSeries*)series(LineName(indexs[i]));
        if (!lines[i]) {
            lines[i] = createQuadraticCurveSeries(LineName(indexs[i]));
        }
        // 重新设置DAC曲线的坐标轴范围
        lines[i]->attachedAxes().at(0)->setMin(0.0);
        lines[i]->attachedAxes().at(0)->setMax(static_cast<double>(aScanIntf()->getScanData(getAScanCursor()).size()));
        lines[i]->attachedAxes().at(1)->setMin(0.0);
        lines[i]->attachedAxes().at(1)->setMax(200.0);
        lines[i]->setVisible();
    }

    // 填充数据
    for (int i = 0; std::cmp_less(i, aScanIntf()->getScanData(getAScanCursor()).size()); i++) {
        for (auto j = 0; std::cmp_less(j, pts.size()); j++) {
            auto val = LineExpr(i);
            if (val.has_value()) {
                pts[j].push_back(QPointF(i, CalculateGainOutput(val.value(), ModifyGain(indexs[j]))));
            }
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

void AScanInteractor::updateGateSeries(::Union::BasicType::Gate gate, int index) {
    QString gateName = QString(GATE_SERIES_NAME).arg(index);
    auto    line     = (QLineSeries*)series(gateName);

    if (!line) {
        line = createGateSeries(index);
    }
    QList<QPointF> gateList = {};
    if (gate.enable) {
        constexpr auto midify_bias = 0.005;
        auto           cmp000      = dynamic_cast<Union::UniversalApparatus::AScan::Special::CMP000Special*>(aScanIntf().get());
        if (index == 1 && cmp000 != nullptr && cmp000->isSpecial000Enabled(getAScanCursor()) && cmp000->gateBIsLostType(getAScanCursor())) {
            gateList.append({gate.pos, gate.height - midify_bias});
            gateList.append({gate.pos + midify_bias, gate.height});
            gateList.append({gate.pos + gate.width - midify_bias, gate.height});
            gateList.append({gate.pos + gate.width, gate.height - midify_bias});
            line->replace(gateList);
            return;
        }
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
    using namespace std::chrono_literals;
    auto _cur_t = std::chrono::system_clock::now();
    if (m_isPlaying && (_cur_t - m_lastUpdateGateValueTime < 200ms)) {
        return getGateValue();
    }

    m_lastUpdateGateValueTime = _cur_t;
    if (!checkAScanCursorValid()) {
        std::array<QVariantMap, 2> _m_gateValue = {};
        for (auto i = 0; std::cmp_less(i, _m_gateValue.size()); i++) {
            _m_gateValue[i] = {
                {"amp",  "-"},
                {"→",    "-"},
                {"↓",    "-"},
                {"↘",    "-"},
                {"equi", "-"},
            };
        }
        return QJsonArray::fromVariantList({_m_gateValue[0], _m_gateValue[1]});
    }

    return aScanIntf()->createGateValue(getAScanCursor(), getSoftGain());
}

template <int N>
void AScanInteractor::updateGateSeries(std::array<::Union::BasicType::Gate, N> gate) {
    for (auto i = 0; std::cmp_less(i, gate.size()); i++) {
        updateGateSeries(gate[i], i);
    }
}

QAbstractSeries* AScanInteractor::series(const QString& name) {
    QAbstractSeries* ret = nullptr;
    QMetaObject::invokeMethod(m_chartView, "series", Q_RETURN_ARG(QAbstractSeries*, ret), Q_ARG(QString, name));
    return ret;
}

QAbstractSeries* AScanInteractor::series(int index) {
    QAbstractSeries* ret = nullptr;
    QMetaObject::invokeMethod(m_chartView, "series", Q_RETURN_ARG(QAbstractSeries*, ret), Q_ARG(int, index));
    return ret;
}

int AScanInteractor::seriesCount() const {
    return m_chartView->property("count").toInt();
}
