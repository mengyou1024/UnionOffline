#include "AxisView.hpp"

#include "union_common.hpp"
#include <AppSetting.hpp>
#include <QPainter>

namespace Union::View {

    AxisView::AxisView() {
        connect(AppSetting::Instance(), &AppSetting::roundToIntChanged, this, [this] { update(); });
    }

    AxisView::~AxisView() {
        disconnect(AppSetting::Instance(), &AppSetting::roundToIntChanged, this, nullptr);
    }

    bool AxisView::isVertical() const {
        return m_isVertical;
    }

    void AxisView::setIsVertical(bool newIsVertical) {
        if (m_isVertical == newIsVertical)
            return;
        m_isVertical = newIsVertical;
        emit isVerticalChanged();
        QMetaObject::invokeMethod(this, [this]() { update(); });
    }

    bool AxisView::reverse() const {
        return m_reverse;
    }

    void AxisView::setReverse(bool newReverse) {
        if (m_reverse == newReverse)
            return;
        m_reverse = newReverse;
        emit reverseChanged();
        QMetaObject::invokeMethod(this, [this]() { update(); });
    }

    QPointF AxisView::axisRange() const {
        return m_axisRange;
    }

    void AxisView::setAxisRange(QPointF newAxisRange) {
        if (m_axisRange == newAxisRange)
            return;
        m_axisRange = newAxisRange;
        emit axisRangeChanged();
        QMetaObject::invokeMethod(this, [this]() { update(); });
    }

    void AxisView::drawHorizontalAxis(QPainter* painter) const {
        using Union::Common::KeepDecimals;
        using Union::Common::ValueMap;
        const auto tick_count = KeepDecimals<0>(width() / 100.0);
        painter->fillRect(QRect(0, 0, width(), height()), QBrush(0xafeeee));
        QFontMetricsF font_metrics = painter->fontMetrics();
        // 水平
        for (int i = 0; i <= tick_count * 10; i++) {
            qreal   x             = std::round(((width() - 1) / (tick_count * 10.0)) * i);
            qreal   y             = 0;
            QColor  color         = QColor(0x00cc00);
            double  display_value = 0;
            QString value         = {};
            if (AppSetting::Instance()->roundToInt()) {
                display_value = KeepDecimals<0>(ValueMap(x, {m_axisRange.x(), m_axisRange.y()}, {0, width() - 1}));
                value         = QString::number(display_value, 'f', 0);
            } else {
                display_value = KeepDecimals<1>(ValueMap(x, {m_axisRange.x(), m_axisRange.y()}, {0, width() - 1}));
                value         = QString::number(display_value, 'f', 1);
            }

            QLineF line;
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
                if (i != (tick_count * 10)) {
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

    void AxisView::drawVerticalAxis(QPainter* painter) const {
        using Union::Common::KeepDecimals;
        using Union::Common::ValueMap;
        const auto tick_count = KeepDecimals<0>(height() / 100.0);
        painter->fillRect(QRect(0, 0, width(), height()), QBrush(0xafeeee));
        QFontMetricsF font_metrics = painter->fontMetrics();
        // 垂直
        for (int i = 0; i <= tick_count * 10; i++) {
            qreal  x             = width() - 1;
            qreal  y             = 0;
            double display_value = 0;
            if (reverse()) {
                y             = std::round((height() - 1) / (tick_count * 10.0) * ((tick_count * 10.0) - i));
                display_value = KeepDecimals<1>(ValueMap(y, {m_axisRange.x(), m_axisRange.y()}, {0, height() - 1}));
            } else {
                display_value = KeepDecimals<1>(ValueMap(y, {m_axisRange.y(), m_axisRange.x()}, {0, height() - 1}));
            }

            QString value = QString::number(display_value, 'f', 1);
            if (AppSetting::Instance()->roundToInt()) {
                display_value = KeepDecimals<0>(display_value);
                value         = QString::number(display_value, 'f', 0);
            }

            QColor color = QColor(0x00cc00);
            QLineF line;
            painter->setPen(color);
            if (i % 10 == 0) {
                line = {x - 15, y, x, y};
                if (i == (tick_count * 10)) {
                    color = QColor(255, 0, 0);
                    painter->setPen(color);
                } else {
                    color = QColor(0, 0, 0);
                    painter->setPen(color);
                }
                painter->save();
                QRectF fm_rect = font_metrics.boundingRect(value);

                QRectF draw_rect = QRectF(QPointF(0, 0), QSizeF(fm_rect.width(), fm_rect.height()));

                if (!reverse() && i == (tick_count * 10)) {
                    draw_rect = QRectF(QPointF(-fm_rect.width() - 10, 0), QSizeF(fm_rect.width(), fm_rect.height()));
                } else if (reverse() && i == 0) {
                    draw_rect = QRectF(QPointF(-fm_rect.width() - 10, 0), QSizeF(fm_rect.width(), fm_rect.height()));
                }

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

    void AxisView::paint(QPainter* painter) {
        if (isVertical()) {
            drawVerticalAxis(painter);
        } else {
            drawHorizontalAxis(painter);
        }
    }

} // namespace Union::View
