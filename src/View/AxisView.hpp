#pragma once
#include <QObject>
#include <QQuickPaintedItem>

namespace Union::View {
    class AxisView : public QQuickPaintedItem {
        Q_OBJECT
        QML_ELEMENT

        bool    m_isVertical = false;      ///< 是否时垂直坐标轴
        bool    m_reverse    = false;      ///< 坐标轴翻转
        QPointF m_axisRange  = {0, 100.0}; ///< 轴坐标
        int     m_tickCount  = 5;          ///< 划分个数

    public:
        bool    isVertical() const;
        void    setIsVertical(bool newIsVertical);
        bool    reverse() const;
        void    setReverse(bool newReverse);
        QPointF axisRange() const;
        void    setAxisRange(QPointF newAxisRange);
        int     tickCount() const;
        void    setTickCount(int newTickCount);

    signals:
        void isVerticalChanged();
        void reverseChanged();
        void axisRangeChanged();
        void tickCountChanged();

    private:
        void drawHorizontalAxis(QPainter* painter) const;
        void drawVerticalAxis(QPainter* painter) const;

        Q_PROPERTY(bool isVertical READ isVertical WRITE setIsVertical NOTIFY isVerticalChanged FINAL)
        Q_PROPERTY(bool reverse READ reverse WRITE setReverse NOTIFY reverseChanged FINAL)
        Q_PROPERTY(QPointF axisRange READ axisRange WRITE setAxisRange NOTIFY axisRangeChanged FINAL)
        Q_PROPERTY(int tickCount READ tickCount WRITE setTickCount NOTIFY tickCountChanged FINAL)

    protected:
        virtual void paint(QPainter* painter) override;
    };
} // namespace Union::View
