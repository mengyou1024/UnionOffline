#include "TofdPeDScanView.hpp"
#include <QLoggingCategory>
#include <QQmlProperty>
#include <QValueAxis>
#include <QtCharts>
#include <QtCore>
#include <Yo/File>
#include <span>
#include <vector>

namespace TOFD_PE {
    TofdPeInteractor* TofdPeDScanView::intr() const {
        return m_intr;
    }

    void TofdPeDScanView::setIntr(TofdPeInteractor* newIntr) {
        if (m_intr == newIntr)
            return;
        m_intr = newIntr;
        emit intrChanged();
    }

    bool TofdPeDScanView::isPe() const {
        return m_isPe;
    }

    void TofdPeDScanView::setIsPe(bool newIsPe) {
        if (m_isPe == newIsPe)
            return;
        m_isPe = newIsPe;
        emit isPeChanged();
    }

    int TofdPeDScanView::softGain() const {
        return m_softGain;
    }

    void TofdPeDScanView::setSoftGain(int newSoftGain) {
        if (m_softGain == newSoftGain)
            return;
        m_softGain = newSoftGain;
        emit softGainChanged();
        update();
        updateAScan();
    }

    int TofdPeDScanView::cursor() const {
        return m_cursor;
    }

    void TofdPeDScanView::setCursor(int newCursor) {
        if (!intr() || newCursor < 0 || newCursor >= intr()->getMaxLines()) {
            return;
        }

        if (m_cursor == newCursor)
            return;
        m_cursor = newCursor;
        emit cursorChanged();
        updateAScan();
    }

    QPointF TofdPeDScanView::abscissaRange() const {
        return m_abscissaRange;
    }

    void TofdPeDScanView::setAbscissaRange(QPointF newAbscissaRange) {
        if (m_abscissaRange == newAbscissaRange)
            return;
        m_abscissaRange = newAbscissaRange;
        emit abscissaRangeChanged();
    }

    QPointF TofdPeDScanView::ordinateRange() const {
        return m_ordinateRange;
    }

    void TofdPeDScanView::setOrdinateRange(QPointF newOrdinateRange) {
        if (m_ordinateRange == newOrdinateRange)
            return;
        m_ordinateRange = newOrdinateRange;
        emit ordinateRangeChanged();
    }

    TofdPeAScanView* TofdPeDScanView::aScanView() const {
        return m_aScanView;
    }

    void TofdPeDScanView::setAScanView(TofdPeAScanView* newAScanView) {
        if (m_aScanView == newAScanView)
            return;
        m_aScanView = newAScanView;
        emit aScanViewChanged();
    }

    TofdPeDScanView::TofdPeDScanView() {}

    TofdPeDScanView::~TofdPeDScanView() {}

    void TofdPeDScanView::paint(QPainter* painter) {
        painter->fillRect(QRect(0, 0, width(), height()), QBrush(Qt::black));
        if (isPe()) {
            drawAxis(painter);
            drawSecondDScan(painter);
        } else {
            drawTofdVerticalAxis(painter);
            drawDScan(painter);
        }
    }

    QRect TofdPeDScanView::getDrawable() const {
        return QRect(AXIS_WIDTH, 0, width() - AXIS_WIDTH, height() - AXIS_WIDTH);
    }

    void TofdPeDScanView::drawAxis(QPainter* painter) const {
        drawHorizontalAxis(painter);
        drawVerticalAxis(painter);
    }

    void TofdPeDScanView::drawTofdVerticalAxis(QPainter* painter) const {
        drawHorizontalAxis(painter);
        if (intr() == nullptr) {
            drawVerticalAxis(painter);
        }
        // 1. 反向推导出零点位置
        const auto zp_h     = intr()->getVerticalAxisZeroPoint() * getDrawable().height();
        const auto drawAble = getDrawable();

        // 垂直
        if (zp_h < 0 || zp_h > drawAble.height()) {
            // 如果零点超过坐标轴区域, 则正常绘制
            drawVerticalAxis(painter, [this](double v) { return intr()->getTofdDepth(v); });
        } else {
            painter->fillRect(QRect(0, 0, AXIS_WIDTH, height()), QBrush(0xafeeee));
            painter->setPen(QColor(255, 0, 0));
            painter->drawLine(QPointF(drawAble.left() - 16, zp_h), QPointF(drawAble.left() - 1, zp_h));

            auto DrawText = [=](QString s, QPointF pt) {
                QFontMetricsF fm       = painter->fontMetrics();
                QRectF        fmRect   = fm.boundingRect(s);
                QRectF        drawRect = QRectF(QPointF(0, 0), QSizeF(fmRect.width(), fmRect.height()));
                painter->save();
                painter->translate(pt.x() + 5, pt.y() + 5);
                painter->rotate(90);
                painter->drawText(drawRect, Qt::AlignCenter, s);
                painter->restore();
            };
            DrawText("0mm", QPointF(drawAble.left() - 1 - 15, zp_h));

            qreal step = std::round((drawAble.height() - 1) / 50.0);

            const auto GET_DRAW_PARAM = [](int idx) -> std::tuple<int, QColor, bool> {
                idx %= 10;
                if (idx == 0) {
                    return std::make_tuple(AXIS_WIDTH - 16, Qt::black, true);
                } else if (idx == 5) {
                    return std::make_tuple(AXIS_WIDTH - 11, 0x00cc00, false);
                }
                return std::make_tuple(AXIS_WIDTH - 6, 0x00cc00, false);
            };

            using _My_t = std::tuple<int, QColor, bool>;

            const auto DRAW_AXIS = [=, this](const _My_t param, double y) {
                const auto& [start, color, need_draw_text] = param;
                painter->setPen(color);
                painter->drawLine(QPointF(start, y), QPointF(drawAble.left() - 1, y));
                if (need_draw_text) {
                    auto val = static_cast<double>(y) / static_cast<double>(drawAble.height());
                    auto str = QString::number(intr()->getTofdDepth(val));
                    DrawText(str, QPointF(start, y));
                }
            };

            int draw_times = 0;
            // 向上画
            while ((++draw_times * step) - zp_h < 0) {
                auto y = zp_h - (draw_times * step);
                DRAW_AXIS(GET_DRAW_PARAM(draw_times), y);
            }
            draw_times = 0;
            // 向下画
            while ((++draw_times * step) + zp_h < drawAble.height()) {
                auto y = zp_h + (draw_times * step);
                DRAW_AXIS(GET_DRAW_PARAM(draw_times), y);
            }
        }
    }

    void TofdPeDScanView::drawDScan(QPainter* painter) const {
        if (intr() == nullptr) {
            return;
        }
        const auto maxLines = intr()->getMaxLines();
        QImage     dScan(maxLines, intr()->getAScanSize(), QImage::Format_RGB32);
        for (uint32_t x = 0; std::cmp_less(x, maxLines); x++) {
            for (uint32_t y = 0; std::cmp_less(y, intr()->getAScanSize()); y++) {
                auto index_x = static_cast<int>(x) + static_cast<int>(intr()->tofdSpace());
                if (std::cmp_less(index_x, intr()->getLines()) && std::cmp_greater_equal(index_x, 0)) {
                    auto bias    = static_cast<int>(intr()->getTofdData()[index_x * intr()->getAScanSize() + y]) - 128;
                    auto newBias = Union::CalculateGainOutput((double)bias, softGain());
                    if (newBias > 72) {
                        newBias = 72;
                    } else if (newBias < -128) {
                        newBias = -128;
                    }
                    auto     gray  = static_cast<uint8_t>(newBias + 128);
                    uint32_t color = gray | gray << 8 | gray << 16;
                    dScan.setPixel(QPoint(x, y), color);
                } else {
                    dScan.setPixel(QPoint(x, y), {});
                }
            }
        }
        painter->drawImage(getDrawable(), dScan);
    }

    void TofdPeDScanView::drawSecondDScan(QPainter* painter) const {
        if (intr() == nullptr) {
            return;
        }
        const auto maxLines = intr()->getMaxLines();
        QImage     dScan(maxLines, intr()->getAScanSize(), QImage::Format_RGB32);
        for (uint32_t x = 0; std::cmp_less(x, maxLines); x++) {
            for (uint32_t y = 0; std::cmp_less(y, intr()->getAScanSize()); y++) {
                auto index_x = static_cast<int>(x) + static_cast<int>(intr()->peSpace());
                if (std::cmp_less(index_x, intr()->getSubLines()) && std::cmp_greater_equal(index_x, 0)) {
                    auto    _t   = Union::CalculateGainOutput((double)intr()->getPeData()[index_x * intr()->getAScanSize() + y], softGain());
                    uint8_t gray = 0;
                    if (_t > 255.0) {
                        gray = 255;
                    } else if (_t < 0.0) {
                        gray = 0;
                    } else {
                        gray = static_cast<uint8_t>(_t);
                    }
                    uint32_t color = gray | gray << 8 | gray << 16;
                    dScan.setPixel(QPoint(x, y), color);
                } else {
                    dScan.setPixel(QPoint(x, y), {});
                }
            }
        }
        painter->drawImage(getDrawable(), dScan);
    }

    void TofdPeDScanView::drawHorizontalAxis(QPainter* painter) const {
        const auto drawAble = getDrawable();
        painter->fillRect(QRect(AXIS_WIDTH, drawAble.bottom() + 1, drawAble.width(), AXIS_WIDTH), QBrush(0xafeeee));
        // 水平
        for (int i = 0; i <= 50; i++) {
            QLineF        li;
            qreal         x  = std::round(drawAble.x() + ((drawAble.width() - 1) / 50.0) * i);
            qreal         y  = drawAble.bottom() + 1;
            QColor        c  = QColor(0x00cc00);
            QString       s  = QString::number(m_abscissaRange.x() + m_abscissaRange.y() / 50.0 * i, 'f', 0);
            QFontMetricsF fm = painter->fontMetrics();
            painter->setPen(c);
            if (i % 10 == 0) {
                li = {x, y, x, y + 15};
                if (i == 0) {
                    c = QColor(255, 0, 0);
                    painter->setPen(c);
                } else {
                    c = QColor(0, 0, 0);
                    painter->setPen(c);
                }
                QRectF fmRect   = fm.boundingRect(s);
                QRectF drawRect = {};
                if (i != 50) {
                    drawRect = QRectF(QPointF(li.p2().x() + 5, li.p2().y() - 5), QSizeF(fmRect.width(), fmRect.height()));
                } else {
                    drawRect = QRectF(QPointF(li.p2().x() - fmRect.width() - 5, li.p2().y() - 5), QSizeF(fmRect.width(), fmRect.height()));
                }
                painter->drawText(drawRect, Qt::AlignCenter, s);
            } else if (i % 5 == 0) {
                li = {x, y, x, y + 10};
            } else {
                li = {x, y, x, y + 5};
            }
            painter->drawLine(li);
        }
    }

    void TofdPeDScanView::drawVerticalAxis(QPainter* painter, std::optional<std::function<double(double)>> axisFunc) const {
        const auto drawAble = getDrawable();
        painter->fillRect(QRect(0, 0, AXIS_WIDTH, height()), QBrush(0xafeeee));
        // 垂直
        for (int i = 0; i <= 50; i++) {
            QLineF  li;
            qreal   x = drawAble.left() - 1;
            qreal   y = std::round((drawAble.height() - 1) / 50.0 * (50 - i));
            QColor  c = QColor(0x00cc00);
            QString s = QString::number(m_ordinateRange.x() + m_ordinateRange.y() / 50.0 * (50 - i), 'f', 1);
            if (axisFunc.has_value()) {
                s = QString::number(axisFunc.value()((50.0 - i) / 50.0), 'f', 1);
            }
            QFontMetricsF fm = painter->fontMetrics();
            painter->setPen(c);
            if (i % 10 == 0) {
                li = {x - 15, y, x, y};
                if (i == 50) {
                    c = QColor(255, 0, 0);
                    painter->setPen(c);
                } else {
                    c = QColor(0, 0, 0);
                    painter->setPen(c);
                }
                painter->save();
                QRectF fmRect   = fm.boundingRect(s);
                QRectF drawRect = QRectF(QPointF(0, 0), QSizeF(fmRect.width(), fmRect.height()));

                painter->translate(li.p1().x() + 5, li.p1().y() + 5);
                painter->rotate(90);
                painter->drawText(drawRect, Qt::AlignCenter, s);
                painter->restore();
            } else if (i % 5 == 0) {
                li = {x - 10, y, x, y};
            } else {
                li = {x - 5, y, x, y};
            }
            painter->drawLine(li);
        }
    }

    void TofdPeDScanView::updateAScan() const {
        auto GetData = [&]() -> std::optional<std::vector<uint8_t>> {
            if (intr() == nullptr) {
                return std::nullopt;
            }
            auto _cursor = m_cursor + intr()->tofdSpace();
            if (_cursor < 0 || _cursor >= intr()->getLines()) {
                return std::nullopt;
            }
            std::vector<uint8_t> data = {};
            const std::span      data_ptr(intr()->getTofdData().begin() + (intr()->getAScanSize() * _cursor), intr()->getAScanSize());
            for (auto i = 0; std::cmp_less(i, intr()->getAScanSize()); i++) {
                auto temp    = data_ptr[i];
                auto bias    = static_cast<int>(temp) - 128;
                auto newBias = Union::CalculateGainOutput((double)bias, softGain());
                if (newBias < -128) {
                    newBias = -128;
                }
                auto result = static_cast<uint8_t>(newBias + 128);
                data.emplace_back(result);
            }
            return data;
        };

        auto GetSecondData = [&]() -> std::optional<std::vector<uint8_t>> {
            if (intr() == nullptr) {
                return std::nullopt;
            }
            auto _cursor = m_cursor + intr()->peSpace();
            if (_cursor < 0 || _cursor >= intr()->getSubLines()) {
                return std::nullopt;
            }
            std::vector<uint8_t> data = {};
            const std::span      data_ptr(intr()->getPeData().begin() + (intr()->getAScanSize() * _cursor), intr()->getAScanSize());
            for (auto i = 0; std::cmp_less(i, intr()->getAScanSize()); i++) {
                auto  raw    = data_ptr[i];
                auto  _t     = Union::CalculateGainOutput((double)raw, softGain());
                qreal result = 0;
                if (_t > 255.0) {
                    result = 255.0;
                } else if (_t < 0.0) {
                    result = 0.0;
                } else {
                    result = _t;
                }
                data.push_back(result);
            }
            return data;
        };
        if (m_aScanView == nullptr) {
            return;
        }

        std::vector<uint8_t> data = {};
        if (!isPe()) {
            m_aScanView->replace(GetData().value_or(data));
        } else {
            m_aScanView->replace(GetSecondData().value_or(data));
        }
    }
} // namespace TOFD_PE
