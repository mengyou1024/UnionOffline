#include "RailWeldDigram.h"
#include <QBrush>
#include <QPainter>
namespace Union::AScan::RailWeld {
    RailWeldDigram::RailWeldDigram() {}

    int RailWeldDigram::dotX() const {
        return m_dotX;
    }

    void RailWeldDigram::setDotX(int newDotX) {
        if (m_dotX == newDotX)
            return;
        m_dotX = newDotX;
        update();
        emit dotXChanged();
    }

    int RailWeldDigram::dotY() const {
        return m_dotY;
    }

    void RailWeldDigram::setDotY(int newDotY) {
        if (m_dotY == newDotY)
            return;
        m_dotY = newDotY;
        update();
        emit dotYChanged();
    }

    int RailWeldDigram::dotZ() const {
        return m_dotZ;
    }

    void RailWeldDigram::setDotZ(int newDotZ) {
        if (m_dotZ == newDotZ)
            return;
        m_dotZ = newDotZ;
        update();
        emit dotZChanged();
    }

    void RailWeldDigram::paint(QPainter* painter) {
        painter->setRenderHint(QPainter::Antialiasing);
        paintDigrameA(painter);
        paintDigrameB(painter);
    }

    void RailWeldDigram::paintDigrameA(QPainter* painter) const {
        painter->setPen(Qt::black);
        painter->save();
        auto scale_factor = std::min(width() / 120, height() / 150);
        painter->scale(scale_factor, scale_factor);
        painter->translate(QPointF{117.5, 143.48});
        painter->rotate(180);

        constexpr QPointF A = {0, 0};
        constexpr QPointF B = {114, 0};
        constexpr QPointF C = {114, 11};
        constexpr QPointF D = {67, 24.19};
        constexpr QPointF E = {57, 27};
        constexpr QPointF F = {47, 24.19};
        constexpr QPointF G = {0, 11};
        constexpr QPointF H = {47, 96.19};
        constexpr QPointF I = {57, 91.90};
        constexpr QPointF J = {67, 96.19};
        constexpr QPointF K = {92, 106.90};
        constexpr QPointF L = {92, 136.90};
        constexpr QPointF M = {22, 136.90};
        constexpr QPointF N = {22, 106.90};

        const static QVector<QLineF> lines = {
            {A, B},
            {B, C},
            {C, E},
            {E, G},
            {G, A},
            {F, H},
            {D, J},
            {I, K},
            {K, L},
            {L, M},
            {M, N},
            {N, I},
        };
        painter->drawLines(lines);
        painter->setBrush(Qt::red);
        painter->setPen(Qt::NoPen);
        constexpr auto zeroPoint = I + QPointF(0, 15);
        painter->drawEllipse(zeroPoint + QPointF(-dotX(), dotY()), 1, 1);
        painter->restore();
    }

    void RailWeldDigram::paintDigrameB(QPainter* painter) const {
        painter->setPen(Qt::black);
        painter->save();
        painter->setBrush(Qt::NoBrush);
        auto scale_factorA = std::min(width() / 120, height() / 150);
        painter->scale(scale_factorA, scale_factorA);
        painter->translate(QPointF{0, 150});
        painter->scale(1.0 / scale_factorA, 1.0 / scale_factorA);
        auto scale_factor = std::min(width() / 260, height() / 140);
        painter->scale(scale_factor, scale_factor);
        painter->translate(QPointF{10, 10});

        const static QVector<QRectF> rects = {
            {0, 0,   240, 30},
            {0, 30,  240, 65},
            {0, 95,  240, 5 },
            {0, 100, 240, 20},
        };
        painter->drawRects(rects);
        constexpr auto zeroPoint = QPointF(2, 28);
        painter->setBrush(Qt::red);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(zeroPoint + QPointF(dotZ(), -dotY()), 2, 2);
        painter->restore();
    }
} // namespace Union::AScan::RailWeld
