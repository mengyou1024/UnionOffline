#include "BasicView.hpp"
#include <QPainter>

namespace Union::View {

    bool BasicView::enableAxis() const {
        return m_enableAxis;
    }

    void BasicView::setEnableAxis(bool newEnableAxis) {
        if (m_enableAxis == newEnableAxis)
            return;
        m_enableAxis = newEnableAxis;
        emit enableAxisChanged();
        QMetaObject::invokeMethod(this, [this]() { this->update(); });
    }

    bool BasicView::enableGrid() const {
        return m_enableGrid;
    }

    void BasicView::setEnableGrid(bool newEnableGrid) {
        if (m_enableGrid == newEnableGrid)
            return;
        m_enableGrid = newEnableGrid;
        emit enableGridChanged();
        QMetaObject::invokeMethod(this, [this]() { this->update(); });
    }

    QPointF BasicView::verticalAxisRange() const {
        return m_verticalAxisRange;
    }

    void BasicView::setVerticalAxisRange(QPointF newVerticalAxisRange) {
        if (m_verticalAxisRange == newVerticalAxisRange)
            return;
        m_verticalAxisRange = newVerticalAxisRange;
        emit verticalAxisRangeChanged();
        QMetaObject::invokeMethod(this, [this]() { this->update(); });
    }

    QPointF BasicView::horizontalAxisRange() const {
        return m_horizontalAxisRange;
    }

    void BasicView::setHorizontalAxisRange(QPointF newHorizontalAxisRange) {
        if (m_horizontalAxisRange == newHorizontalAxisRange)
            return;
        m_horizontalAxisRange = newHorizontalAxisRange;
        emit horizontalAxisRangeChanged();
        QMetaObject::invokeMethod(this, [this]() { this->update(); });
    }

    QColor BasicView::axisBackgroundColor() const {
        return m_axisBackgroundColor;
    }

    void BasicView::setAxisBackgroundColor(const QColor& newAxisBackgroundColor) {
        if (m_axisBackgroundColor == newAxisBackgroundColor)
            return;
        m_axisBackgroundColor = newAxisBackgroundColor;
        emit axisBackgroundColorChanged();
        QMetaObject::invokeMethod(this, [this]() { this->update(); });
    }

    int BasicView::verticalTickCount() const {
        return m_verticalTickCount;
    }

    void BasicView::setVerticalTickCount(int newVerticalTickCount) {
        if (m_verticalTickCount == newVerticalTickCount)
            return;
        m_verticalTickCount = newVerticalTickCount;
        emit verticalTickCountChanged();
        QMetaObject::invokeMethod(this, [this]() { this->update(); });
    }

    int BasicView::horizontalTickCount() const {
        return m_horizontalTickCount;
    }

    void BasicView::setHorizontalTickCount(int newHorizontalTickCount) {
        if (m_horizontalTickCount == newHorizontalTickCount)
            return;
        m_horizontalTickCount = newHorizontalTickCount;
        emit horizontalTickCountChanged();
        QMetaObject::invokeMethod(this, [this]() { this->update(); });
    }

    QColor BasicView::gridColor() const {
        return m_gridColor;
    }

    void BasicView::setgridColor(const QColor& newgridColor) {
        if (m_gridColor == newgridColor)
            return;
        m_gridColor = newgridColor;
        emit gridColorChanged();
        QMetaObject::invokeMethod(this, [this]() { this->update(); });
    }

    void BasicView::paint(QPainter* painter) {
        painter->fillRect(QRect(0, 0, width(), height()), QBrush(Qt::black));
        if (m_enableGrid) {
            drawGrid(painter);
        }
        if (m_enableAxis) {
            drawHorizontalAxis(painter);
            drawVerticalAxis(painter);
        }
    }

    void BasicView::drawHorizontalAxis(QPainter* painter) const {
        const auto drawable = getDrawable();
        painter->fillRect(QRect(AXIS_WIDTH, drawable.bottom() + 1, drawable.width(), AXIS_WIDTH), QBrush(0xafeeee));
        const auto    step         = (m_horizontalAxisRange.y() - m_horizontalAxisRange.x()) / (m_horizontalTickCount * 10.0);
        QFontMetricsF font_metrics = painter->fontMetrics();
        // 水平
        for (int i = 0; i <= m_horizontalTickCount * 10; i++) {
            qreal   x     = std::round(drawable.x() + ((drawable.width() - 1) / (m_horizontalTickCount * 10.0)) * i);
            qreal   y     = drawable.bottom() + 1;
            QColor  color = QColor(0x00cc00);
            QString value = QString::number(m_horizontalAxisRange.x() + step * i, 'f', 1);
            QLineF  line;
            painter->setPen(color);
            if (i % 10 == 0) {
                line = {x, y, x, y + 15};
                if (i == 0) {
                    color = QColor(255, 0, 0);
                    painter->setPen(color);
                } else {
                    color = QColor(0, 0, 0);
                    painter->setPen(color);
                }
                QRectF fm_rect   = font_metrics.boundingRect(value);
                QRectF draw_rect = {};
                if (i != (m_horizontalTickCount * 10)) {
                    draw_rect = QRectF(QPointF(line.p2().x() + 5, line.p2().y() - 5), QSizeF(fm_rect.width(), fm_rect.height()));
                } else {
                    draw_rect = QRectF(QPointF(line.p2().x() - fm_rect.width() - 5, line.p2().y() - 5), QSizeF(fm_rect.width(), fm_rect.height()));
                }
                painter->drawText(draw_rect, Qt::AlignCenter, value);
            } else if (i % 5 == 0) {
                line = {x, y, x, y + 10};
            } else {
                line = {x, y, x, y + 5};
            }
            painter->drawLine(line);
        }
    }

    void BasicView::drawVerticalAxis(QPainter* painter) const {
        const auto drawable = getDrawable();
        painter->fillRect(QRect(0, 0, AXIS_WIDTH, height()), QBrush(0xafeeee));
        auto          step         = (m_verticalAxisRange.y() - m_verticalAxisRange.x()) / (m_verticalTickCount * 10.0);
        QFontMetricsF font_metrics = painter->fontMetrics();
        // 垂直
        for (int i = 0; i <= m_verticalTickCount * 10; i++) {
            qreal x = drawable.left() - 1;
            qreal y = 0;
            if (reverseVerticalAxis()) {
                y = std::round((drawable.height() - 1) / (m_verticalTickCount * 10.0) * ((m_verticalTickCount * 10.0) - i));
            } else {
                y = std::round((drawable.height() - 1) / (m_verticalTickCount * 10.0) * i);
            }
            QColor  color = QColor(0x00cc00);
            QString value = QString::number(m_verticalAxisRange.x() + step * ((m_verticalTickCount * 10) - i), 'f', 1);
            QLineF  line;
            painter->setPen(color);
            if (i % 10 == 0) {
                line = {x - 15, y, x, y};
                if (i == (m_verticalTickCount * 10)) {
                    color = QColor(255, 0, 0);
                    painter->setPen(color);
                } else {
                    color = QColor(0, 0, 0);
                    painter->setPen(color);
                }
                painter->save();
                QRectF fm_rect   = font_metrics.boundingRect(value);
                QRectF draw_rect = QRectF(QPointF(0, 0), QSizeF(fm_rect.width(), fm_rect.height()));

                painter->translate(line.p1().x() + 5, line.p1().y() + 5);
                painter->rotate(90);
                painter->drawText(draw_rect, Qt::AlignCenter, value);
                painter->restore();
            } else if (i % 5 == 0) {
                line = {x - 10, y, x, y};
            } else {
                line = {x - 5, y, x, y};
            }
            painter->drawLine(line);
        }
    }

    bool BasicView::reverseVerticalAxis() const {
        return m_reverseVerticalAxis;
    }

    void BasicView::setReverseVerticalAxis(bool newReverseVerticalAxis) {
        if (m_reverseVerticalAxis == newReverseVerticalAxis)
            return;
        m_reverseVerticalAxis = newReverseVerticalAxis;
        emit reverseVerticalAxisChanged();
    }

    QRect BasicView::getDrawable() const noexcept {
        if (m_enableAxis) {
            return QRect(AXIS_WIDTH, 0, width() - AXIS_WIDTH, height() - AXIS_WIDTH);
        }
        return QRect(0, 0, width(), height());
    }

    void BasicView::drawAxis(QPainter* painter) const noexcept {
        drawHorizontalAxis(painter);
        drawVerticalAxis(painter);
    }

    void BasicView::drawGrid(QPainter* painter) const noexcept {
        auto drawable = getDrawable();
        // 绘制坐标轴
        auto             points = (m_horizontalTickCount + 1) * 2 + (m_verticalTickCount + 1) * 2;
        QVector<QPointF> grid(points);
        // 垂直
        qreal temp;
        for (int32_t i = 0; i < (m_verticalTickCount + 1) * 2; i += 2) {
            temp        = std::round(qreal(drawable.height() - 1) / m_verticalTickCount * i / 2.0 + qreal(drawable.y()));
            grid[i]     = {qreal(drawable.left()), temp};
            grid[i + 1] = {qreal(drawable.right()), temp};
        }
        // 水平
        for (int32_t i = 0; i < (m_horizontalTickCount + 1) * 2; i += 2) {
            temp                                        = std::round(qreal(drawable.width() - 1) / m_horizontalTickCount * i / 2.0 + qreal(drawable.x()));
            grid[(m_verticalTickCount + 1) * 2 + i]     = {temp, qreal(drawable.top())};
            grid[(m_verticalTickCount + 1) * 2 + i + 1] = {temp, qreal(drawable.bottom())};
        }

        // 虚线的画笔
        QVector<qreal> dashes;
        qreal          space = 4;
        dashes << 8 << space;
        QPen pen;
        pen.setBrush(QBrush(m_gridColor));
        pen.setDashPattern(dashes);
        painter->setPen(pen);
        painter->drawLines(grid);
    }

} // namespace Union::View
