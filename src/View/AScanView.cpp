#include "AScanView.hpp"
#include <QPainter>
#include <future>

namespace Union::View {
    Union::View::AScanView::AScanView() {
        m_enableGrid = true;
    }

    QColor AScanView::lineColor() const {
        return m_lineColor;
    }

    void AScanView::setLineColor(const QColor &newLineColor) {
        if (m_lineColor == newLineColor)
            return;
        m_lineColor = newLineColor;
        emit lineColorChanged();
        QMetaObject::invokeMethod(this, [this]() { this->update(); });
    }

    void AScanView::replace(const std::vector<uint8_t> &data) noexcept {
        m_lineData = std::make_shared<std::vector<uint8_t>>(data.begin(), data.end());
        QMetaObject::invokeMethod(this, [this]() { this->update(); });
    }

    void AScanView::hook(const std::shared_ptr<const std::vector<uint8_t>> &data) noexcept {
        m_lineData = data;
        QMetaObject::invokeMethod(this, [this]() { this->update(); });
    }

    void AScanView::paint(QPainter *painter) {
        // 绘制父类的界面(背景、坐标轴、网格)
        auto lines = std::async([this]() {
            return getLines();
        });
        BasicView::paint(painter);
        painter->setPen(m_lineColor);
        painter->drawLines(lines.get());
    }

    QVector<QPointF> AScanView::getLines() const {
        QVector<QPointF> ret;
        if (m_lineData == nullptr || m_lineData->size() < 2) {
            return ret;
        }
        auto MapDot = [this](const QPointF &pt) -> QPointF {
            QRect   drawAble = getDrawable();
            qreal   width    = drawAble.width();
            qreal   height   = drawAble.height();
            QPointF ret;
            ret.setX(drawAble.left() + (pt.x() / (m_lineData->size() - 1)) * (width));
            ret.setY(drawAble.bottom() - (pt.y() / 200.0 * height));
            return ret;
        };

        for (int i = 1; i < std::ssize(*m_lineData); i++) {
            ret.push_back(MapDot(QPointF(i - 1, (*m_lineData)[i - 1])));
            ret.push_back(MapDot(QPointF(i, (*m_lineData)[i])));
        }
        return ret;
    }
} // namespace Union::View
