#include "BScanView.hpp"

#include "ColorTable.hpp"
#include <AppSetting.hpp>
#include <QCursor>
#include <QFontMetricsF>
#include <QLoggingCategory>
#include <QPainter>
#include <QPainterPath>
#include <union_common.hpp>

[[maybe_unused]] static Q_LOGGING_CATEGORY(TAG, "Union.View.BScanView");
using namespace ::Union::Common;

namespace Union::View {

    int BScanView::dataCursor() const {
        return m_dataCursor;
    }

    void BScanView::setDataCursor(int newDataCursor) {
        if (m_dataCursor == newDataCursor)
            return;
        m_dataCursor = newDataCursor;
        emit dataCursorChanged();
    }

    BScanView::BScanView() :
    IScanView() {
        display_text_ = "B";
        connect(this, &IScanView::imagePointChanged, this, [this] {
            setDataCursor(imagePoint().y());
        });
        connect(AppSetting::Instance(), &AppSetting::bScanImageSmoothingChanged, this, [this] { update(); });

        connect(this, &BScanView::widthChanged, this, [this] {
            update_defect_mask_size();
            update();
        });
        connect(this, &BScanView::heightChanged, this, [this] {
            update_defect_mask_size();
            update();
        });
    }

    BScanView::~BScanView() {
        disconnect(this, &IScanView::imagePointChanged, this, nullptr);
        disconnect(AppSetting::Instance(), &AppSetting::bScanImageSmoothingChanged, this, nullptr);
        disconnect(this, &BScanView::widthChanged, this, nullptr);
        disconnect(this, &BScanView::heightChanged, this, nullptr);
    }

    void BScanView::pushDefectItem(QRect region, double max_amp, QPoint pos, QString h, QString a_max, QString area, QString q) {
        auto defect_top_left     = raw_image_pos_to_defect_mask_pos(region.topLeft());
        auto defect_bottom_right = raw_image_pos_to_defect_mask_pos(region.bottomRight());

        auto local_top_left     = raw_image_pos_to_local_pos(region.topLeft());
        auto local_bottom_right = raw_image_pos_to_local_pos(region.bottomRight());

        auto local_pos = raw_image_pos_to_local_pos(pos);

        if (!defect_top_left.has_value() || !defect_bottom_right.has_value() ||
            !local_top_left.has_value() || !local_bottom_right.has_value() || !local_pos.has_value()) {
            return;
        }

        auto hor_len = map_to_show_hor_value(defect_bottom_right->x()) - map_to_show_hor_value(defect_top_left->x());
        auto amp_pos = QPointF(map_to_show_hor_value(local_pos->x()), map_to_show_ver_value(local_pos->y()));
        auto s1      = KeepDecimals<1>(map_to_show_ver_value(defect_top_left->y()));
        auto s2      = KeepDecimals<1>(map_to_show_ver_value(defect_bottom_right->y()));
        auto ver_len = s2 - s1;
        defect_list_.emplace_back(DefectItem{hor_len,
                                             ver_len,
                                             max_amp,
                                             amp_pos, QRect(defect_top_left.value(), defect_bottom_right.value()),
                                             s1, s2, h, a_max, area, q});

        qCDebug(TAG) << DefectItem{hor_len, ver_len, max_amp, amp_pos, QRect(defect_top_left.value(), defect_bottom_right.value()),
                                   s1, s2, h, a_max, area, q};

        update();

        emit defectListChanged();
    }

    void BScanView::clearDefectList() {
        if (defect_mask_.has_value()) {
            defect_mask_->fill(Qt::transparent);
        }
        defect_list_.clear();
        update();
        emit defectListChanged();
    }

    void BScanView::delectDefectItem(int idx) {
        if (idx < 0 || idx >= std::ssize(defect_list_)) {
            return;
        }
        defect_list_.erase(defect_list_.begin() + idx);
        update();
        emit defectListChanged();
    }

    void BScanView::locateToDefect(int idx) {
        if (idx < 0 || idx >= std::ssize(defect_list_)) {
            return;
        }
        // 1. 调整坐标区域的显示范围
        if (!::Union::View::contains(horShowRange(), defect_list_.at(idx).pos.x())) {
            auto min_hor_range = min_show_range_width().value_or(0) / 2.0;
            auto min_value     = defect_list_.at(idx).pos.x() - min_hor_range;
            auto max_value     = defect_list_.at(idx).pos.x() + min_hor_range;
            if (min_value < horRange().first) {
                min_value = horRange().first;
            }
            if (max_value > horRange().second) {
                max_value = horRange().second;
            }

            setHorShowRange({min_value, max_value});
        }

        if (!Union::View::contains(verShowRange(), defect_list_.at(idx).pos.y())) {
            auto min_ver_range = min_show_range_height().value_or(0) / 2.0;
            auto min_value     = defect_list_.at(idx).pos.y() - min_ver_range;
            auto max_value     = defect_list_.at(idx).pos.y() + min_ver_range;

            if (min_value < verRange().first) {
                min_value = verRange().first;
            }
            if (max_value > verRange().second) {
                max_value = verRange().second;
            }

            setVerShowRange({min_value, max_value});
        }

        // 2. 调整红色测量线的位置
        auto x = ValueMap(defect_list_.at(idx).pos.x(), drawable_hor_range(), horShowRange());
        auto y = ValueMap(defect_list_.at(idx).pos.y(), drawable_ver_range(), verShowRange());
        setMeasuringPointRed(QPoint(KeepDecimals<0>(x), KeepDecimals<0>(y)));
    }

    void BScanView::replace(const std::vector<std::optional<uint8_t>>& data, int width, int height, bool set_size) noexcept {
        try {
            auto           image       = QImage(width, height, QImage::Format_RGB888);
            decltype(auto) COLOR_TABLE = Union::Common::Color::ColorTable::BlackWhiteGradient();
            image.fill(0);
            for (int x = 0; x < width; x++) {
                for (int y = 0; y < height; y++) {
                    if (data.at(y * width + x).has_value()) {
                        auto value     = data.at(y * width + x).value();
                        auto new_value = Common::ValueMap(value, {0, 255}, {0, 200});
                        if (new_value > 255.0) {
                            new_value = 255;
                        } else if (new_value < 0) {
                            new_value = 0;
                        }

                        image.setPixel(x, y, COLOR_TABLE.at(static_cast<uint8_t>(new_value)));
                    }
                }
            }

            if (set_size) {
                resetMeasuringLine();
                clearDefectList();
            }

            QMetaObject::invokeMethod(this, [=, this, image = std::move(image)]() mutable {
                updateImage(std::move(image));
                update_defect_mask_size();
            });
        } catch (const std::exception& e) {
            qCWarning(TAG) << "replace image error";
            qCWarning(TAG) << e.what();
        }
    }

    void BScanView::paint(QPainter* painter) {
        IScanView::paint(painter);

        painter->save();
        draw_box_defect(painter);
        painter->restore();
    }

    void BScanView::draw_image(const QImage& img, QPainter* painter) {
        auto transformation_mode = Qt::SmoothTransformation;
        if (AppSetting::Instance()->bScanImageSmoothing()) {
            transformation_mode = Qt::SmoothTransformation;
        } else {
            transformation_mode = Qt::FastTransformation;
        }
        auto           image_scaled  = img.scaled(drawable().size(), Qt::IgnoreAspectRatio, transformation_mode);
        auto           image_to_draw = QImage(image_scaled.size(), QImage::Format_RGBA8888);
        decltype(auto) COLOR_TABLE   = Union::Common::Color::ColorTable::T8ColorTable();
        image_to_draw.fill(0);

        for (int x = 0; x < image_scaled.width(); x++) {
            for (int y = 0; y < image_scaled.height(); y++) {
                image_to_draw.setPixel(x, y, COLOR_TABLE.at(image_scaled.pixel(x, y) & 0xFF));
            }
        }
        painter->drawImage(drawable(), image_to_draw);
    }

    void BScanView::draw_box_defect(QPainter* painter) {
        if (!defect_mask_.has_value()) {
            return;
        }

        {
            defect_mask_->fill(Qt::transparent);
            QPainter image_painter(&(*defect_mask_));
            QPen     pen = image_painter.pen();
            pen.setWidthF(1.5);
            pen.setBrush(Qt::black);
            image_painter.setPen(pen);

            auto font = image_painter.font();
            font.setBold(true);
            font.setPointSize(13);
            image_painter.setFont(font);
            QFontMetricsF font_metrics = image_painter.fontMetrics();

            for (int i = 0; i < std::ssize(defect_list_); i++) {
                image_painter.drawRect(defect_list_.at(i).rect);
                auto text_box = font_metrics.boundingRect(QString::number(i + 1));

                text_box.moveBottom(defect_list_.at(i).rect.topLeft().y() - 5);

                if (text_box.top() < 0) {
                    text_box.moveTop(defect_list_.at(i).rect.bottomRight().y() + 5);
                }

                text_box.moveLeft(defect_list_.at(i).rect.topLeft().x() - 5);

                if (text_box.left() < 0) {
                    text_box.moveRight(defect_list_.at(i).rect.bottomRight().y() + 5);
                }

                image_painter.drawText(text_box, QString::number(i + 1));

                const auto radius = std::max(text_box.width(), text_box.height());

                image_painter.drawEllipse(text_box.center(), radius / 2.0, radius / 2.0);
            }
        }

        Range image_h_range = {0, defect_mask_->width() - 1};
        Range image_v_range = {0, defect_mask_->height() - 1};

        int left   = ValueMap(horShowRange().first, image_h_range, horRange());
        int right  = ValueMap(horShowRange().second, image_h_range, horRange());
        int top    = ValueMap(verShowRange().first, image_v_range, verRange());
        int bottom = ValueMap(verShowRange().second, image_v_range, verRange());

        painter->drawImage(drawable(), *defect_mask_, QRect(QPoint(left, top), QPoint(right, bottom)));
    }

    void BScanView::update_defect_mask_size() {
        if (!image_raw_.has_value()) {
            defect_mask_ = std::nullopt;
            return;
        }

        auto size = image_raw_->size();

        if (is_raw_image_width_can_contain()) {
            size.setWidth(drawable_size().width());
        }

        if (is_raw_image_height_can_contain()) {
            size.setHeight(drawable_size().height());
        }

        defect_mask_ = QImage(size, QImage::Format_RGBA8888);
        defect_mask_->fill(Qt::transparent);
    }

    std::optional<QPoint> BScanView::local_pos_to_defect_mask_pos(QPoint pt) const {
        if (!image_raw_.has_value() || !defect_mask_.has_value() || !image_visable_.has_value()) {
            return std::nullopt;
        }

        auto size = image_raw_->size();

        if (is_raw_image_width_can_contain()) {
            size.setWidth(drawable_size().width());
        }

        if (is_raw_image_height_can_contain()) {
            size.setHeight(drawable_size().height());
        }

        auto show_size = image_visable_->size();

        if (is_raw_image_width_can_contain()) {
            show_size.setWidth(drawable_size().width());
        }

        if (is_raw_image_height_can_contain()) {
            show_size.setHeight(drawable_size().height());
        }

        Range image_h_range = {0, size.width() - 1};
        Range image_v_range = {0, size.height() - 1};

        double x_bias = ValueMap(horShowRange().first, image_h_range, horRange());
        double y_bias = ValueMap(verShowRange().first, image_v_range, verRange());

        int x = x_bias + (pt.x() - drawable().left()) / (drawable_size().width() / static_cast<double>(show_size.width()));
        int y = y_bias + pt.y() / (drawable_size().height() / static_cast<double>(show_size.height()));

        if (x < 0 || x >= defect_mask_->width() || y < 0 || y >= defect_mask_->height()) {
            return std::nullopt;
        }

        return QPoint(x, y);
    }

    std::optional<QPoint> BScanView::raw_image_pos_to_defect_mask_pos(QPoint pt) const {
        return raw_image_pos_to_local_pos(pt)
            .and_then([this](QPoint pt) -> std::optional<QPoint> {
                return local_pos_to_defect_mask_pos(pt);
            });
    }

} // namespace Union::View

QDebug operator<<(QDebug debug, const ::Union::View::DefectItem& defect) {
    debug << "DefectItem { hor_len:" << defect.hor_len << ", ver_len:" << defect.ver_len
          << ", max_amp:" << defect.max_amp << ", amp_pos:" << defect.pos << ", rect:" << defect.rect
          << ", s1:" << defect.s1 << ", s2:" << defect.s2 << ", h" << defect.h
          << ", aMax:" << defect.a_max << ", region:" << defect.region;

    return debug;
}
