#pragma once

#include <AScanType.hpp>
#include <QQuickPaintedItem>
#include <QSemaphore>

namespace Union::AScan::RailWeld {

    class RailWeldSimulation : public QQuickPaintedItem {
        Q_OBJECT
        QML_ELEMENT

        int      m_simulationProbePosition  = 0;    ///< 仿真探头大致摆放位置
        int      m_simulationProbePoint     = 0;    ///< 仿真探头详细点位
        int      m_simulationProbeDirection = 0;    ///< 仿真探头摆放方向
        int      m_simulationRailType       = 0;    ///< 反正钢轨型号
        qreal    m_probeAngle               = 64.3; ///< 探头角度
        qreal    m_actualSoundDistance      = 100;  ///< 实际声程(S)
        QVariant m_ascanIntf                = {};   ///< A扫接口

    public:
        RailWeldSimulation();

        int      simulationProbePosition() const;
        void     setSimulationProbePosition(int newSimulationProbePosition);
        int      simulationProbePoint() const;
        void     setSimulationProbePoint(int newSimulationProbePoint);
        int      simulationProbeDirection() const;
        void     setSimulationProbeDirection(int newSimulationProbeDirection);
        int      simulationRailType() const;
        void     setSimulationRailType(int newSimulationRailType);
        qreal    probeAngle() const;
        void     setProbeAngle(qreal newProbeAngle);
        qreal    actualSoundDistance() const;
        void     setActualSoundDistance(qreal newActualSoundDistance);
        QVariant ascanIntf() const;
        void     setAscanIntf(const QVariant& newAscanIntf);

        Q_INVOKABLE void cursorChanged(int idx);

    signals:

        void simulationProbePositionChanged();
        void simulationProbePointChanged();
        void simulationProbeDirectionChanged();
        void simulationRailTypeChanged();
        void probeAngleChanged();
        void actualSoundDistanceChanged();
        void ascanIntfChanged();

    protected:
        void paint(QPainter* painter) override;

    private:
        QVector<QPointF> m_pointsRefine  = {};
        QVector<double>  m_thicknessData = {};
        QString          m_channelName   = {};

        QVector<QPointF> generateRefinePoints(const QVector<QPointF>& points, float maxSegmentLength); ///< 生成优化过的钢轨图像二维线段点
        void             drawRailImage(QPainter* painter) const noexcept;                              ///< 绘制钢轨2维示意图
        void             drawProbePosition(QPainter* painter, int point) const noexcept;               ///< 绘制探头位置
        void             drawVariableThicknessLine(QPainter* painter) const noexcept;                  ///< 绘制仿真图像

        bool findIntersection(const QPointF& p1, const QPointF& p2, const QPointF& p3, const QPointF& p4, QPointF& intersectionPoint) const;
        void drawVariableThicknessLine(QPainter* painter, const QVector<QPointF>& points);

        QVector<QPointF> calculateLinePoints() const noexcept;

        Q_PROPERTY(int simulationProbePosition READ simulationProbePosition WRITE setSimulationProbePosition NOTIFY simulationProbePositionChanged FINAL)
        Q_PROPERTY(int simulationProbePoint READ simulationProbePoint WRITE setSimulationProbePoint NOTIFY simulationProbePointChanged FINAL)
        Q_PROPERTY(int simulationProbeDirection READ simulationProbeDirection WRITE setSimulationProbeDirection NOTIFY simulationProbeDirectionChanged FINAL)
        Q_PROPERTY(int simulationRailType READ simulationRailType WRITE setSimulationRailType NOTIFY simulationRailTypeChanged FINAL)
        Q_PROPERTY(qreal probeAngle READ probeAngle WRITE setProbeAngle NOTIFY probeAngleChanged FINAL)
        Q_PROPERTY(qreal actualSoundDistance READ actualSoundDistance WRITE setActualSoundDistance NOTIFY actualSoundDistanceChanged FINAL)
        Q_PROPERTY(QVariant ascanIntf READ ascanIntf WRITE setAscanIntf NOTIFY ascanIntfChanged FINAL)
    };
} // namespace Union::AScan::RailWeld
