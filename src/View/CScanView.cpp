#include "CScanView.hpp"

#include "ColorTable.hpp"
#include <AppSetting.hpp>
#include <QCursor>
#include <QFontMetricsF>
#include <QLoggingCategory>
#include <QPainter>
#include <QPainterPath>
#include <union_common.hpp>

[[maybe_unused]] static Q_LOGGING_CATEGORY(TAG, "Union.View.CScanView");
using namespace ::Union::Common;

namespace Union::View {

    int CScanView::dataCursor() const {
        return m_dataCursor;
    }

    void CScanView::setDataCursor(int newDataCursor) {
        if (m_dataCursor == newDataCursor)
            return;
        m_dataCursor = newDataCursor;
        emit dataCursorChanged();
    }

    int CScanView::toIntDataCursor(QPoint pt) {
        return pt.y() * _raw_width + pt.x();
    }

    CScanView::CScanView() :
    IScanView() {
        display_text_ = "C";
        connect(this, &IScanView::imagePointChanged, this, [this] {
            setDataCursor((imagePoint().y() * _raw_width + imagePoint().x()));
        });
        connect(AppSetting::Instance(), &AppSetting::cScanImageSmoothingChanged, this, [this] { update(); });
        connect(AppSetting::Instance(), &AppSetting::displayCScanLayerLineChanged, this, [this] { update(); });
    }

    CScanView::~CScanView() {
        disconnect(this, &IScanView::imagePointChanged, this, nullptr);
        disconnect(AppSetting::Instance(), &AppSetting::cScanImageSmoothingChanged, this, nullptr);
        disconnect(AppSetting::Instance(), &AppSetting::displayCScanLayerLineChanged, this, nullptr);
    }

    void CScanView::replace(const std::vector<std::optional<uint8_t>>& data, int width, int height, bool set_size) noexcept {
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

            _raw_width  = width;
            _raw_height = height;

            if (set_size) {
                resetMeasuringLine();
            }

            QMetaObject::invokeMethod(this, [=, this, image = std::move(image)]() mutable {
                updateImage(std::move(image));
                update();
            });

        } catch (const std::exception& e) {
            qCWarning(TAG) << "replace image error";
            qCWarning(TAG) << e.what();
        }
    }

    void CScanView::paint(QPainter* painter) {
        IScanView::paint(painter);
    }

    void CScanView::draw_image(const QImage& img, QPainter* painter) {
        auto transformation_mode = Qt::SmoothTransformation;

        if (AppSetting::Instance()->cScanImageSmoothing()) {
            transformation_mode = Qt::SmoothTransformation;
        } else {
            transformation_mode = Qt::FastTransformation;
        }

        auto image_scaled = img.scaled(drawable_size(), Qt::IgnoreAspectRatio, transformation_mode);

        auto           image_to_draw = QImage(image_scaled.size(), QImage::Format_RGBA8888);
        decltype(auto) COLOR_TABLE   = Union::Common::Color::ColorTable::T8ColorTable();
        image_to_draw.fill(0);

        for (int x = 0; x < image_scaled.width(); x++) {
            for (int y = 0; y < image_scaled.height(); y++) {
                image_to_draw.setPixel(x, y, COLOR_TABLE.at(image_scaled.pixel(x, y) & 0xFF));
            }
        }

        painter->drawImage(drawable(), image_to_draw);

        if (AppSetting::Instance()->displayCScanLayerLine()) {
            painter->setPen(QPen(Qt::white, 1));
            for (int index = 1; index < img.height(); index++) {
                const auto y = static_cast<double>(drawable().height()) / img.height() * index;
                painter->drawLine(drawable().left(), y, drawable().right(), y);
            }
        }
    }

} // namespace Union::View
