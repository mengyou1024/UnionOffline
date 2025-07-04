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

    BScanView::CursorLocation BScanView::cursorLocation() const {
        return m_cursorLocation;
    }

    void BScanView::setCursorLocation(CursorLocation newCursorLocation) {
        if (m_cursorLocation == newCursorLocation)
            return;
        m_cursorLocation = newCursorLocation;
        emit cursorLocationChanged();
        update();
    }

    bool BScanView::isPressed() const {
        return m_isPressed;
    }

    void BScanView::setIsPressed(bool newIsPressed) {
        if (m_isPressed == newIsPressed)
            return;
        m_isPressed = newIsPressed;
        emit isPressedChanged();
    }

    bool BScanView::isControlPressed() const {
        return m_isControlPressed;
    }

    void BScanView::setIsControlPressed(bool newIsControlPressed) {
        if (m_isControlPressed == newIsControlPressed)
            return;
        m_isControlPressed = newIsControlPressed;
        emit isControlPressedChanged();
    }

    void BScanView::setRedHLineFromCScan(qreal pos) {
        m_measuringPointRed.setY(pos * (getDrawable().height() - 1));
        update();
    }

    void BScanView::setBlueHLineFromCScan(qreal pos) {
        m_measuringPointBlue.setY(pos * (getDrawable().height() - 1));
        update();
    }

    void BScanView::setValueFromCScan(qreal red, qreal blue) {
        _red_h_value  = red;
        _blue_h_value = blue;
    }

    void BScanView::setRedValueFromCScan(qreal red) {
        _red_h_value = red;
        update();
    }

    void BScanView::setBlueValueFromCScan(qreal blue) {
        _blue_h_value = blue;
        update();
    }

    void BScanView::clearValueFromCScan() {
        _red_h_value  = std::nullopt;
        _blue_h_value = std::nullopt;
    }

    BScanView::BScanView() {
        setFocus(true);
        setKeepMouseGrab(true);
        setAcceptHoverEvents(true);
        setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
        m_reverseVerticalAxis = true;
        m_enableAxis          = false;
        connect(AppSetting::Instance(), &AppSetting::bScanImageSmoothingChanged, this, [this] { update(); });
        resetLines();
        connect(this, &BScanView::heightChanged, this, &BScanView::resetLines);
        connect(this, &BScanView::widthChanged, this, &BScanView::resetLines);
    }

    BScanView::~BScanView() {
        disconnect(AppSetting::Instance(), &AppSetting::bScanImageSmoothingChanged, this, nullptr);
        disconnect(this, &BScanView::heightChanged, this, &BScanView::resetLines);
        disconnect(this, &BScanView::widthChanged, this, &BScanView::resetLines);
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

    void BScanView::resetLines() {
        m_measuringPointRed  = QPoint(width() * 0.2, height() * 0.2);
        m_measuringPointBlue = QPoint(width() * 0.4, height() * 0.4);
        update();
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
                resetLines();
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

        auto transformation_mode = Qt::SmoothTransformation;

        if (AppSetting::Instance()->bScanImageSmoothing()) {
            transformation_mode = Qt::SmoothTransformation;
        } else {
            transformation_mode = Qt::FastTransformation;
        }

        auto _image = m_image.scaled(getDrawable().size(), Qt::IgnoreAspectRatio, transformation_mode);

        auto           image       = QImage(_image.size(), QImage::Format_RGBA8888);
        decltype(auto) COLOR_TABLE = Union::Common::Color::ColorTable::T8ColorTable();
        image.fill(0);

        for (int x = 0; x < _image.width(); x++) {
            for (int y = 0; y < _image.height(); y++) {
                image.setPixel(x, y, COLOR_TABLE.at(_image.pixel(x, y) & 0xFF));
            }
        }

        painter->drawImage(getDrawable(), image);
        if (enableCursor()) {
            drawMeasuringLine(painter);
            drawMeasuringText(painter);
        }
    }

    void BScanView::mousePressEvent(QMouseEvent* event) {
        forceActiveFocus();
        if (enableCursor()) {
            setIsPressed(true);
            if (isControlPressed() && event->button() == Qt::LeftButton) {
                auto data_cursor = (getDrawable().y() - event->y()) * m_image.height() / getDrawable().height();
                setDataCursor(std::abs(data_cursor));
                m_measuringPointRed = event->pos();
                update();
            } else if (isControlPressed() && event->button() == Qt::RightButton) {
                m_measuringPointBlue = event->pos();
                update();
            }
        }
    }

    void BScanView::mouseMoveEvent(QMouseEvent* event) {
        if (enableCursor() && eventHandlerCommon(event)) {}
    }

    void BScanView::mouseReleaseEvent(QMouseEvent*) {
        if (enableCursor()) {
            setIsPressed(false);
        }
    }

    void BScanView::hoverEnterEvent(QHoverEvent*) {
        forceActiveFocus();
    }

    void BScanView::hoverMoveEvent(QHoverEvent* event) {
        if (!getDrawable().contains(event->pos())) {
            return;
        }

        if (!enableCursor()) {
            return;
        }

        if (isPressed()) {
            return;
        }

        constexpr auto NEAR_THRESH = 5;

        bool size_horizontal_cursor = false;
        bool size_vertical_cursor   = false;

        auto cursor_location = CursorLocation::LOCATION_NEAR_NONE;

        if (qAbs(event->pos().x() - m_measuringPointRed.x()) <= NEAR_THRESH) {
            cursor_location        = static_cast<CursorLocation>(cursor_location | CursorLocation::LOCATION_NEAR_RED_VERTIVAL_LINE);
            size_horizontal_cursor = true;
        } else if (qAbs(event->pos().x() - m_measuringPointBlue.x()) <= NEAR_THRESH) {
            cursor_location        = static_cast<CursorLocation>(cursor_location | CursorLocation::LOCATION_NEAR_BLUE_VERTIVAL_LINE);
            size_horizontal_cursor = true;
        }

        if (qAbs(event->pos().y() - m_measuringPointRed.y()) <= NEAR_THRESH) {
            cursor_location      = static_cast<CursorLocation>(cursor_location | CursorLocation::LOCATION_NEAR_RED_HORIZONTAL_LINE);
            size_vertical_cursor = true;
        } else if (qAbs(event->pos().y() - m_measuringPointBlue.y()) <= NEAR_THRESH) {
            cursor_location      = static_cast<CursorLocation>(cursor_location | CursorLocation::LOCATION_NEAR_BLUE_HORIZONTAL_LINE);
            size_vertical_cursor = true;
        }

        if (size_horizontal_cursor && size_vertical_cursor) {
            setCursor(Qt::SizeAllCursor);
        } else if (size_horizontal_cursor) {
            setCursor(Qt::SizeHorCursor);
        } else if (size_vertical_cursor) {
            setCursor(Qt::SizeVerCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }

        setCursorLocation(cursor_location);
    }

    void BScanView::hoverLeaveEvent(QHoverEvent*) {
        setCursorLocation(CursorLocation::LOCATION_NEAR_NONE);
        setIsPressed(false);
        setCursor(Qt::ArrowCursor);
        setIsControlPressed(false);
    }

    void BScanView::keyPressEvent(QKeyEvent* event) {
        if (event->key() == Qt::Key_Control) {
            setIsControlPressed(true);
        }

        if (event->key() == Qt::Key_W) {
            if (m_measuringPointRed.y() == getDrawable().top()) {
                return;
            }
            m_measuringPointRed.setY(m_measuringPointRed.y() - 1);
            auto data_cursor = (getDrawable().y() - m_measuringPointRed.y()) * m_image.height() / getDrawable().height();
            setDataCursor(std::abs(data_cursor));
            update();
        }

        if (event->key() == Qt::Key_S) {
            if (m_measuringPointRed.y() == getDrawable().bottom()) {
                return;
            }
            m_measuringPointRed.setY(m_measuringPointRed.y() + 1);
            auto data_cursor = (getDrawable().y() - m_measuringPointRed.y()) * m_image.height() / getDrawable().height();
            setDataCursor(std::abs(data_cursor));
            update();
        }

        if (event->key() == Qt::Key_A) {
            if (m_measuringPointRed.x() == getDrawable().left()) {
                return;
            }
            m_measuringPointRed.setX(m_measuringPointRed.x() - 1);
            update();
        }

        if (event->key() == Qt::Key_D) {
            if (m_measuringPointRed.x() == getDrawable().right()) {
                return;
            }
            m_measuringPointRed.setX(m_measuringPointRed.x() + 1);
            update();
        }

        if (event->key() == Qt::Key_Up) {
            if (m_measuringPointBlue.y() == getDrawable().top()) {
                return;
            }
            m_measuringPointBlue.setY(m_measuringPointBlue.y() - 1);
            update();
        }

        if (event->key() == Qt::Key_Down) {
            if (m_measuringPointBlue.y() == getDrawable().bottom()) {
                return;
            }
            m_measuringPointBlue.setY(m_measuringPointBlue.y() + 1);
            update();
        }

        if (event->key() == Qt::Key_Left) {
            if (m_measuringPointBlue.x() == getDrawable().left()) {
                return;
            }
            m_measuringPointBlue.setX(m_measuringPointBlue.x() - 1);
            update();
        }

        if (event->key() == Qt::Key_Right) {
            if (m_measuringPointBlue.x() == getDrawable().right()) {
                return;
            }
            m_measuringPointBlue.setX(m_measuringPointBlue.x() + 1);
            update();
        }
    }

    void BScanView::keyReleaseEvent(QKeyEvent* event) {
        if (event->key() == Qt::Key_Control) {
            setIsControlPressed(false);
        }
    }

    bool BScanView::eventHandlerCommon(QMouseEvent* event) noexcept {
        if (!getDrawable().contains(event->pos())) {
            return false;
        }

        if (cursorLocation() & CursorLocation::LOCATION_NEAR_RED_VERTIVAL_LINE) {
            m_measuringPointRed.setX(event->pos().x());
        }

        if (cursorLocation() & CursorLocation::LOCATION_NEAR_RED_HORIZONTAL_LINE) {
            auto data_cursor = (getDrawable().y() - event->y()) * m_image.height() / getDrawable().height();
            setDataCursor(std::abs(data_cursor));
            m_measuringPointRed.setY(event->pos().y());
            emit updateCScanRedVLine(static_cast<qreal>(std::abs(getDrawable().y() - event->y())) / getDrawable().height());
        }

        if (cursorLocation() & CursorLocation::LOCATION_NEAR_BLUE_VERTIVAL_LINE) {
            m_measuringPointBlue.setX(event->pos().x());
        }

        if (cursorLocation() & CursorLocation::LOCATION_NEAR_BLUE_HORIZONTAL_LINE) {
            m_measuringPointBlue.setY(event->pos().y());
            emit updateCScanBlueVLine(static_cast<qreal>(std::abs(getDrawable().y() - event->y())) / getDrawable().height());
        }

        update();
        return true;
    }

    void BScanView::drawMeasuringLine(QPainter* painter) {
        Q_ASSERT(painter);
        if (!enableCursor()) {
            return;
        }

        QColor red_line_vertical    = Qt::red;
        QColor red_line_horizontal  = Qt::red;
        QColor red_line_quad        = Qt::red;
        QColor blue_line_vertical   = Qt::blue;
        QColor blue_line_horizontal = Qt::blue;
        QColor blue_line_quad       = Qt::blue;

        if (cursorLocation() & CursorLocation::LOCATION_NEAR_RED_VERTIVAL_LINE) {
            red_line_vertical = Qt::green;
            red_line_quad     = Qt::green;
        }

        if (cursorLocation() & CursorLocation::LOCATION_NEAR_RED_HORIZONTAL_LINE) {
            red_line_horizontal = Qt::green;
            red_line_quad       = Qt::green;
        }

        if (cursorLocation() & CursorLocation::LOCATION_NEAR_BLUE_VERTIVAL_LINE) {
            blue_line_vertical = Qt::green;
            blue_line_quad     = Qt::green;
        }

        if (cursorLocation() & CursorLocation::LOCATION_NEAR_BLUE_HORIZONTAL_LINE) {
            blue_line_horizontal = Qt::green;
            blue_line_quad       = Qt::green;
        }

        painter->setPen(QPen(blue_line_horizontal, 2));
        painter->drawLine(QPoint(getDrawable().left(), m_measuringPointBlue.y()), QPoint(getDrawable().right(), m_measuringPointBlue.y()));
        painter->setPen(QPen(blue_line_vertical, 2));
        painter->drawLine(QPoint(m_measuringPointBlue.x(), getDrawable().top()), QPoint(m_measuringPointBlue.x(), getDrawable().bottom()));
        painter->setPen(QPen(blue_line_quad, 2));

        painter->setPen(QPen(red_line_horizontal, 2));
        painter->drawLine(QPoint(getDrawable().left(), m_measuringPointRed.y()), QPoint(getDrawable().right(), m_measuringPointRed.y()));
        painter->setPen(QPen(red_line_vertical, 2));
        painter->drawLine(QPoint(m_measuringPointRed.x(), getDrawable().top()), QPoint(m_measuringPointRed.x(), getDrawable().bottom()));
        painter->setPen(QPen(red_line_quad, 2));
    }

    void BScanView::drawMeasuringText(QPainter* painter) {
        painter->setPen(QPen(Qt::black, 1));
        auto font = painter->font();
        font.setBold(true);
        font.setPointSize(13);
        painter->setFont(font);
        QFontMetricsF font_metrics = painter->fontMetrics();

        const auto value_vertical_red = Common::KeepDecimals<1>(Common::ValueMap(
            m_measuringPointRed.x(),
            {m_horizontalAxisRange.x(), m_horizontalAxisRange.y()},
            {getDrawable().left(), getDrawable().right()}));

        const auto value_vertical_blue = Common::KeepDecimals<1>(Common::ValueMap(
            m_measuringPointBlue.x(),
            {m_horizontalAxisRange.x(), m_horizontalAxisRange.y()},
            {getDrawable().left(), getDrawable().right()}));

        const auto value_vertical_bias = Common::KeepDecimals<1>(value_vertical_blue - value_vertical_red);

        const auto value_horizontal_red = Common::KeepDecimals<1>(_red_h_value.value_or(Common::ValueMap(
            m_measuringPointRed.y(),
            {m_verticalAxisRange.x(), m_verticalAxisRange.y()},
            {getDrawable().top(), getDrawable().bottom()})));

        const auto value_horizontal_blue = Common::KeepDecimals<1>(_blue_h_value.value_or(Common::ValueMap(
            m_measuringPointBlue.y(),
            {m_verticalAxisRange.x(), m_verticalAxisRange.y()},
            {getDrawable().top(), getDrawable().bottom()})));

        const auto value_horizontal_bias = Common::KeepDecimals<1>(value_horizontal_blue - value_horizontal_red);

        QString str  = QString::asprintf("%.1f", value_vertical_red);
        auto    rect = font_metrics.boundingRect(str);

        if (m_measuringPointRed.x() <= (width() / 2.0)) {
            rect.moveTo(m_measuringPointRed.x() + 5, 0);
        } else {
            rect.moveTo(m_measuringPointRed.x() - rect.width() - 5, 0);
        }
        painter->setPen(QPen(Qt::black, 1));
        painter->drawText(rect.bottomLeft(), str);

        str  = QString::asprintf("%.1f", value_vertical_blue);
        rect = font_metrics.boundingRect(str);

        if (m_measuringPointBlue.x() <= (width() / 2.0)) {
            rect.moveTo(m_measuringPointBlue.x() + 5, 20);
        } else {
            rect.moveTo(m_measuringPointBlue.x() - rect.width() - 5, 20);
        }
        painter->setPen(QPen(Qt::black, 1));
        painter->drawText(rect.bottomLeft(), str);

        str  = QString::asprintf("水平间距: %.1f", value_vertical_bias);
        rect = font_metrics.boundingRect(str);

        rect.moveTopRight(getDrawable().topRight() + QPoint(-10, 0));
        painter->setPen(QPen(Qt::black, 1));
        painter->drawText(rect.bottomLeft(), str);

        str  = QString::asprintf("%.1f", value_horizontal_red);
        rect = font_metrics.boundingRect(str);

        if (m_measuringPointRed.y() <= (height() / 2.0)) {
            if (m_measuringPointRed.x() < rect.width() + 50) {
                rect.moveTopLeft(QPoint(0, m_measuringPointRed.y()));
            } else {
                rect.moveTopRight(m_measuringPointRed + QPoint(-50, 0));
            }
        } else {
            if (m_measuringPointRed.x() < rect.width() + 50) {
                rect.moveTopLeft(QPoint(0, m_measuringPointRed.y()));
            } else {
                rect.moveBottomRight(m_measuringPointRed + QPoint(-50, -10));
            }
        }
        painter->setPen(QPen(Qt::black, 1));
        painter->drawText(rect.bottomLeft(), str);

        str  = QString::asprintf("%.1f", value_horizontal_blue);
        rect = font_metrics.boundingRect(str);

        if (m_measuringPointBlue.y() <= (height() / 2.0)) {
            if (m_measuringPointBlue.x() < getDrawable().right() - rect.width() - 50) {
                rect.moveTopLeft(m_measuringPointBlue + QPoint(50, 0));
            } else {
                rect.moveTopRight(QPoint(getDrawable().right(), m_measuringPointBlue.y()));
            }
        } else {
            if (m_measuringPointBlue.x() < getDrawable().right() - rect.width() - 50) {
                rect.moveBottomLeft(m_measuringPointBlue + QPoint(50, -10));
            } else {
                rect.moveBottomRight(QPoint(getDrawable().right(), m_measuringPointBlue.y()));
            }
        }

        painter->setPen(QPen(Qt::black, 1));
        painter->drawText(rect.bottomLeft(), str);

        str  = QString::asprintf("垂直间距: %.1f", value_horizontal_bias);
        rect = font_metrics.boundingRect(str);

        rect.moveTopRight(getDrawable().topRight() + QPoint(-10, rect.height()));
        painter->setPen(QPen(Qt::black, 1));
        painter->drawText(rect.bottomLeft(), str);
    }
} // namespace Union::View
