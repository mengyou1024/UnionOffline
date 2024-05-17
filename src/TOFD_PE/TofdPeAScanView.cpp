#include "TofdPeAScanView.hpp"
#include <QLoggingCategory>
#include <QQmlProperty>
#include <QValueAxis>
#include <QtCharts>
#include <QtCore>
#include <Yo/File>

namespace TOFD_PE {

    void TofdPeAScanView::replace(const std::vector<uint8_t>& v) {
        m_data = v;
        update();
    }

    void TofdPeAScanView::paint(QPainter* painter) {
        painter->fillRect(QRect(0, 0, width(), height()), Qt::black);
        drawGridLines(painter);
        drawAScanLine(painter);
    }

    void TOFD_PE::TofdPeAScanView::drawGridLines(QPainter* painter) {
        const QRect drawAble(0, 0, width(), height() - 1);
        // 绘制坐标轴
        // 虚线的画笔
        QVector<qreal> dashes;
        qreal          space = 2;
        dashes << 4 << space << 4 << space;
        QPen pen;
        pen.setBrush(QBrush(0xaeaeae));
        pen.setDashPattern(dashes);
        QPointF grid[24] = {};
        // 横向
        qreal temp;
        for (int32_t i = 0; i < 12; i += 2) {
            temp        = qreal(drawAble.height()) / 5.0 * i / 2.0 + qreal(drawAble.y());
            grid[i]     = {qreal(drawAble.left()), temp};
            grid[i + 1] = {qreal(drawAble.right()), temp};
        }
        // 纵向
        for (int32_t i = 0; i < 12; i += 2) {
            temp             = qreal(drawAble.width() - 1) / 5.0 * i / 2.0 + qreal(drawAble.x());
            grid[12 + i]     = {temp, qreal(drawAble.top())};
            grid[12 + i + 1] = {temp, qreal(drawAble.bottom())};
        }
        painter->setPen(pen);
        painter->drawLines(grid, 12);
    }

    void TofdPeAScanView::drawAScanLine(QPainter* painter) {
        if (m_data.size() == 0) {
            return;
        }
        QVector<QLineF> lines = {};
        lines.resize(static_cast<int>(m_data.size()) - 1);
        for (auto i = 0; i < lines.size(); i++) {
            auto x1  = static_cast<double>(m_data[i]) / 255.0 * static_cast<double>(width());
            auto y1  = static_cast<double>(height()) / static_cast<double>(m_data.size()) * static_cast<double>(i);
            auto x2  = static_cast<double>(m_data[i + 1]) / 255.0 * static_cast<double>(width());
            auto y2  = static_cast<double>(height()) / static_cast<double>(m_data.size()) * static_cast<double>(i + 1);
            lines[i] = QLineF(QPointF(x1, y1), QPointF(x2, y2));
        }
        painter->setPen(QColor(Qt::yellow));
        painter->drawLines(lines);
    }

} // namespace TOFD_PE
