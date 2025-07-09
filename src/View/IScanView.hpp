#pragma once

#include <QImage>
#include <QPixmap>
#include <QQuickPaintedItem>

namespace Union::View {
    using Range = std::pair<double, double>;

    class IScanView : public QQuickPaintedItem {
    public:
        IScanView();
        ~IScanView() override;

        enum CursorLocation {
            // 测量线
            LOCATION_NEAR_NONE                 = 0,
            LOCATION_NEAR_RED_VERTIVAL_LINE    = 1,
            LOCATION_NEAR_RED_HORIZONTAL_LINE  = 1 << 1,
            LOCATION_NEAR_BLUE_VERTIVAL_LINE   = 1 << 2,
            LOCATION_NEAR_BLUE_HORIZONTAL_LINE = 1 << 3,
            LOCATION_TOP_LEFT                  = LOCATION_NEAR_RED_VERTIVAL_LINE | LOCATION_NEAR_RED_HORIZONTAL_LINE,
            LOCATION_TOP_RIGHT                 = LOCATION_NEAR_RED_HORIZONTAL_LINE | LOCATION_NEAR_BLUE_VERTIVAL_LINE,
            LOCATION_BOTTOM_LEFT               = LOCATION_NEAR_RED_VERTIVAL_LINE | LOCATION_NEAR_BLUE_HORIZONTAL_LINE,
            LOCATION_BOTTOM_RIGHT              = LOCATION_NEAR_BLUE_VERTIVAL_LINE | LOCATION_NEAR_BLUE_HORIZONTAL_LINE,

            // 水平轴显示区域
            LOCATION_NEAR_HOR_AXIS_SHOW_LEFT  = 1 << 4,
            LOCATION_NEAR_HOR_AXIS_SHOW_RIGHT = 1 << 5,
            LOCATION_NEAR_HOR_AXIS_SHOW_RECT  = 1 << 6,

            // 垂直轴显示区域
            LOCATION_NEAR_VER_AXIS_SHOW_TOP    = 1 << 7,
            LOCATION_NEAR_VER_AXIS_SHOW_BOTTOM = 1 << 8,
            LOCATION_NEAR_VER_AXIS_SHOW_RECT   = 1 << 9,
        };

    private:
        Q_OBJECT
        QML_ELEMENT

        Range m_horRange     = {0, 100};
        Range m_horShowRange = {25, 75};
        Range m_verRange     = {0, 100};
        Range m_verShowRange = {25, 75};

        CursorLocation m_cursorLocation     = LOCATION_NEAR_NONE;
        bool           m_isMousePressed     = false;
        bool           m_isCtrlPressed      = false;
        QPoint         m_measuringPointRed  = {};
        QPoint         m_measuringPointBlue = {};

        qreal m_horRedValue  = {};
        qreal m_verRedValue  = {};
        qreal m_horBlueValue = {};
        qreal m_verBlueValue = {};

        QPoint m_imagePoint = {};

    public:
        void updateImage(QImage image);
        void resetHorShowRange();
        void resetVerShowRange();
        void resetShowRange();
        void resetMeasuringLine();

        Range          horRange() const;
        void           setHorRange(const Range& newHorRange);
        Range          horShowRange() const;
        void           setHorShowRange(const Range& newHorShowRange);
        Range          verRange() const;
        void           setVerRange(const Range& newVerRange);
        Range          verShowRange() const;
        void           setVerShowRange(const Range& newVerShowRange);
        void           setDisplayText(const QString& text);
        CursorLocation cursorLocation() const;
        void           setCursorLocation(CursorLocation newCursorLocation);
        bool           isMousePressed() const;
        void           setIsMousePressed(bool newIsMousePressed);
        bool           isCtrlPressed() const;
        void           setIsCtrlPressed(bool newIsCtrlPressed);
        QPoint         measuringPointRed() const;
        void           setMeasuringPointRed(QPoint newMeasuringPointRed);
        QPoint         measuringPointBlue() const;
        void           setMeasuringPointBlue(QPoint newMeasuringPointBlue);
        qreal          horRedValue() const;
        void           setHorRedValue(qreal newHorRedValue);
        qreal          verRedValue() const;
        void           setVerRedValue(qreal newVerRedValue);
        qreal          horBlueValue() const;
        void           setHorBlueValue(qreal newHorBlueValue);
        qreal          verBlueValue() const;
        void           setVerBlueValue(qreal newVerBlueValue);
        QPoint         imagePoint() const;
        void           setImagePoint(QPoint newImagePoint);

    signals:
        void horRangeChanged();
        void horShowRangeChanged();
        void verRangeChanged();
        void verShowRangeChanged();
        void cursorLocationChanged();
        void isMousePressedChanged();
        void isCtrlPressedChanged();
        void measuringPointRedChanged();
        void measuringPointBlueChanged();
        void horRedValueChanged();
        void verRedValueChanged();
        void horBlueValueChanged();
        void verBlueValueChanged();
        void imagePointChanged();

    protected:
        virtual void paint(QPainter* painter) override;

        virtual void mousePressEvent(QMouseEvent* event) override;
        virtual void mouseMoveEvent(QMouseEvent* event) override;
        virtual void mouseReleaseEvent(QMouseEvent* event) override;
        virtual void hoverEnterEvent(QHoverEvent* event) override;
        virtual void hoverMoveEvent(QHoverEvent* event) override;
        virtual void hoverLeaveEvent(QHoverEvent* event) override;
        virtual void keyPressEvent(QKeyEvent* event) override;
        virtual void keyReleaseEvent(QKeyEvent* event) override;
        virtual void wheelEvent(QWheelEvent* event) override;
        virtual void draw_image(const QImage& img, QPainter* painter);

    protected:
        inline static constexpr auto AXIS_WIDTH            = 40;
        inline static constexpr auto AXIS_MASK_COLOR       = QColor(0xCC, 0xCC, 0xCC, 200);
        inline static constexpr auto AXIS_BACKGROUND_COLOR = QColor(0xaf, 0xee, 0xee);
        inline static constexpr auto NEAR_THRESHOLD        = 5;

        std::optional<QPoint> pressed_point_ = std::nullopt;
        QString               display_text_  = "";
        std::optional<QImage> image_raw_     = std::nullopt;
        std::optional<QImage> image_visable_ = std::nullopt;
        std::optional<QRect>  box_selection_ = std::nullopt;

        QRect                drawable() const;
        QSize                drawable_size() const;
        std::optional<QSize> image_size() const;
        Range                drawable_hor_range() const;
        Range                drawable_ver_range() const;
        QRect                hor_axis_rect() const;
        QRect                ver_axis_rect() const;
        QRect                hor_show_rect() const;
        QRect                ver_show_rect() const;
        QRect                display_text_rect() const;

        qreal map_to_hor_value(qreal x) const;
        qreal map_to_ver_value(qreal y) const;
        qreal map_to_show_hor_value(qreal x) const;
        qreal map_to_show_ver_value(qreal y) const;
        qreal map_to_hor_value_display(qreal x) const;
        qreal map_to_ver_value_display(qreal y) const;
        qreal map_to_show_hor_value_display(qreal x) const;
        qreal map_to_show_ver_value_display(qreal y) const;

        qreal show_hor_x_left() const;
        qreal show_hor_x_right() const;
        qreal show_ver_y_top() const;
        qreal show_ver_y_bottom() const;

        std::optional<qreal> min_show_range_width() const;
        std::optional<qreal> min_show_range_height() const;

        void draw_hor_axis(QPainter* painter) const;
        void draw_ver_axis(QPainter* painter) const;
        void draw_display_text(QPainter* painter) const;
        void draw_measuring_line(QPainter* painter) const;
        void draw_measuring_text(QPainter* painter);
        void draw_box_selection(QPainter* painter) const;

        bool is_near(qreal val, qreal target) const;

        void change_location_and_cursor_hor(QHoverEvent* event);
        void change_location_and_cursor_ver(QHoverEvent* event);
        void change_location_and_cursor_measuring_line(QHoverEvent* event);
        void change_location_and_cursor_to_none();

        void move_hor_show_left(double x);
        void move_hor_show_right(double x);
        void move_hor_show_rect(double delta);
        void move_ver_show_top(double y);
        void move_ver_show_bottom(double y);
        void move_ver_show_rect(double delta);

        void copy_image_visable_from_raw();

        void update_image_point();

        std::optional<QPoint> measuring_point_to_image_point(QPoint pt);

        Q_PROPERTY(Range horRange READ horRange WRITE setHorRange NOTIFY horRangeChanged FINAL)
        Q_PROPERTY(Range horShowRange READ horShowRange WRITE setHorShowRange NOTIFY horShowRangeChanged FINAL)
        Q_PROPERTY(Range verRange READ verRange WRITE setVerRange NOTIFY verRangeChanged FINAL)
        Q_PROPERTY(Range verShowRange READ verShowRange WRITE setVerShowRange NOTIFY verShowRangeChanged FINAL)
        Q_PROPERTY(CursorLocation cursorLocation READ cursorLocation WRITE setCursorLocation NOTIFY cursorLocationChanged FINAL)
        Q_PROPERTY(bool isMousePressed READ isMousePressed WRITE setIsMousePressed NOTIFY isMousePressedChanged FINAL)
        Q_PROPERTY(bool isCtrlPressed READ isCtrlPressed WRITE setIsCtrlPressed NOTIFY isCtrlPressedChanged FINAL)
        Q_PROPERTY(QPoint measuringPointRed READ measuringPointRed WRITE setMeasuringPointRed NOTIFY measuringPointRedChanged FINAL)
        Q_PROPERTY(QPoint measuringPointBlue READ measuringPointBlue WRITE setMeasuringPointBlue NOTIFY measuringPointBlueChanged FINAL)
        Q_PROPERTY(qreal horRedValue READ horRedValue WRITE setHorRedValue NOTIFY horRedValueChanged FINAL)
        Q_PROPERTY(qreal verRedValue READ verRedValue WRITE setVerRedValue NOTIFY verRedValueChanged FINAL)
        Q_PROPERTY(qreal horBlueValue READ horBlueValue WRITE setHorBlueValue NOTIFY horBlueValueChanged FINAL)
        Q_PROPERTY(qreal verBlueValue READ verBlueValue WRITE setVerBlueValue NOTIFY verBlueValueChanged FINAL)
        Q_PROPERTY(QPoint imagePoint READ imagePoint WRITE setImagePoint NOTIFY imagePointChanged FINAL)
    };
} // namespace Union::View
