#include "RailWeldDigram.hpp"
#include <QBrush>
#include <QPainter>

namespace Union::AScan::RailWeld {
    bool RailWeldDigram::zeroPointInFoot() const {
        return m_zeroPointInFoot;
    }

    void RailWeldDigram::setZeroPointInFoot(bool newZeroPointInFoot) {
        if (m_zeroPointInFoot == newZeroPointInFoot)
            return;
        m_zeroPointInFoot = newZeroPointInFoot;
        emit zeroPointInFootChanged();
    }

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
        auto scale_factor = std::min(width() / 160, height() / 180);
        painter->scale(scale_factor, scale_factor);
        painter->translate(QPointF{155, 180});
        painter->rotate(180);

        constexpr auto _AA = 16.0;
        constexpr auto _BB = 150.0;
        constexpr auto _CC = 40.0;
        constexpr auto _DD = 176.0;
        constexpr auto _EE = 40.0;
        constexpr auto _FF = 35.0;
        constexpr auto _GG = 74.0;
        constexpr auto _HH = 20.0;

        constexpr QPointF A = {0, 0};
        constexpr QPointF B = {_BB, 0};
        constexpr QPointF C = {_BB, _AA};
        constexpr QPointF D = {_BB / 2.0 + _HH / 2.0, _AA + (_CC - _AA) * ((_BB / 2.0 - _HH / 2.0) / (_BB / 2.0))};
        constexpr QPointF E = {_BB / 2.0, _CC};
        constexpr QPointF F = {_BB / 2.0 - _HH / 2.0, _AA + (_CC - _AA) * ((_BB / 2.0 - _HH / 2.0) / (_BB / 2.0))};
        constexpr QPointF G = {0, _AA};
        constexpr QPointF H = {_BB / 2.0 - _HH / 2.0, _DD - _FF - ((_EE - _FF) / (_GG / 2.0) * (_GG / 2.0 - _HH / 2.0))};
        constexpr QPointF I = {_BB / 2, _DD - _EE};
        constexpr QPointF J = {_BB / 2.0 + _HH / 2.0, _DD - _FF - ((_EE - _FF) / (_GG / 2.0) * (_GG / 2.0 - _HH / 2.0))};
        constexpr QPointF K = {_BB / 2.0 + _GG / 2.0, _DD - _FF};
        constexpr QPointF L = {_BB / 2.0 + _GG / 2.0, _DD};
        constexpr QPointF M = {_BB / 2.0 - _GG / 2.0, _DD};
        constexpr QPointF N = {_BB / 2.0 - _GG / 2.0, _DD - _FF};

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
        auto zeroPoint = QPointF(_BB / 2.0, _DD);
        if (zeroPointInFoot()) {
            zeroPoint = QPointF(_BB / 2.0, 20.0);
        }
        painter->drawEllipse(zeroPoint + QPointF(-dotX(), dotY()), 1, 1);
        painter->restore();
    }

    void RailWeldDigram::paintDigrameB(QPainter* painter) const {
        painter->setPen(Qt::black);
        painter->save();
        painter->setBrush(Qt::NoBrush);
        auto scale_factorA = std::min(width() / 160, height() / 180);
        painter->scale(scale_factorA, scale_factorA);
        painter->translate(QPointF{0, 180});
        painter->scale(1.0 / scale_factorA, 1.0 / scale_factorA);
        auto scale_factor = std::min(width() / 260, height() / 180);
        painter->scale(scale_factor, scale_factor);
        painter->translate(QPointF{8, 50});

        const static QVector<QRectF> rects = {
            {0, 0,       240, 40},
            {0, 40,      240, 96},
            {0, 40 + 96, 240, 40},
        };
        painter->drawRects(rects);
        auto zeroPoint = QPointF(0, 0);
        if (zeroPointInFoot()) {
            zeroPoint = QPointF(0, 176 - 20);
        }
        painter->setBrush(Qt::red);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(zeroPoint + QPointF(dotZ(), -dotY()), 2, 2);
        painter->restore();
    }
} // namespace Union::AScan::RailWeld
