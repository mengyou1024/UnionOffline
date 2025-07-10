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
    }

    BScanView::~BScanView() {
        disconnect(this, &IScanView::imagePointChanged, this, nullptr);
        disconnect(AppSetting::Instance(), &AppSetting::bScanImageSmoothingChanged, this, nullptr);
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
            }

            QMetaObject::invokeMethod(this, [=, this, image = std::move(image)]() mutable {
                updateImage(std::move(image));
            });
        } catch (const std::exception& e) {
            qCWarning(TAG) << "replace image error";
            qCWarning(TAG) << e.what();
        }
    }

    void BScanView::paint(QPainter* painter) {
        IScanView::paint(painter);
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

} // namespace Union::View
