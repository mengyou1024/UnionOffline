#include "BScanView.hpp"
#include "ColorTable.hpp"
#include <QCursor>
#include <QLoggingCategory>
#include <QPainter>
#include <union_common.hpp>

[[maybe_unused]] static Q_LOGGING_CATEGORY(TAG, "Union.View.BScanView");

namespace Union::View {

    int BScanView::imgWidth() const {
        return m_imgWidth;
    }

    void BScanView::setImgWidth(int newImgWidth) {
        if (m_imgWidth == newImgWidth)
            return;
        m_imgWidth = newImgWidth;
        emit imgWidthChanged();
    }

    int BScanView::imgHeight() const {
        return m_imgHeight;
    }

    void BScanView::setImgHeight(int newImgHeight) {
        if (m_imgHeight == newImgHeight)
            return;
        m_imgHeight = newImgHeight;
        emit imgHeightChanged();
    }

    bool BScanView::enableCursor() const {
        return m_enableCursor;
    }

    void BScanView::setEnableCursor(bool newEnableCursor) {
        if (m_enableCursor == newEnableCursor)
            return;
        m_enableCursor = newEnableCursor;
        emit enableCursorChanged();
    }

    BScanView::BScanView() {
        setKeepMouseGrab(true);
        setAcceptedMouseButtons(Qt::LeftButton);
        m_reverseVerticalAxis = true;
        m_enableAxis          = false;
    }

    QColor BScanView::cursorLineColor() const {
        return m_cursorLineColor;
    }

    void BScanView::setCursorLineColor(const QColor& newCursorLineColor) {
        if (m_cursorLineColor == newCursorLineColor)
            return;
        m_cursorLineColor = newCursorLineColor;
        emit cursorLineColorChanged();
        QMetaObject::invokeMethod(this, [this]() { this->update(); });
    }

    int BScanView::cursorLineWidth() const {
        return m_cursorLineWidth;
    }

    void BScanView::setCursorLineWidth(int newCursorLineWidth) {
        if (m_cursorLineWidth == newCursorLineWidth)
            return;
        m_cursorLineWidth = newCursorLineWidth;
        emit cursorLineWidthChanged();
    }

    int BScanView::dataCursor() const {
        return m_dataCursor;
    }

    void BScanView::setDataCursor(int newDataCursor) {
        if (m_dataCursor == newDataCursor)
            return;
        m_dataCursor = newDataCursor;
        emit dataCursorChanged();
    }

    void BScanView::resetDrawLine() {
        m_drawLine = std::nullopt;
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
                setWidth(width + AXIS_WIDTH);
                setHeight(height + AXIS_WIDTH);
            }
            QMetaObject::invokeMethod(this, [=, this, image = std::move(image)]() mutable {
                m_image = std::move(image);

                update();
            });
        } catch (const std::exception& e) {
            qCWarning(TAG) << "replace image error";
            qCWarning(TAG) << e.what();
        }
    }

    void BScanView::paint(QPainter* painter) {
        BasicView::paint(painter);

        auto _image = m_image.scaled(getDrawable().size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        auto           image       = QImage(_image.size(), QImage::Format_RGBA8888);
        decltype(auto) COLOR_TABLE = Union::Common::Color::ColorTable::T8ColorTable();
        image.fill(0);

        for (int x = 0; x < _image.width(); x++) {
            for (int y = 0; y < _image.height(); y++) {
                image.setPixel(x, y, COLOR_TABLE.at(_image.pixel(x, y) & 0xFF));
            }
        }

        painter->drawImage(getDrawable(), image);
        if (enableCursor() && m_drawLine.has_value()) {
            painter->setPen(QPen(cursorLineColor(), cursorLineWidth()));
            painter->drawLine(QPoint(getDrawable().left(), m_drawLine.value()), QPoint(getDrawable().right(), m_drawLine.value()));
        }
    }

    void BScanView::mousePressEvent(QMouseEvent* event) {
        if (enableCursor()) {
            eventHandlerCommon(event);
        }
    }

    void BScanView::mouseMoveEvent(QMouseEvent* event) {
        if (enableCursor() && eventHandlerCommon(event)) {
            setCursor(Qt::SizeVerCursor);
        }
    }

    void BScanView::mouseReleaseEvent(QMouseEvent*) {
        if (enableCursor()) {
            setCursor(Qt::ArrowCursor);
        }
    }

    bool BScanView::eventHandlerCommon(QMouseEvent* event) noexcept {
        if (!getDrawable().contains(QPoint(event->x(), event->y()))) {
            return false;
        }

        auto data_cursor = (getDrawable().y() - event->y()) * m_image.height() / getDrawable().height();
        setDataCursor(std::abs(data_cursor));
        m_drawLine = event->y();
        QMetaObject::invokeMethod(this, [this]() { this->update(); });
        return true;
    }
} // namespace Union::View
