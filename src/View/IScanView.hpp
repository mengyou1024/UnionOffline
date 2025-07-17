#pragma once

#include <QImage>
#include <QPixmap>
#include <QQuickPaintedItem>

namespace Union::View {
    using Range = std::pair<double, double>;

    [[maybe_unused]] inline static bool contains(const Range& range, double val) {
        if (val >= std::min(range.first, range.second) && val <= std::max(range.first, range.second)) {
            return true;
        }
        return false;
    }

    [[maybe_unused]] inline static double length(const Range& range) {
        return std::abs(range.second - range.first);
    }

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

        std::optional<QImage> grabImage();

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
        void boxSelected(QRect rect);

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
        // 坐标轴宽度
        inline static constexpr auto AXIS_WIDTH = 40;
        // 坐标轴掩膜(滚动条)颜色
        inline static constexpr auto AXIS_MASK_COLOR = QColor(0xCC, 0xCC, 0xCC, 200);
        // 坐标轴背景色
        inline static constexpr auto AXIS_BACKGROUND_COLOR = QColor(0xaf, 0xee, 0xee);
        // is_near 函数的判断阈值
        inline static constexpr auto NEAR_THRESHOLD = 5;

        std::optional<QPoint> pressed_point_ = std::nullopt; // 鼠标按下的点
        QString               display_text_  = "";           // 左下角显示的文本
        std::optional<QImage> image_raw_     = std::nullopt; // 原始图像
        std::optional<QImage> image_visable_ = std::nullopt; // 可现区域的图像
        std::optional<QRect>  box_selection_ = std::nullopt; // 框选区域

        // 一些区域划分定义
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

        // 将屏幕上的像素点位置值映射到对应坐标的坐标值, display表示该值仅用于显示(保留1或0位小数)
        qreal map_to_hor_value(qreal x) const;
        qreal map_to_ver_value(qreal y) const;
        qreal map_to_show_hor_value(qreal x) const;
        qreal map_to_show_ver_value(qreal y) const;
        qreal map_to_hor_value_display(qreal x) const;
        qreal map_to_ver_value_display(qreal y) const;
        qreal map_to_show_hor_value_display(qreal x) const;
        qreal map_to_show_ver_value_display(qreal y) const;

        // 水平坐标轴和垂直坐标轴在控件中的边界(屏幕像素点位置值)
        qreal show_hor_x_left() const;
        qreal show_hor_x_right() const;
        qreal show_ver_y_top() const;
        qreal show_ver_y_bottom() const;

        // 可显坐标轴的最小宽度
        // 当图像上的每一个像素点与屏幕上的像素点一一对应时
        // 可显坐标轴范围最大值 - 最小值
        std::optional<qreal> min_show_range_width() const;
        std::optional<qreal> min_show_range_height() const;

        // 图像绘制相关
        void draw_hor_axis(QPainter* painter) const;       // 绘制水平坐标轴
        void draw_ver_axis(QPainter* painter) const;       // 绘制垂直坐标轴
        void draw_display_text(QPainter* painter) const;   // 绘制左下角显示的字符
        void draw_measuring_line(QPainter* painter) const; // 绘制测量线
        void draw_measuring_text(QPainter* painter);       // 绘制测量线中的文本(同时更新对应的属性)
        void draw_box_selection(QPainter* painter) const;  // 绘制框选区域

        // 判断一个值是否与目标值靠近 由 NEAR_THRESHOLD 决定阈值
        bool is_near(qreal val, qreal target) const;

        // 改变 cursorLocation 变量的状态 和 鼠标形状
        void change_location_and_cursor_hor(QHoverEvent* event);            // 水平坐标轴相关
        void change_location_and_cursor_ver(QHoverEvent* event);            // 垂直坐标轴相关
        void change_location_and_cursor_measuring_line(QHoverEvent* event); // 测量线相关
        void change_location_and_cursor_to_none();                          // 其他未定义区域

        // 移动坐标轴可显区域范围
        void move_hor_show_left(double x);     // 改变水平坐标可显范围的起始值(缩放)
        void move_hor_show_right(double x);    // 改变水平坐标可显范围的终止值(缩放)
        void move_hor_show_rect(double delta); // 移动水平坐标轴滑块一个变化量(平移)
        void move_ver_show_top(double y);
        void move_ver_show_bottom(double y);
        void move_ver_show_rect(double delta);

        // 从原始图像中拷贝可见区域到 image_visible_ 变量
        void copy_image_visable_from_raw();

        // 更新imagePoint属性
        void update_image_point();

        // 更新缺陷掩膜图像的大小
        void update_defect_mask_size();

        bool is_raw_image_width_can_contain() const;
        bool is_raw_image_height_can_contain() const;
        bool is_show_image_width_can_contain() const;
        bool is_show_image_height_can_contain() const;

        // 控件上的像素点位置转换为原始图像上点的位置
        std::optional<QPoint> local_pos_to_raw_image_pos(QPoint pt) const;
        // 原始图像上像素点的位置转换为屏幕上像素点的位置
        std::optional<QPoint> raw_image_pos_to_local_pos(QPoint pt) const;

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
