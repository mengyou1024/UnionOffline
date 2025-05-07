#include "CScanView.hpp"
#include "ColorTable.hpp"
#include <QCursor>
#include <QLoggingCategory>
#include <QPainter>

[[maybe_unused]] static Q_LOGGING_CATEGORY(TAG, "Union.View.CScanView");

namespace Union::View {

    int CScanView::dataCursorInt() const {
        return m_dataCursorInt;
    }

    void CScanView::setDataCursorInt(int newDataCursorInt) {
        if (m_dataCursorInt == newDataCursorInt)
            return;
        m_dataCursorInt = newDataCursorInt;
        qCDebug(TAG) << "cursorInt changed:" << newDataCursorInt;
        emit dataCursorIntChanged();
    }

    int CScanView::imgWidth() const {
        return m_imgWidth;
    }

    void CScanView::setImgWidth(int newImgWidth) {
        if (m_imgWidth == newImgWidth)
            return;
        m_imgWidth = newImgWidth;
        emit imgWidthChanged();
    }

    int CScanView::imgHeight() const {
        return m_imgHeight;
    }

    void CScanView::setImgHeight(int newImgHeight) {
        if (m_imgHeight == newImgHeight)
            return;
        m_imgHeight = newImgHeight;
        emit imgHeightChanged();
    }

    CScanView::CScanView() {
        setKeepMouseGrab(true);
        setAcceptedMouseButtons(Qt::LeftButton);
        m_reverseVerticalAxis = true;
        m_enableAxis          = false;
    }

    QPoint CScanView::dataCursor() const {
        return m_dataCursor;
    }

    void CScanView::setDataCursor(QPoint newDataCursor) {
        if (m_dataCursor == newDataCursor)
            return;
        m_dataCursor = newDataCursor;
        emit dataCursorChanged();
    }

    QColor CScanView::cursorLineColor() const {
        return m_cursorLineColor;
    }

    void CScanView::setCursorLineColor(const QColor& newCursorLineColor) {
        if (m_cursorLineColor == newCursorLineColor)
            return;
        m_cursorLineColor = newCursorLineColor;
        emit cursorLineColorChanged();
    }

    int CScanView::cursorLineWidth() const {
        return m_cursorLineWidth;
    }

    void CScanView::setCursorLineWidth(int newCursorLineWidth) {
        if (m_cursorLineWidth == newCursorLineWidth)
            return;
        m_cursorLineWidth = newCursorLineWidth;
        emit cursorLineWidthChanged();
    }

    void CScanView::replace(const std::vector<std::optional<uint8_t>>& data, int width, int height, bool set_size) noexcept {
        try {
            auto           image       = QImage(width, height, QImage::Format_RGB888);
            decltype(auto) COLOR_TABLE = Union::Common::Color::ColorTable::T8ColorTable();
            image.fill(0);
            for (int x = 0; x < width; x++) {
                for (int y = 0; y < height; y++) {
                    if (data.at(y * width + x).has_value()) {
                        image.setPixel(x, y, COLOR_TABLE.at(data.at(y * width + x).value()));
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

    void CScanView::paint(QPainter* painter) {
        BasicView::paint(painter);
        painter->drawImage(getDrawable(), m_image);
        if (m_drawPoint.has_value()) {
            painter->setPen(QPen(cursorLineColor(), cursorLineWidth()));
            painter->drawLine(QPoint(getDrawable().left(), m_drawPoint.value().y()), QPoint(getDrawable().right(), m_drawPoint.value().y()));
            painter->drawLine(QPoint(m_drawPoint.value().x(), getDrawable().top()), QPoint(m_drawPoint.value().x(), getDrawable().bottom()));
        }
    }

    void CScanView::mousePressEvent(QMouseEvent* event) {
        eventHandlerCommon(event);
    }

    void CScanView::mouseMoveEvent(QMouseEvent* event) {
        if (eventHandlerCommon(event)) {
            setCursor(Qt::SizeAllCursor);
        }
    }

    void CScanView::mouseReleaseEvent(QMouseEvent*) {
        setCursor(Qt::ArrowCursor);
    }

    bool CScanView::eventHandlerCommon(QMouseEvent* event) noexcept {
        if (!getDrawable().contains(QPoint(event->x(), event->y()))) {
            return false;
        }

        auto cursor_x = (event->x() - getDrawable().x()) * m_image.width() / getDrawable().width();
        auto cursor_y = (event->y() - getDrawable().y()) * m_image.height() / getDrawable().height();
        setDataCursor({cursor_x, cursor_y});
        setDataCursorInt(cursor_y * m_image.width() + cursor_x);
        m_drawPoint = event->pos();
        QMetaObject::invokeMethod(this, [this]() { this->update(); });
        return true;
    }
} // namespace Union::View
