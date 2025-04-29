#pragma once
#include <QObject>
#include <QQuickPaintedItem>

namespace Union::View {
    class BasicView : public QQuickPaintedItem {
        Q_OBJECT
        QML_ELEMENT
    public:
        [[nodiscard]] bool    enableAxis() const;
        void                  setEnableAxis(bool newEnableAxis);
        [[nodiscard]] bool    enableGrid() const;
        void                  setEnableGrid(bool newEnableGrid);
        [[nodiscard]] QPointF verticalAxisRange() const;
        void                  setVerticalAxisRange(QPointF newVerticalAxisRange);
        [[nodiscard]] QPointF horizontalAxisRange() const;
        void                  setHorizontalAxisRange(QPointF newHorizontalAxisRange);
        [[nodiscard]] QColor  axisBackgroundColor() const;
        void                  setAxisBackgroundColor(const QColor &newAxisBackgroundColor);
        [[nodiscard]] int     verticalTickCount() const;
        void                  setVerticalTickCount(int newVerticalTickCount);
        [[nodiscard]] int     horizontalTickCount() const;
        void                  setHorizontalTickCount(int newHorizontalTickCount);
        [[nodiscard]] QColor  gridColor() const;
        void                  setgridColor(const QColor &newgridColor);
        [[nodiscard]] bool    reverseVerticalAxis() const;
        void                  setReverseVerticalAxis(bool newReverseVerticalAxis);

    signals:
        void enableAxisChanged();
        void enableGridChanged();
        void verticalAxisRangeChanged();
        void horizontalAxisRangeChanged();
        void axisBackgroundColorChanged();
        void verticalTickCountChanged();
        void horizontalTickCountChanged();
        void gridColorChanged();
        void reverseVerticalAxisChanged();

    private:
        void drawHorizontalAxis(QPainter *painter) const;
        void drawVerticalAxis(QPainter *painter) const;

        Q_PROPERTY(bool enableAxis READ enableAxis WRITE setEnableAxis NOTIFY enableAxisChanged FINAL)
        Q_PROPERTY(bool enableGrid READ enableGrid WRITE setEnableGrid NOTIFY enableGridChanged FINAL)
        Q_PROPERTY(QPointF verticalAxisRange READ verticalAxisRange WRITE setVerticalAxisRange NOTIFY verticalAxisRangeChanged FINAL)
        Q_PROPERTY(QPointF horizontalAxisRange READ horizontalAxisRange WRITE setHorizontalAxisRange NOTIFY horizontalAxisRangeChanged FINAL)
        Q_PROPERTY(QColor axisBackgroundColor READ axisBackgroundColor WRITE setAxisBackgroundColor NOTIFY axisBackgroundColorChanged FINAL)
        Q_PROPERTY(int verticalTickCount READ verticalTickCount WRITE setVerticalTickCount NOTIFY verticalTickCountChanged FINAL)
        Q_PROPERTY(int horizontalTickCount READ horizontalTickCount WRITE setHorizontalTickCount NOTIFY horizontalTickCountChanged FINAL)
        Q_PROPERTY(QColor gridColor READ gridColor WRITE setgridColor NOTIFY gridColorChanged FINAL)
        Q_PROPERTY(bool reverseVerticalAxis READ reverseVerticalAxis WRITE setReverseVerticalAxis NOTIFY reverseVerticalAxisChanged FINAL)

    protected:
        inline static constexpr auto AXIS_WIDTH = 35;

        bool    m_enableAxis          = true;            ///< 启用坐标轴
        bool    m_enableGrid          = false;           ///< 启用网格
        QPointF m_verticalAxisRange   = {0, 100.0};      ///< 垂直轴坐标
        QPointF m_horizontalAxisRange = {0, 100.0};      ///< 水平轴坐标
        QColor  m_axisBackgroundColor = {175, 238, 238}; ///< 坐标轴背景色
        int     m_verticalTickCount   = 5;               ///< 垂直方向划分个数
        int     m_horizontalTickCount = 5;               ///< 水平方向划分个数
        QColor  m_gridColor           = {255, 255, 0};   ///< 网格线颜色
        bool    m_reverseVerticalAxis = false;           ///< 翻转垂直坐标轴方向

        virtual void paint(QPainter *painter) override;

        [[nodiscard]] QRect getDrawable() const noexcept;
        void  drawAxis(QPainter *painter) const noexcept;
        void  drawGrid(QPainter *painter) const noexcept;
    };
} // namespace Union::View
