#include "AScanInteractor.hpp"
#include <AppSetting.hpp>
#include <ExcelRender.hpp>
#include <QLoggingCategory>
#include <QQmlProperty>
#include <QValueAxis>
#include <QtCore>
#include <ranges>
#include <stacktrace>

static Q_LOGGING_CATEGORY(TAG, "AScanIntr");

using namespace ::Union::Common;
using namespace ::Union::UniversalApparatus::AScan;
constexpr auto SCAN_LINE_WIDTH = 1.5;
constexpr auto GATE_LINE_WIDTH = 2.5;

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
    connect(this, &AScanInteractor::workpieceThicknessSpecialValueChanged,
            this, &AScanInteractor::onWorkpieceThicknessSpecialValueChanged);
    connect(AppSetting::Instance(), &AppSetting::calculateGateResultChanged, this, [this] {
        auto mdat_spec = std::dynamic_pointer_cast<Instance::UnType>(aScanIntf());
        if (mdat_spec == nullptr) {
            return;
        }

        if (AppSetting::Instance()->calculateGateResult()) {
            mdat_spec->setUseGateResultCalculate(true);
        } else {
            mdat_spec->setUseGateResultCalculate(false);
        }
        // 重新设置门内数据
        setGateValue(CreateGateValue());
    });
    connect(this, &AScanInteractor::bScanIsGateModeChanged,
            this, [this] {
                // 更新B扫或C扫
                auto mdat_spec = std::dynamic_pointer_cast<Instance::UnType>(aScanIntf());
                if (mdat_spec == nullptr) {
                    return;
                }
                mdat_spec->setUseBScanGateMode(bScanIsGateMode());
                updateBOrCScanView(false);
                updateBOrCScanViewRange();
                updateExtraBScanView(false);
                updateExtraBScanViewRange();
            });
}

AScanInteractor::~AScanInteractor() {
    setScanViewHandler(nullptr);
    setScanViewHandlerExtra(nullptr);

    disconnect(this, &AScanInteractor::aScanCursorChanged, this, &AScanInteractor::changeDataCursor);
    disconnect(this, &AScanInteractor::softGainChanged, this, &AScanInteractor::updateCurrentFrame);
    disconnect(this, &AScanInteractor::workpieceThicknessSpecialValueChanged, this, &AScanInteractor::onWorkpieceThicknessSpecialValueChanged);
    disconnect(AppSetting::Instance(), &AppSetting::calculateGateResultChanged, this, nullptr);
    disconnect(this, &AScanInteractor::bScanIsGateModeChanged, this, nullptr);
}

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
    try {
        if (!checkAScanCursorValid()) {
            return false;
        }

        auto vmp = aScanIntf()->createReportValueMap(getAScanCursor(), getSoftGain());

        auto       excel_template   = "excel_templates/AScan/T_报表生成.xlsx";
        auto       rail_weld_spec   = std::dynamic_pointer_cast<Special::RailWeldDigramSpecial>(aScanIntf());
        auto       mdat_type        = std::dynamic_pointer_cast<Instance::UnType>(aScanIntf());
        const auto is_rail_weld_390 = rail_weld_spec != nullptr && mdat_type == nullptr;
        const auto is_rail_weld_t8  = mdat_type != nullptr && !mdat_type->isCMP001IsNullptr(getAScanCursor());
        if (is_rail_weld_390 || is_rail_weld_t8) {
            // 钢轨特化版本(390、T8)
            excel_template = "excel_templates/AScan/T_报表生成(钢轨焊缝).xlsx";
        }

        QMap<QString, QImage> image_map = {};

        if (img) {
            image_map.insert("AScan", img->image());
        }

        auto img_ascan_interface = std::dynamic_pointer_cast<Special::CameraImageSpecial>(aScanIntf());
        if (img_ascan_interface) {
            auto cameraImage = img_ascan_interface->getCameraImage(getAScanCursor());
            if (!cameraImage.isNull()) {
                image_map.insert("Camera", cameraImage);
            }
        }

        if (m_scanViewHandler) {
            auto scan_view_image = m_scanViewHandler->grabImage();
            if (scan_view_image.has_value()) {
                image_map.insert("ScanView", scan_view_image.value());
            }
        }

        if (m_scanViewHandlerExtra != nullptr) {
            auto scan_view_image = m_scanViewHandlerExtra->grabImage();
            if (scan_view_image.has_value()) {
                image_map.insert("ScanViewExtra", scan_view_image.value());
            }
        }

        auto result = Union::Common::File::RenderExcel::Render(excel_template, _fileName, vmp, image_map);
        qCInfo(TAG) << "保存报表:" << _fileName;
        return result;
    } catch (std::exception& e) {
        qCCritical(TAG) << e.what();
        auto stack_trace = std::stacktrace::current();
        qCCritical(TAG) << std::to_string(stack_trace).c_str();
        return false;
    }
}

bool AScanInteractor::performanceClicked(QString _fileName) {
    try {
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
    } catch (std::exception& e) {
        qCCritical(TAG) << e.what();
        auto stack_trace = std::stacktrace::current();
        qCCritical(TAG) << std::to_string(stack_trace).c_str();
        return false;
    }
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

void AScanInteractor::seriesRemoved(QtCharts::QAbstractSeries* series) {
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

    try {
        return aScanIntf()->createTechnologicalParameter(getAScanCursor());
    } catch (std::exception& e) {
        qCCritical(TAG) << e.what();
        auto stack_trace = std::stacktrace::current();
        qCCritical(TAG) << std::to_string(stack_trace).c_str();
        return {};
    }
}

QVariantList AScanInteractor::getFileNameList() {
    try {
        if (aScanIntf() == nullptr) {
            return {};
        }
        auto         fileName_list = aScanIntf()->getFileNameList();
        QVariantList ret;
        for (const auto& fileName : fileName_list) {
            ret.push_back(QVariant(QString::fromStdWString(fileName)));
        }
        return ret;
    } catch (std::exception& e) {
        qCCritical(TAG) << e.what();
        auto stack_trace = std::stacktrace::current();
        qCCritical(TAG) << std::to_string(stack_trace).c_str();
        return {};
    }
}

void AScanInteractor::setFileNameIndex(int idx) {
    if (aScanIntf() == nullptr) {
        qCCritical(TAG) << "ascan interface is nullptr";
        auto stack_trace = std::stacktrace::current();
        qCCritical(TAG) << std::to_string(stack_trace).c_str();
        return;
    }

    try {
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
    } catch (std::exception& e) {
        qCCritical(TAG) << e.what();
        auto stack_trace = std::stacktrace::current();
        qCCritical(TAG) << std::to_string(stack_trace).c_str();
    }
}

bool AScanInteractor::railWeldSpecial_ZeroPointInFoot() {
    auto railweld_ptr = std::dynamic_pointer_cast<Special::RailWeldDigramSpecial>(aScanIntf());
    if (railweld_ptr != nullptr) {
        return railweld_ptr->zeroPointInFoot();
    }
    return false;
}

void AScanInteractor::onWorkpieceThicknessSpecialValueChanged() {
    auto set_workpiece_thicknes_spec = std::dynamic_pointer_cast<Special::SetWorkpieceThicknessSpecial>(aScanIntf());
    if (set_workpiece_thicknes_spec == nullptr) {
        return;
    }
    bool  is_ok           = false;
    qreal thickness_value = m_workpieceThicknessSpecialValue.toReal(&is_ok);
    if (is_ok) {
        set_workpiece_thicknes_spec->setWorkpieceThickness(thickness_value);

    } else {
        set_workpiece_thicknes_spec->setWorkpieceThickness(std::nullopt);
    }
    setGateValue(CreateGateValue());
}

void AScanInteractor::changeDataCursor() {
    try {
        if (std::cmp_less(getAScanCursor(), aScanIntf() ? aScanIntf()->getDataSize() : 0)) {
            // 1. 更新A扫曲线
            updateAScanSeries();
            // 2. 更新波门曲线
            updateGateSeries(aScanIntf()->getGate(getAScanCursor()));

            // 3. 更新DAC曲线
            updateQuadraticCurveSeries(QuadraticCurveSeriesType::DAC);

            // 4. 更新AVG曲线
            updateQuadraticCurveSeries(QuadraticCurveSeriesType::AVG);

            // 5. 更新AScan图的波门信息显示
            setIsGateEnable(QVariantList{isGateEnable(0), isGateEnable(1)});
            setGateValue(CreateGateValue());
            qCDebug(TAG) << "当前指针:" << getAScanCursor();
            qCDebug(TAG) << "当前门内最高波:" << std::get<1>(aScanIntf()->getGateResult(getAScanCursor()).value_or(std::make_tuple<double, uint8_t>(0, 0)));
            // 6. 更新显示的声程模式
            switch (aScanIntf()->getDistanceMode(getAScanCursor())) {
                case DISTANCE_MODE_Y:
                    setDistanceMode("Y");
                    break;
                case DISTANCE_MODE_X:
                    setDistanceMode("X");
                    break;
                case DISTANCE_MODE_S:
                    setDistanceMode("S");
                    break;
                default:
                    setDistanceMode("N");
            }
            // 7. 更新摄像头控件
            auto camera_special = std::dynamic_pointer_cast<Special::CameraImageSpecial>(aScanIntf());
            if (camera_special != nullptr) {
                setHasCameraImage(camera_special->showCameraImage(getAScanCursor()));
            }
            // 8. 更新B扫或C扫的坐标范围
            updateBOrCScanViewRange();
            updateExtraBScanViewRange();

            // 9. 更新钢轨仿真图
            auto cmp001 = std::dynamic_pointer_cast<Special::CMP001Special>(aScanIntf());
            if (cmp001 && cmp001->isSpecial001Enabled(getAScanCursor())) {
                setShowCMP001Special(true);
            } else {
                setShowCMP001Special(false);
            }
        }
    } catch (std::exception& e) {
        qCCritical(TAG) << e.what();
        qCCritical(TAG) << std::to_string(std::stacktrace::current()).c_str();
    }
}

void AScanInteractor::updateCurrentFrame() {
    // 增益发生变化
    if (aScanIntf() != nullptr) {
        try {
            updateAScanSeries(aScanIntf()->getAScanSeriesData(getAScanCursor(), m_softGain),
                              {aScanIntf()->getAxisBias(getAScanCursor()), 0},
                              {aScanIntf()->getAxisLen(getAScanCursor()), 100.0});
            updateQuadraticCurveSeries(QuadraticCurveSeriesType::DAC);
            updateQuadraticCurveSeries(QuadraticCurveSeriesType::AVG);
            setGateValue(CreateGateValue());
            updateBOrCScanView(false);
            updateBOrCScanViewRange();
            updateExtraBScanView(false);
            updateExtraBScanViewRange();
            auto cmp001 = std::dynamic_pointer_cast<Special::CMP001Special>(aScanIntf());
            if (cmp001 && cmp001->isSpecial001Enabled(getAScanCursor())) {
                setShowCMP001Special(true);
            } else {
                setShowCMP001Special(false);
            }
        } catch (std::exception& e) {
            qCCritical(TAG) << e.what();
            qCCritical(TAG) << std::to_string(std::stacktrace::current()).c_str();
        }
    }
}

void AScanInteractor::updateBOrCScanView(bool set_size) {
    if (aScanIntf() == nullptr) {
        qCWarning(TAG) << "nullptr error:" << std::to_string(std::stacktrace::current()).c_str();
    }

    try {
        if (showCScanView()) {
            const auto cscan_spec = std::dynamic_pointer_cast<Special::CScanSpecial>(aScanIntf());

            if (cscan_spec == nullptr) {
                return;
            }

            const auto frame_per_row = cscan_spec->getCScanXDots();
            const auto frame_per_col = cscan_spec->getCScanYDots();

            auto c_scan_sp = qSharedPointerDynamicCast<Union::View::CScanView>(m_scanViewHandler);
            if (c_scan_sp == nullptr) {
                c_scan_sp = QSharedPointer<Union::View::CScanView>::create();
                setScanViewHandler(c_scan_sp);
            }

            std::vector<std::optional<uint8_t>> cscan_image;

            const auto width  = frame_per_row;
            const auto height = frame_per_col;
            cscan_image.resize(width * height);
            std::ranges::fill(cscan_image, std::nullopt);

            for (auto idx = 0; idx < aScanIntf()->getDataSize(); idx++) {
                uint8_t amp_in_gate;
                std::tie(std::ignore, amp_in_gate) = aScanIntf()->getGateResult(idx).value_or(std::make_tuple<double, uint8_t>(0.0, 0));
                auto [x, y]                        = cscan_spec->getCScanEncoder(idx);
                const auto img_idx                 = y * width + x;
                if (img_idx >= std::ssize(cscan_image)) {
                    continue;
                }
                cscan_image.at(img_idx) = std::min<double>(std::numeric_limits<uint8_t>::max(), CalculateGainOutput(amp_in_gate, getSoftGain()));
            }
            c_scan_sp->replace(cscan_image, width, height, set_size);

            setScanViewHandler(c_scan_sp);
            setSoftGainEnable(true);

        } else if (showBScanView()) {
            const auto bscan_spec = std::dynamic_pointer_cast<Special::BScanSpecial>(aScanIntf());
            const auto mdata_spec = std::dynamic_pointer_cast<::Instance::UnType>(aScanIntf());
            auto       b_scan_sp  = qSharedPointerDynamicCast<Union::View::BScanView>(m_scanViewHandler);
            if (b_scan_sp == nullptr) {
                b_scan_sp = QSharedPointer<Union::View ::BScanView>::create();
                setScanViewHandler(b_scan_sp);
            }

            std::vector<std::optional<uint8_t>> bscan_image;
            const auto                          height = bscan_spec->getBScanXDots();
            int                                 width  = 0;
            for (auto i = 0; i < aScanIntf()->getDataSize(); i++) {
                if (std::ssize(mdata_spec->getDataOrDataInGate(i, 0)) > width) {
                    width = std::ssize(mdata_spec->getDataOrDataInGate(i, 0));
                }
            }

            bscan_image.resize(width * height);
            std::ranges::fill(bscan_image, std::nullopt);

            for (auto idx = 0; idx < aScanIntf()->getDataSize(); idx++) {
                auto data = std::visit(
                    [&, this](auto&& data_in_gate) {
                        return data_in_gate | std::views::transform([this](auto&& val) {
                                   return std::min<double>(std::numeric_limits<uint8_t>::max(), CalculateGainOutput(val, getSoftGain()));
                               }) |
                               std::ranges::to<std::vector<std::optional<uint8_t>>>();
                    },
                    mdata_spec->getDataOrDataInGate(idx, 0));

                auto       frame   = bscan_spec->getBScanEncoder(idx);
                const auto img_idx = width * frame;

                if (img_idx >= std::ssize(bscan_image)) {
                    continue;
                }

                auto dist_ptr = bscan_image.begin() + img_idx;

                for (auto i : std::views::iota(0, std::min<int>(width, static_cast<int>(data.size())))) {
                    *(dist_ptr + i) = data.at(i);
                }
            }

            b_scan_sp->replace(bscan_image, width, height, set_size);

            setScanViewHandlerExtra(nullptr);
            setSoftGainEnable(true);
        } else {
            setScanViewHandler(nullptr);
        }
    } catch (std::exception& e) {
        qCCritical(TAG) << e.what();
        auto stack_trace = std::stacktrace::current();
        qCCritical(TAG) << std::to_string(stack_trace).c_str();
        setScanViewHandler(nullptr);
    }
    emit updateBOrCScanHandler();
}

void AScanInteractor::updateBOrCScanViewRange() {
    if (aScanIntf() == nullptr) {
        qCWarning(TAG) << "nullptr error:" << std::to_string(std::stacktrace::current()).c_str();
    }

    try {
        if (showCScanView()) {
            auto cscan_spec  = std::dynamic_pointer_cast<Special::CScanSpecial>(aScanIntf());
            auto c_scan_view = qSharedPointerDynamicCast<Union::View::CScanView>(m_scanViewHandler);
            if (c_scan_view && cscan_spec) {
                const auto range_x = cscan_spec->getCScanRangeX();
                const auto range_y = cscan_spec->getCScanRangeY();
                c_scan_view->setHorRange(range_x);
                c_scan_view->setVerRange(range_y);
            }

        } else if (showBScanView()) {
            auto bscan_spec  = std::dynamic_pointer_cast<Special::BScanSpecial>(aScanIntf());
            auto b_scan_view = qSharedPointerDynamicCast<Union::View::BScanView>(m_scanViewHandler);
            if (b_scan_view && bscan_spec) {
                if (!bScanIsGateMode()) {
                    const auto range_start = aScanIntf()->getAxisBias(getAScanCursor());
                    const auto range_end   = aScanIntf()->getAxisLen(getAScanCursor()) + range_start;
                    b_scan_view->setHorRange({range_start, range_end});
                } else {
                    auto&& [gate0, _]     = aScanIntf()->getGate(getAScanCursor());
                    auto       axis_range = aScanIntf()->getAxisRange(0);
                    const auto start      = ValueMap(gate0.pos(), axis_range);
                    const auto end        = ValueMap(gate0.pos() + gate0.width(), axis_range);
                    b_scan_view->setHorRange({start, end});
                }

                const auto range = bscan_spec->getBScanRange();
                b_scan_view->setVerRange(range);
            }
        }
    } catch (std::exception& e) {
        qCCritical(TAG) << e.what();
        auto stack_trace = std::stacktrace::current();
        qCCritical(TAG) << std::to_string(stack_trace).c_str();
    }
}

void AScanInteractor::updateExtraBScanView(bool set_size) {
    if (aScanIntf() == nullptr) {
        qCWarning(TAG) << "nullptr error:" << std::to_string(std::stacktrace::current()).c_str();
    }
    try {
        if (showCScanView()) {
            const auto c_scan_spec = std::dynamic_pointer_cast<Special::CScanSpecial>(aScanIntf());
            const auto mdata_spec  = std::dynamic_pointer_cast<::Instance::UnType>(aScanIntf());
            auto       b_scan_sp   = qSharedPointerDynamicCast<Union::View::BScanView>(m_scanViewHandlerExtra);
            if (b_scan_sp == nullptr) {
                b_scan_sp = QSharedPointer<Union::View::BScanView>::create();
                setScanViewHandlerExtra(b_scan_sp);
            }

            std::vector<std::optional<uint8_t>> bscan_image;
            // 获取当前行的B扫数据游标
            const auto line_width = mdata_spec->getCScanXDots();
            const auto line_count = getAScanCursor() / line_width;
            mdata_spec->setExtraBScanLine(line_count);
            auto extra_b_scan_cursors = std::views::iota(line_count * line_width, (line_count + 1) * line_width);

            int width                         = 0;
            auto&& [min_encoder, max_encoder] = mdata_spec->getBScanMinMaxXEncoderValue();
            for (auto idx : extra_b_scan_cursors) {
                auto current_frame_ascan_size = std::ssize(mdata_spec->getDataOrDataInGate(idx, 0));
                if (current_frame_ascan_size > width) {
                    width = current_frame_ascan_size;
                }
            }
            const auto height = max_encoder - min_encoder + 1;

            bscan_image.resize(width * height);
            std::ranges::fill(bscan_image, std::nullopt);

            for (auto idx : extra_b_scan_cursors) {
                auto data = std::visit(
                    [&](auto&& data) {
                        return data | std::views::transform([this](auto&& val) {
                                   return std::min<double>(std::numeric_limits<uint8_t>::max(), CalculateGainOutput(val, getSoftGain()));
                               }) |
                               std::ranges::to<std::vector<std::optional<uint8_t>>>();
                    },
                    mdata_spec->getDataOrDataInGate(idx, 0));

                auto       frame   = mdata_spec->getBScanEncoder(idx);
                const auto img_idx = width * frame;

                if (img_idx >= std::ssize(bscan_image)) {
                    continue;
                }

                auto dist_ptr = bscan_image.data() + img_idx;
                memcpy_s(dist_ptr, width * sizeof(std::optional<uint8_t>),
                         data.data(), std::ssize(data) * sizeof(std::optional<uint8_t>));
            }
            b_scan_sp->replace(bscan_image, width, height, set_size);
        } else {
            setScanViewHandlerExtra(nullptr);
        }
    } catch (std::exception& e) {
        qCCritical(TAG) << e.what();
        auto stack_trace = std::stacktrace::current();
        qCCritical(TAG) << std::to_string(stack_trace).c_str();
    }
    emit updateBScanExtraHandler();
}

void AScanInteractor::updateExtraBScanViewRange() {
    if (aScanIntf() == nullptr) {
        qCWarning(TAG) << "nullptr error:" << std::to_string(std::stacktrace::current()).c_str();
    }

    if (!showCScanView()) {
        return;
    }

    try {
        // VARIFY: 更新C扫坐标轴
        auto       cscan_spec  = std::dynamic_pointer_cast<Special::CScanSpecial>(aScanIntf());
        auto       b_scan_view = qSharedPointerDynamicCast<Union::View::BScanView>(m_scanViewHandlerExtra);
        const auto mdata_spec  = std::dynamic_pointer_cast<::Instance::UnType>(aScanIntf());
        if (b_scan_view && cscan_spec) {
            if (!bScanIsGateMode()) {
                const auto range_start = aScanIntf()->getAxisBias(getAScanCursor());
                const auto range_end   = aScanIntf()->getAxisLen(getAScanCursor()) + range_start;
                b_scan_view->setHorRange({range_start, range_end});
                const auto range_y = mdata_spec->getSubBScanRange();
                b_scan_view->setVerRange({range_y.first, range_y.second});
            } else {
                auto&& [gate0, _]     = aScanIntf()->getGate(getAScanCursor());
                auto       axis_range = aScanIntf()->getAxisRange(0);
                const auto start      = ValueMap(gate0.pos(), axis_range);
                const auto end        = ValueMap(gate0.pos() + gate0.width(), axis_range);
                b_scan_view->setHorRange({start, end});
                const auto range_y = mdata_spec->getSubBScanRange();
                b_scan_view->setVerRange(range_y);
            }
        }

    } catch (std::exception& e) {
        qCCritical(TAG) << e.what();
        auto stack_trace = std::stacktrace::current();
        qCCritical(TAG) << std::to_string(stack_trace).c_str();
    }
}

void AScanInteractor::updateOnDrawGate() {
    try {
        if (std::cmp_less(getAScanCursor(), aScanIntf() ? aScanIntf()->getDataSize() : 0)) {
            // 更新波门曲线
            updateGateSeries(aScanIntf()->getGate(getAScanCursor()));
            // 更新AScan图的波门信息显示
            setIsGateEnable(QVariantList{isGateEnable(0), isGateEnable(1)});
            setGateValue(CreateGateValue());
            // 更新B扫或C扫
            if (bScanIsGateMode()) {
                // 如果B扫是门内模式则需要更新B扫图像
                updateBOrCScanView(false);
                updateBOrCScanViewRange();
            }
            updateExtraBScanView(false);
            updateExtraBScanViewRange();
        }
    } catch (std::exception& e) {
        qCCritical(TAG) << e.what();
        qCCritical(TAG) << std::to_string(std::stacktrace::current()).c_str();
    }
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
    Q_ASSERT(newAScanCursor >= 0);

    if (m_aScanCursor == newAScanCursor) {
        return;
    }

    const auto old_cursor = m_aScanCursor;

    m_aScanCursor = newAScanCursor;

    const auto mdata_spec = std::dynamic_pointer_cast<::Instance::UnType>(aScanIntf());
    if (mdata_spec && mdata_spec->isCScanSubLineChanged(old_cursor, m_aScanCursor)) {
        updateExtraBScanView(true);
    }

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
    auto img_ascan_interface = std::dynamic_pointer_cast<Special::CameraImageSpecial>(aScanIntf());
    if (img_ascan_interface == nullptr) {
        return QImage();
    }
    return img_ascan_interface->getCameraImage(getAScanCursor());
}

QVariantList AScanInteractor::getRailWeldDot() const {
    auto railweld_ascan_interface = std::dynamic_pointer_cast<Special::RailWeldDigramSpecial>(aScanIntf());
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
    try {
        if (aScanIntf() == nullptr) {
            return false;
        }
        auto gate = aScanIntf()->getGate(getAScanCursor());
        return gate.at(gate_idx % 2).enable();
    } catch (std::exception& e) {
        qCCritical(TAG) << e.what();
        auto stack_trace = std::stacktrace::current();
        qCCritical(TAG) << std::to_string(stack_trace).c_str();
        return false;
    }
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

AScanInteractor::AScanType AScanInteractor::aScanIntf() const {
    return m_aScanIntf;
}

void AScanInteractor::setAScanIntf(const AScanType& newAScanIntf) {
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
    return m_scanViewHandler.get();
}

void AScanInteractor::setScanViewHandler(IScanViewPointer newScanViewHandler) {
    if (m_scanViewHandler == newScanViewHandler)
        return;
    m_scanViewHandler = newScanViewHandler;
    emit scanViewHandlerChanged();
}

bool AScanInteractor::softGainEnable() const {
    return m_softGainEnable;
}

void AScanInteractor::setSoftGainEnable(bool newSoftGainEnable) {
    if (m_softGainEnable == newSoftGainEnable)
        return;
    m_softGainEnable = newSoftGainEnable;
    emit softGainEnableChanged();
}

QQuickItem* AScanInteractor::scanViewHandlerExtra() const {
    return m_scanViewHandlerExtra.get();
}

void AScanInteractor::setScanViewHandlerExtra(IScanViewPointer newScanViewHandlerExtra) {
    if (m_scanViewHandlerExtra == newScanViewHandlerExtra)
        return;
    m_scanViewHandlerExtra = newScanViewHandlerExtra;
    emit scanViewHandlerExtraChanged();
}

bool AScanInteractor::isSetWorkpieceThicknessSpecialEnabled() const {
    return m_isSetWorkpieceThicknessSpecialEnabled;
}

void AScanInteractor::setIsSetWorkpieceThicknessSpecialEnabled(bool newIsSetWorkpieceThicknessSpecialEnabled) {
    if (m_isSetWorkpieceThicknessSpecialEnabled == newIsSetWorkpieceThicknessSpecialEnabled)
        return;
    m_isSetWorkpieceThicknessSpecialEnabled = newIsSetWorkpieceThicknessSpecialEnabled;
    emit isSetWorkpieceThicknessSpecialEnabledChanged();
}

QVariant AScanInteractor::workpieceThicknessSpecialValue() const {
    return m_workpieceThicknessSpecialValue;
}

void AScanInteractor::setWorkpieceSpecialValue(const QVariant& newSetWorkpieceSpecialValue) {
    if (m_workpieceThicknessSpecialValue == newSetWorkpieceSpecialValue)
        return;
    m_workpieceThicknessSpecialValue = newSetWorkpieceSpecialValue;

    emit workpieceThicknessSpecialValueChanged();
}

void AScanInteractor::drawGate(int gate_idx, qreal pos, qreal width, qreal height) {
    qCDebug(TAG) << "重绘波门:" << gate_idx << Gate(pos, width, height, true);
    auto overwrite_gate_spexc = std::dynamic_pointer_cast<Special::OverWriteGateInfoSpecial>(aScanIntf());
    if (overwrite_gate_spexc == nullptr) {
        return;
    }
    overwrite_gate_spexc->setGateInfo(gate_idx, Gate(pos, width, height, true));
    updateOnDrawGate();
}

QVariant AScanInteractor::isNearGate(int x, int y, int w, int h, int threshold) {
    if (aScanIntf() == nullptr) {
        return {};
    }

    qCDebug(TAG) << "x:" << x << "y:" << y << "w:" << w << "h:" << h;

    for (int i = 0; i < 2; i++) {
        auto gate = aScanIntf()->getGate(getAScanCursor()).at(i);
        if (!gate.enable()) {
            continue;
        }
        auto rect_gate_head = QRectF(
            gate.pos() * w - threshold,
            (1.0 - gate.height()) * h - threshold,
            gate.width() * w / 2 + 2 * threshold,
            2 * threshold);

        auto rect_gate_tail = QRectF(
            (gate.pos() + gate.width() / 2.0) * w - threshold,
            (1.0 - gate.height()) * h - threshold,
            gate.width() * w / 2.0 + 2 * threshold,
            2 * threshold);

        if (rect_gate_head.contains(x, y)) {
            return QVariantList{i, 0};
        } else if (rect_gate_tail.contains(x, y)) {
            return QVariantList{i, 1};
        }
    }
    qCDebug(TAG) << "未点击到波门附近";
    return {};
}

void AScanInteractor::drawGateDelta(int gate_idx, qreal pos, qreal width, qreal height) {
    auto overwrite_gate_spexc = std::dynamic_pointer_cast<Special::OverWriteGateInfoSpecial>(aScanIntf());
    if (overwrite_gate_spexc == nullptr) {
        return;
    }

    auto gate = aScanIntf()->getGate(getAScanCursor()).at(gate_idx);
    if (!gate.enable()) {
        return;
    }

    gate.enable(true);
    gate.pos(gate.pos() + pos);
    gate.width(gate.width() + width);
    gate.height(gate.height() + height);
    overwrite_gate_spexc->setGateInfo(gate_idx, gate);
    updateOnDrawGate();
}

void AScanInteractor::clearGate() {
    auto overwrite_gate_spexc = std::dynamic_pointer_cast<Special::OverWriteGateInfoSpecial>(aScanIntf());
    if (overwrite_gate_spexc == nullptr) {
        return;
    }
    overwrite_gate_spexc->clearGateInfo();
    updateOnDrawGate();
}

void AScanInteractor::boxSelected(const QRect& rect) {
    auto mdata_type = std::dynamic_pointer_cast<::Union::UniversalApparatus::AScan::Instance::UnType>(aScanIntf());
    if (mdata_type) {
        auto defect_item = mdata_type->calculateDefectRect(rect);
        if (defect_item.has_value()) {
            QRect defect_rect(QPoint(defect_item->left, defect_item->top), QPoint(defect_item->right, defect_item->bottom));
            emit  pushDefectItem(defect_rect, defect_item->peak / 2.0, defect_item->peak_point, defect_item->h, defect_item->a_max, defect_item->area, defect_item->q);
        }
    }
}

bool AScanInteractor::enableOverWriteGate() const {
    return m_enableOverWriteGate;
}

void AScanInteractor::setEnableOverWriteGate(bool newEnableOverWriteGate) {
    if (m_enableOverWriteGate == newEnableOverWriteGate)
        return;
    m_enableOverWriteGate = newEnableOverWriteGate;
    emit enableOverWriteGateChanged();
}

bool AScanInteractor::bScanIsGateMode() const {
    return m_bScanIsGateMode;
}

void AScanInteractor::setBScanIsGateMode(bool newBScanIsGateMode) {
    if (m_bScanIsGateMode == newBScanIsGateMode)
        return;
    m_bScanIsGateMode = newBScanIsGateMode;
    emit bScanIsGateModeChanged();
}

QVariantList AScanInteractor::isGateEnable() const {
    return m_isGateEnable;
}

void AScanInteractor::setIsGateEnable(const QVariantList& newIsGateEnable) {
    if (m_isGateEnable == newIsGateEnable)
        return;
    m_isGateEnable = newIsGateEnable;
    emit isGateEnableChanged();
}

bool AScanInteractor::checkAScanCursorValid() {
    if (!std::cmp_greater(aScanIntf() ? aScanIntf()->getDataSize() : 0, getAScanCursorMax()) || !(getAScanCursorMax() >= 0)) {
        return false;
    }
    return true;
}

void AScanInteractor::setDefaultValue() {
    setSoftGainEnable(false);
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
    setIsSetWorkpieceThicknessSpecialEnabled(false);
    setWorkpieceSpecialValue(QVariant::Invalid);
    setEnableOverWriteGate(false);
    setBScanIsGateMode(false);
}

bool AScanInteractor::openFile(QString filename) {
    try {
        qCInfo(TAG) << "open file:" << filename;
        auto READ_FUNC = AScanFileSelector::Instance()->GetReadFunction(filename.toStdWString());
        if (!READ_FUNC.has_value()) {
            QFileInfo info(filename);
            qCWarning(TAG) << "can't find read interface, file suffix" << info.suffix();
            return false;
        }
        try {
            AScanType ascan_type = nullptr;
            for (const auto& func : READ_FUNC.value()) {
                ascan_type = func(filename.toStdWString());
                if (ascan_type != nullptr) {
                    break;
                }
            }
            setAScanIntf(ascan_type);
            emit aScanIntfChanged();
        } catch (std::exception& e) {
            qCCritical(TAG) << e.what();
            setAScanIntf(nullptr);
            emit aScanIntfChanged();
        }

        if (!aScanIntf()) {
            qCCritical(TAG) << "read file error, fileName:" << filename;
            return false;
        }

        if (aScanIntf()->getDataSize() > 1) {
            setReplayVisible(true);
            setReplayTimerInterval(aScanIntf()->getReplayTimerInterval());
        } else if (aScanIntf()->getDataSize() == 1) {
            setReplayVisible(false);
        } else {
            qCCritical(TAG) << "no data on file:" << filename;
            return false;
        }
        setReportEnabled(aScanIntf()->getReportEnable());
        setDateEnabled(aScanIntf()->getDateEnable());
        setDate(QString::fromStdString(aScanIntf()->getDate(getAScanCursor())));
        setAScanCursorMax(aScanIntf()->getDataSize() - 1);
        auto camera_special = std::dynamic_pointer_cast<Special::CameraImageSpecial>(aScanIntf());
        if (camera_special != nullptr) {
            setHasCameraImage(camera_special->showCameraImage(getAScanCursor()));
        }
        if (getAScanCursor() == 0) {
            changeDataCursor();
        }
        setAScanCursor(0);
        // Special: CMP001
        auto cmp001 = std::dynamic_pointer_cast<Special::CMP001Special>(aScanIntf());
        if (cmp001 && cmp001->isSpecial001Enabled(0)) {
            setShowCMP001Special(true);
        }
        // Special: BScanSpecial
        auto b_scan_sepcial = std::dynamic_pointer_cast<Special::BScanSpecial>(aScanIntf());
        if (b_scan_sepcial && b_scan_sepcial->isSpecialBScanEnabled() && aScanIntf()->getDataSize() > 1) {
            setShowBScanView(true);
            setReplayVisible(false);
            setBScanIsGateMode(b_scan_sepcial->isGateMode());
        }
        // Special: CScanSpecial
        auto c_scan_special = std::dynamic_pointer_cast<Special::CScanSpecial>(aScanIntf());
        if (c_scan_special && c_scan_special->isSpecialCScanEnabled() && aScanIntf()->getDataSize() > 1) {
            if (showBScanView()) {
                qCWarning(TAG) << QObject::tr("同时使用B扫和C扫, B扫已被禁用");
                setShowBScanView(false);
            }
            setShowCScanView(true);
            setReplayVisible(false);
            setBScanIsGateMode(true);
        }
        // Special: SetWorkpieceThickness
        auto workpiece_thickness_spec = std::dynamic_pointer_cast<Special::SetWorkpieceThicknessSpecial>(aScanIntf());
        if (workpiece_thickness_spec) {
            setIsSetWorkpieceThicknessSpecialEnabled(
                workpiece_thickness_spec->isSpecialSetWorkpieceThicknessEnabled());
        }

        if (std::dynamic_pointer_cast<Special::OverWriteGateInfoSpecial>(aScanIntf())) {
            setEnableOverWriteGate(true);
        }

        updateBOrCScanView(true);
        updateBOrCScanViewRange();
        updateExtraBScanView(true);
        updateExtraBScanViewRange();
        setIsGateEnable(QVariantList{isGateEnable(0), isGateEnable(1)});

        // 最后更新A扫数据指针
        if (getAScanCursor() == 0) {
            changeDataCursor();
        }
        setAScanCursor(0);
        return true;
    } catch (std::exception& e) {
        qCCritical(TAG) << e.what();
        auto stack_trace = std::stacktrace::current();
        qCCritical(TAG) << std::to_string(stack_trace).c_str();
        return false;
    }
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
        axis->setMinorTickCount(3);
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
    try {
        updateAScanSeries(aScanIntf()->getAScanSeriesData(getAScanCursor(), m_softGain),
                          {aScanIntf()->getAxisBias(getAScanCursor()), 0},
                          {aScanIntf()->getAxisLen(getAScanCursor()), 100.0});
    } catch (std::exception& e) {
        qCCritical(TAG) << e.what();
        auto stack_trace = std::stacktrace::current();
        qCCritical(TAG) << std::to_string(stack_trace).c_str();
    }
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
    try {
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
        const auto ModifyGain = [&](int idx) -> double {
            switch (type) {
                case QuadraticCurveSeriesType::DAC:
                    if (idx == 0) {
                        return getSoftGain();
                    }
                    return aScanIntf()->getDacLineStandard(getAScanCursor()).at(idx - 1) + getSoftGain();
                case QuadraticCurveSeriesType::AVG:
                    if (idx == 0) {
                        return getSoftGain();
                    }
                    return aScanIntf()->getAvgLineStandard(getAScanCursor()).at(idx - 1) + getSoftGain();
                default:
                    throw std::runtime_error("QuadraticCurveSeriesType error");
            }
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
            int line_count = 0;

            switch (type) {
                case QuadraticCurveSeriesType::DAC:
                    line_count = std::ssize(aScanIntf()->getDacLineStandard(getAScanCursor()));
                    break;
                case QuadraticCurveSeriesType::AVG:
                    line_count = std::ssize(aScanIntf()->getAvgLineStandard(getAScanCursor()));
                    break;
                default:
                    throw std::runtime_error("QuadraticCurveSeriesType error");
            }

            lines.resize(line_count);
            pts.resize(line_count);
            indexs = std::views::iota(1, line_count + 1) | std::views::take(line_count) | std::ranges::to<std::vector<int>>();
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
            lines[i]->attachedAxes().at(0)->setMax(aScanIntf()->getScanData(getAScanCursor()).size() - 1);
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
    } catch (std::exception& e) {
        qCCritical(TAG) << e.what();
        auto stack_trace = std::stacktrace::current();
        qCCritical(TAG) << std::to_string(stack_trace).c_str();
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
    if (gate.enable()) {
        constexpr auto midify_bias = 0.005;
        auto           cmp000      = std::dynamic_pointer_cast<Special::CMP000Special>(aScanIntf());
        if (index == 1 &&
            cmp000 != nullptr &&
            cmp000->isSpecial000Enabled(getAScanCursor()) && cmp000->gateBIsLostType(getAScanCursor())) {
            gateList.append({gate.pos(), gate.height() - midify_bias});
            gateList.append({gate.pos() + midify_bias, gate.height()});
            gateList.append({gate.pos() + gate.width() - midify_bias, gate.height()});
            gateList.append({gate.pos() + gate.width(), gate.height() - midify_bias});
            line->replace(gateList);
            return;
        }
        gateList.append({gate.pos(), gate.height() + midify_bias});
        gateList.append({gate.pos() + midify_bias, gate.height()});
        gateList.append({gate.pos() + gate.width() - midify_bias, gate.height()});
        gateList.append({gate.pos() + gate.width(), gate.height() + midify_bias});
        line->replace(gateList);

        qCDebug(TAG) << "update gate series(" << index << "):" << gate;

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

    m_lastUpdateGateValueTime               = _cur_t;
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
    if (!checkAScanCursorValid()) {
        return QJsonArray::fromVariantList({_m_gateValue[0], _m_gateValue[1]});
    }
    try {
        return aScanIntf()->createGateValue(getAScanCursor(), getSoftGain());
    } catch (...) {}
    return QJsonArray::fromVariantList({_m_gateValue[0], _m_gateValue[1]});
}

void AScanInteractor::updateGateSeries(std::array<::Union::BasicType::Gate, 2> gate) {
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
