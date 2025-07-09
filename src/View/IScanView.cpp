#include "IScanView.hpp"
#include "AppSetting.hpp"
#include "ColorTable.hpp"
#include <QCursor>
#include <QLoggingCategory>
#include <QPainter>
#include <QPainterPath>
#include <QSettings>
#include <union_common.hpp>

using namespace ::Union::Common;

[[maybe_unused]] static Q_LOGGING_CATEGORY(TAG, "Union.View.IScanView");

namespace Union::View {

    Range IScanView::horShowRange() const {
        return m_horShowRange;
    }

    void IScanView::setHorShowRange(const Range& newHorShowRange) {
        if (m_horShowRange == newHorShowRange)
            return;
        m_horShowRange = newHorShowRange;
        emit horShowRangeChanged();
    }

    Range IScanView::verRange() const {
        return m_verRange;
    }

    void IScanView::setVerRange(const Range& newVerRange) {
        if (m_verRange == newVerRange)
            return;
        m_verRange = newVerRange;
        emit verRangeChanged();
    }

    Range IScanView::verShowRange() const {
        return m_verShowRange;
    }

    void IScanView::setVerShowRange(const Range& newVerShowRange) {
        if (m_verShowRange == newVerShowRange)
            return;
        m_verShowRange = newVerShowRange;
        emit verShowRangeChanged();
    }

    void IScanView::setDisplayText(const QString& text) {
        display_text_ = text;
    }

    IScanView::IScanView() {
        setFocus(true);
        setKeepMouseGrab(true);
        setOpaquePainting(true);
        setAcceptHoverEvents(true);
        setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MiddleButton);

        connect(this, &IScanView::horShowRangeChanged, this, [this] {
            copy_image_visable_from_raw();
            update_image_point();
            update();
        });

        connect(this, &IScanView::horRangeChanged, this, [this] {
            resetHorShowRange();
            update();
        });

        connect(this, &IScanView::verShowRangeChanged, this, [this] {
            copy_image_visable_from_raw();
            update_image_point();
            update();
        });

        connect(this, &IScanView::verRangeChanged, this, [this] {
            resetVerShowRange();
            update();
        });

        connect(this, &IScanView::widthChanged, this, [this] {
            resetMeasuringLine();
            update();
        });
        connect(this, &IScanView::heightChanged, this, [this] {
            resetMeasuringLine();
            update();
        });

        connect(this, &IScanView::cursorLocationChanged, this, [this] {
            update();
        });

        connect(this, &IScanView::measuringPointBlueChanged, this, [this] {
            update();
        });

        connect(this, &IScanView::measuringPointRedChanged, this, [this] {
            update_image_point();
            update();
        });
    }

    IScanView::~IScanView() {}

    QPoint IScanView::imagePoint() const {
        return m_imagePoint;
    }

    void IScanView::setImagePoint(QPoint newImagePoint) {
        if (m_imagePoint == newImagePoint)
            return;
        m_imagePoint = newImagePoint;
        emit imagePointChanged();
    }

    qreal IScanView::verBlueValue() const {
        return m_verBlueValue;
    }

    void IScanView::setVerBlueValue(qreal newVerBlueValue) {
        if (qFuzzyCompare(m_verBlueValue, newVerBlueValue))
            return;
        m_verBlueValue = newVerBlueValue;
        emit verBlueValueChanged();
    }

    qreal IScanView::horBlueValue() const {
        return m_horBlueValue;
    }

    void IScanView::setHorBlueValue(qreal newHorBlueValue) {
        if (qFuzzyCompare(m_horBlueValue, newHorBlueValue))
            return;
        m_horBlueValue = newHorBlueValue;
        emit horBlueValueChanged();
    }

    qreal IScanView::verRedValue() const {
        return m_verRedValue;
    }

    void IScanView::setVerRedValue(qreal newVerRedValue) {
        if (qFuzzyCompare(m_verRedValue, newVerRedValue))
            return;
        m_verRedValue = newVerRedValue;
        emit verRedValueChanged();
    }

    qreal IScanView::horRedValue() const {
        return m_horRedValue;
    }

    void IScanView::setHorRedValue(qreal newHorRedValue) {
        if (qFuzzyCompare(m_horRedValue, newHorRedValue))
            return;
        m_horRedValue = newHorRedValue;
        emit horRedValueChanged();
    }

    bool IScanView::isCtrlPressed() const {
        return m_isCtrlPressed;
    }

    void IScanView::setIsCtrlPressed(bool newIsCtrlPressed) {
        if (m_isCtrlPressed == newIsCtrlPressed)
            return;
        m_isCtrlPressed = newIsCtrlPressed;
        emit isCtrlPressedChanged();
    }

    bool IScanView::isMousePressed() const {
        return m_isMousePressed;
    }

    void IScanView::setIsMousePressed(bool newIsMousePressed) {
        if (m_isMousePressed == newIsMousePressed)
            return;
        m_isMousePressed = newIsMousePressed;
        emit isMousePressedChanged();
    }

    void IScanView::updateImage(QImage image) {
        image_raw_ = std::move(image);
        copy_image_visable_from_raw();
        update();
    }

    void IScanView::resetHorShowRange() {
        setHorShowRange(horRange());
    }

    void IScanView::resetVerShowRange() {
        setVerShowRange(verRange());
    }

    void IScanView::resetShowRange() {
        resetHorShowRange();
        resetVerShowRange();
    }

    void IScanView::resetMeasuringLine() {
        setMeasuringPointRed(QPoint(
            KeepDecimals<0>(ValueMap(0.2, drawable_hor_range())),
            KeepDecimals<0>(ValueMap(0.2, drawable_ver_range()))));

        setMeasuringPointBlue(QPoint(
            KeepDecimals<0>(ValueMap(0.4, drawable_hor_range())),
            KeepDecimals<0>(ValueMap(0.4, drawable_ver_range()))));
    }

    IScanView::CursorLocation IScanView::cursorLocation() const {
        return m_cursorLocation;
    }

    void IScanView::setCursorLocation(CursorLocation newCursorLocation) {
        if (m_cursorLocation == newCursorLocation)
            return;
        m_cursorLocation = newCursorLocation;
        emit cursorLocationChanged();
    }

    Range IScanView::horRange() const {
        return m_horRange;
    }

    void IScanView::setHorRange(const Range& newHorRange) {
        if (m_horRange == newHorRange)
            return;
        m_horRange = newHorRange;
        emit horRangeChanged();
    }

    void IScanView::paint(QPainter* painter) {
        painter->fillRect(drawable(), Qt::white);
        painter->save();
        draw_ver_axis(painter);
        painter->restore();

        painter->save();
        draw_hor_axis(painter);
        painter->restore();

        painter->save();
        draw_display_text(painter);
        painter->restore();

        if (image_visable_.has_value()) {
            painter->save();
            draw_image(*image_visable_, painter);
            painter->restore();
        }

        painter->save();
        draw_measuring_line(painter);
        painter->restore();

        painter->save();
        draw_measuring_text(painter);
        painter->restore();

        painter->save();
        draw_box_selection(painter);
        painter->restore();
    }

    void IScanView::mousePressEvent(QMouseEvent* event) {
        forceActiveFocus();
        setIsMousePressed(true);
        pressed_point_ = event->pos();

        if (isCtrlPressed() && event->button() == Qt::LeftButton) {
            setMeasuringPointRed(event->pos());
        } else if (isCtrlPressed() && event->button() == Qt::RightButton) {
            setMeasuringPointBlue(event->pos());
        }

        if (!isCtrlPressed() &&
            event->button() == Qt::LeftButton &&
            drawable().contains(event->pos()) &&
            cursorLocation() == LOCATION_NEAR_NONE) {
            box_selection_ = QRect(QPoint(event->pos()), QPoint(event->pos()));
        }

        if (event->button() == Qt::RightButton && drawable().contains(event->pos())) {
            setCursor(Qt::ClosedHandCursor);
        }
    }

    void IScanView::mouseMoveEvent(QMouseEvent* event) {
        Q_UNUSED(event)

        if (!isCtrlPressed() && event->buttons() == Qt::LeftButton) {
            if (box_selection_.has_value()) {
                box_selection_ = QRect(box_selection_->topLeft(), QPoint(event->pos()));
                update();
            }
        }

        if (event->buttons() == Qt::RightButton && image_visable_.has_value()) {
            // 鼠标移动的像素点
            const auto delta_x = pressed_point_->x() - event->x();
            const auto delta_y = pressed_point_->y() - event->y();
            // 显示图像水平变化的点数
            const auto delta_show_image_hor_dots = static_cast<double>(image_visable_->width()) * delta_x / drawable().width();
            const auto delta_show_image_ver_dots = static_cast<double>(image_visable_->height()) * delta_y / drawable().height();
            const auto delta_hor_range           = (horRange().second - horRange().first) / image_raw_->width() * delta_show_image_hor_dots;
            const auto delta_ver_range           = (verRange().second - verRange().first) / image_raw_->height() * delta_show_image_ver_dots;
            move_hor_show_rect(delta_hor_range);
            move_ver_show_rect(delta_ver_range);
            pressed_point_ = event->pos();
            return;
        }

        switch (cursorLocation()) {
            case LOCATION_NEAR_HOR_AXIS_SHOW_LEFT: {
                move_hor_show_left(map_to_hor_value(event->x()));
                break;
            }

            case LOCATION_NEAR_HOR_AXIS_SHOW_RIGHT: {
                move_hor_show_right(map_to_hor_value(event->x()));
                break;
            }

            case LOCATION_NEAR_HOR_AXIS_SHOW_RECT: {
                auto x         = map_to_hor_value(event->x());
                auto old_x     = map_to_hor_value(pressed_point_->x());
                auto delta_x   = x - old_x;
                pressed_point_ = event->pos();
                move_hor_show_rect(delta_x);
                break;
            }

            case LOCATION_NEAR_VER_AXIS_SHOW_TOP: {
                move_ver_show_top(map_to_ver_value(event->y()));
                break;
            }

            case LOCATION_NEAR_VER_AXIS_SHOW_BOTTOM: {
                move_ver_show_bottom(map_to_ver_value(event->y()));
                break;
            }

            case LOCATION_NEAR_VER_AXIS_SHOW_RECT: {
                auto y         = map_to_ver_value(event->y());
                auto old_y     = map_to_ver_value(pressed_point_->y());
                auto delta_y   = y - old_y;
                pressed_point_ = event->pos();
                move_ver_show_rect(delta_y);
                break;
            }
            default:
                break;
        }

        if (cursorLocation() & CursorLocation::LOCATION_NEAR_RED_VERTIVAL_LINE) {
            if (event->pos().x() < drawable().left()) {
                setMeasuringPointRed(QPoint(drawable().left(), measuringPointRed().y()));
            } else if (event->pos().x() > drawable().right()) {
                setMeasuringPointRed(QPoint(drawable().right(), measuringPointRed().y()));
            } else {
                setMeasuringPointRed(QPoint(event->pos().x(), measuringPointRed().y()));
            }
        }

        if (cursorLocation() & CursorLocation::LOCATION_NEAR_RED_HORIZONTAL_LINE) {
            if (event->pos().y() < drawable().top()) {
                setMeasuringPointRed(QPoint(measuringPointRed().x(), drawable().top()));
            } else if (event->pos().y() > drawable().bottom()) {
                setMeasuringPointRed(QPoint(measuringPointRed().x(), drawable().bottom()));
            } else {
                setMeasuringPointRed(QPoint(measuringPointRed().x(), event->pos().y()));
            }
        }

        if (cursorLocation() & CursorLocation::LOCATION_NEAR_BLUE_VERTIVAL_LINE) {
            if (event->pos().x() < drawable().left()) {
                setMeasuringPointBlue(QPoint(drawable().left(), measuringPointBlue().y()));
            } else if (event->pos().x() > drawable().right()) {
                setMeasuringPointBlue(QPoint(drawable().right(), measuringPointBlue().y()));
            } else {
                setMeasuringPointBlue(QPoint(event->pos().x(), measuringPointBlue().y()));
            }
        }

        if (cursorLocation() & CursorLocation::LOCATION_NEAR_BLUE_HORIZONTAL_LINE) {
            if (event->pos().y() < drawable().top()) {
                setMeasuringPointBlue(QPoint(measuringPointBlue().x(), drawable().top()));
            } else if (event->pos().y() > drawable().bottom()) {
                setMeasuringPointBlue(QPoint(measuringPointBlue().x(), drawable().bottom()));
            } else {
                setMeasuringPointBlue(QPoint(measuringPointBlue().x(), event->pos().y()));
            }
        }
    }

    void IScanView::mouseReleaseEvent(QMouseEvent* event) {
        setIsMousePressed(false);
        pressed_point_ = std::nullopt;
        if (box_selection_.has_value()) {
            setMeasuringPointRed(box_selection_->topLeft());
            setMeasuringPointBlue(box_selection_->bottomRight());
            box_selection_ = std::nullopt;
        }
        // 鼠标释放时, 重新调用hoverMoveEvent设置一下鼠标状态
        QHoverEvent hover_event(QEvent::HoverMove, event->pos(), event->pos());
        hoverMoveEvent(&hover_event);
    }

    void IScanView::hoverEnterEvent(QHoverEvent* event) {
        Q_UNUSED(event);
        forceActiveFocus();
    }

    void IScanView::hoverMoveEvent(QHoverEvent* event) {
        if (isMousePressed()) {
            // 如果鼠标已经被按下, 则不改变鼠标位置的状态
            return;
        }

        if (hor_axis_rect().contains(event->pos())) {
            // 鼠标在X轴区域内
            change_location_and_cursor_hor(event);
        } else if (ver_axis_rect().contains(event->pos())) {
            // 鼠标在Y轴区域内
            change_location_and_cursor_ver(event);
        } else if (drawable().contains(event->pos())) {
            change_location_and_cursor_measuring_line(event);
        } else {
            change_location_and_cursor_to_none();
        }
    }

    void IScanView::hoverLeaveEvent(QHoverEvent* event) {
        Q_UNUSED(event)
        setIsMousePressed(false);
        pressed_point_ = std::nullopt;
        change_location_and_cursor_to_none();
    }

    void IScanView::keyPressEvent(QKeyEvent* event) {
        Q_UNUSED(event)
        if (event->key() == Qt::Key_Control) {
            setIsCtrlPressed(true);
        } else if (event->key() == Qt::Key_W) {
            if (measuringPointRed().y() == drawable().top()) {
                return;
            }
            setMeasuringPointRed(measuringPointRed() + QPoint(0, -1));
        } else if (event->key() == Qt::Key_S) {
            if (measuringPointRed().y() == drawable().bottom()) {
                return;
            }
            setMeasuringPointRed(measuringPointRed() + QPoint(0, 1));
        } else if (event->key() == Qt::Key_A) {
            if (measuringPointRed().x() == drawable().left()) {
                return;
            }
            setMeasuringPointRed(measuringPointRed() + QPoint(-1, 0));
        } else if (event->key() == Qt::Key_D) {
            if (measuringPointRed().x() == drawable().right()) {
                return;
            }
            setMeasuringPointRed(measuringPointRed() + QPoint(1, 0));
        } else if (event->key() == Qt::Key_Up) {
            if (measuringPointBlue().y() == drawable().top()) {
                return;
            }
            setMeasuringPointBlue(measuringPointBlue() + QPoint(0, -1));
        } else if (event->key() == Qt::Key_Down) {
            if (measuringPointBlue().y() == drawable().bottom()) {
                return;
            }
            setMeasuringPointBlue(measuringPointBlue() + QPoint(0, 1));
        } else if (event->key() == Qt::Key_Left) {
            if (measuringPointBlue().x() == drawable().left()) {
                return;
            }
            setMeasuringPointBlue(measuringPointBlue() + QPoint(-1, 0));
        } else if (event->key() == Qt::Key_Right) {
            if (measuringPointBlue().x() == drawable().right()) {
                return;
            }
            setMeasuringPointBlue(measuringPointBlue() + QPoint(1, 0));
        }
    }

    void IScanView::keyReleaseEvent(QKeyEvent* event) {
        Q_UNUSED(event)
        if (event->key() == Qt::Key_Control) {
            setIsCtrlPressed(false);
        }
    }

    void IScanView::wheelEvent(QWheelEvent* event) {
        auto angle = event->angleDelta();
        qCDebug(TAG) << "滚轮:" << angle << event->pixelDelta();
    }

    void IScanView::draw_image(const QImage& img, QPainter* painter) {
        painter->drawImage(drawable(), img);
    }

    QPoint IScanView::measuringPointBlue() const {
        return m_measuringPointBlue;
    }

    void IScanView::setMeasuringPointBlue(QPoint newMeasuringPointBlue) {
        if (m_measuringPointBlue == newMeasuringPointBlue)
            return;
        m_measuringPointBlue = newMeasuringPointBlue;
        emit measuringPointBlueChanged();
    }

    QPoint IScanView::measuringPointRed() const {
        return m_measuringPointRed;
    }

    void IScanView::setMeasuringPointRed(QPoint newMeasuringPointRed) {
        if (m_measuringPointRed == newMeasuringPointRed)
            return;
        m_measuringPointRed = newMeasuringPointRed;
        emit measuringPointRedChanged();
    }

    QRect IScanView::drawable() const {
        return QRect(AXIS_WIDTH, 0, width() - AXIS_WIDTH, height() - AXIS_WIDTH);
    }

    QSize IScanView::drawable_size() const {
        return drawable().size();
    }

    std::optional<QSize> IScanView::image_size() const {
        if (!image_raw_.has_value()) {
            return std::nullopt;
        }
        return image_raw_->size();
    }

    Range IScanView::drawable_hor_range() const {
        return std::make_pair(drawable().left(), drawable().right());
    }

    Range IScanView::drawable_ver_range() const {
        return std::make_pair(drawable().top(), drawable().bottom());
    }

    QRect IScanView::hor_axis_rect() const {
        return QRect(QPoint(drawable().left(), drawable().bottom() + 1),
                     QSize(drawable().width(), AXIS_WIDTH));
    }

    QRect IScanView::ver_axis_rect() const {
        return QRect(QPoint(0, 0), QSize(AXIS_WIDTH, drawable().height()));
    }

    QRect IScanView::hor_show_rect() const {
        const std::pair dist_x_range = {drawable().left(), drawable().right()};
        const auto      x1           = ValueMap(horShowRange().first, dist_x_range, horRange());
        const auto      y1           = drawable().bottom() + 1;
        const auto      x2           = ValueMap(horShowRange().second, dist_x_range, horRange());
        const auto      y2           = height() - 1;

        return QRect(QPoint(x1, y1), QPoint(x2, y2));
    }

    QRect IScanView::ver_show_rect() const {
        const std::pair dist_range = {drawable().top(), drawable().bottom()};
        const auto      x1         = 0;
        const auto      y1         = ValueMap(verShowRange().first, dist_range, verRange());
        const auto      x2         = AXIS_WIDTH - 1;
        const auto      y2         = ValueMap(verShowRange().second, dist_range, verRange());

        return QRect(QPoint(x1, y1), QPoint(x2, y2));
    }

    QRect IScanView::display_text_rect() const {
        return QRect(0, drawable().bottom() + 1, AXIS_WIDTH, AXIS_WIDTH);
    }

    qreal IScanView::map_to_hor_value(qreal x) const {
        return ValueMap(x, horRange(), drawable_hor_range());
    }

    qreal IScanView::map_to_ver_value(qreal y) const {
        return ValueMap(y, verRange(), drawable_ver_range());
    }

    qreal IScanView::map_to_show_hor_value(qreal x) const {
        return ValueMap(x, horShowRange(), drawable_hor_range());
    }

    qreal IScanView::map_to_show_ver_value(qreal y) const {
        return ValueMap(y, verShowRange(), drawable_ver_range());
    }

    qreal IScanView::map_to_hor_value_display(qreal x) const {
        if (AppSetting::Instance()->roundToInt()) {
            return KeepDecimals<0>(map_to_hor_value(x));
        }
        return KeepDecimals<1>(map_to_hor_value(x));
    }

    qreal IScanView::map_to_ver_value_display(qreal y) const {
        if (AppSetting::Instance()->roundToInt()) {
            return KeepDecimals<0>(map_to_ver_value(y));
        }
        return KeepDecimals<1>(map_to_ver_value(y));
    }

    qreal IScanView::map_to_show_hor_value_display(qreal x) const {
        if (AppSetting::Instance()->roundToInt()) {
            return KeepDecimals<0>(map_to_show_hor_value(x));
        }
        return KeepDecimals<1>(map_to_show_hor_value(x));
    }

    qreal IScanView::map_to_show_ver_value_display(qreal y) const {
        if (AppSetting::Instance()->roundToInt()) {
            return KeepDecimals<0>(map_to_show_ver_value(y));
        }
        return KeepDecimals<1>(map_to_show_ver_value(y));
    }

    qreal IScanView::show_hor_x_left() const {
        return ValueMap(horShowRange().first, drawable_hor_range(), horRange());
    }

    qreal IScanView::show_hor_x_right() const {
        return ValueMap(horShowRange().second, drawable_hor_range(), horRange());
    }

    qreal IScanView::show_ver_y_top() const {
        return ValueMap(verShowRange().first, drawable_ver_range(), verRange());
    }

    qreal IScanView::show_ver_y_bottom() const {
        return ValueMap(verShowRange().second, drawable_ver_range(), verRange());
    }

    std::optional<qreal> IScanView::min_show_range_width() const {
        if (!image_raw_.has_value()) {
            return std::nullopt;
        }

        if (drawable_size().width() >= image_raw_->width()) {
            return std::nullopt;
        }

        const auto left  = ValueMap(0, horRange(), {0, image_raw_->width() - 1});
        const auto right = ValueMap(drawable_size().width() - 1, horRange(), {0, image_raw_->width() - 1});
        return right - left;
    }

    std::optional<qreal> IScanView::min_show_range_height() const {
        if (!image_raw_.has_value()) {
            return std::nullopt;
        }

        if (drawable_size().height() >= image_raw_->height()) {
            return std::nullopt;
        }

        const auto top    = ValueMap(0, verRange(), {0, image_raw_->height() - 1});
        const auto bottom = ValueMap(drawable_size().height() - 1, verRange(), {0, image_raw_->height() - 1});
        return bottom - top;
    }

    void IScanView::draw_hor_axis(QPainter* painter) const {
        const auto tick_count = KeepDecimals<0>(width() / 100.0);
        // 绘制坐标轴底色
        painter->fillRect(hor_axis_rect(), AXIS_BACKGROUND_COLOR);
        // 绘制坐标轴掩膜
        painter->fillRect(hor_show_rect(), AXIS_MASK_COLOR);
        QFontMetricsF font_metrics = painter->fontMetrics();
        // 水平
        for (int i = 0; i <= tick_count * 10; i++) {
            qreal  x                   = std::round(ValueMap(i, drawable_hor_range(), {0, tick_count * 10}));
            qreal  y                   = drawable().bottom() + 1;
            QColor color               = QColor(0x00cc00);
            qreal  display_value       = map_to_show_hor_value_display(x);
            qreal  display_value_extra = map_to_hor_value_display(x);
            QLineF line;
            QLineF line_extra;
            painter->setPen(color);
            if (i % 10 == 0) {
                line       = {x, y, x, y + 15};
                line_extra = {x, height() - 1, x, height() - 1 - 15};
                if (i == 0) {
                    color = QColor(255, 0, 0);
                    painter->setPen(color);
                } else {
                    color = QColor(0, 0, 0);
                    painter->setPen(color);
                }
                QRectF fm_rect = font_metrics.boundingRect(QString::number(display_value));
                if (i != (tick_count * 10)) {
                    fm_rect.moveTopLeft(QPointF(line.p2().x() + 5, line.p2().y() - 10));
                } else {
                    fm_rect.moveTopRight(QPointF(line.p2().x() - 5, line.p2().y() - 10));
                }
                painter->drawText(fm_rect, Qt::AlignCenter, QString::number(display_value));
                fm_rect = font_metrics.boundingRect(QString::number(display_value_extra));
                if (i != (tick_count * 10)) {
                    fm_rect.moveTopLeft(QPointF(line.p2().x() + 5, line.p2().y() + 5));
                } else {
                    fm_rect.moveTopRight(QPointF(line.p2().x() - 5, line.p2().y() + 5));
                }
                painter->drawText(fm_rect, Qt::AlignCenter, QString::number(display_value_extra));
            } else if (i % 5 == 0) {
                line       = {x, y, x, y + 10};
                line_extra = {x, height() - 1, x, height() - 1 - 10};
            } else {
                line       = {x, y, x, y + 5};
                line_extra = {x, height() - 1, x, height() - 1 - 5};
            }
            painter->drawLine(line_extra);
            painter->drawLine(line);
        }
    }

    void IScanView::draw_ver_axis(QPainter* painter) const {
        const auto tick_count = KeepDecimals<0>(height() / 100.0);

        painter->fillRect(ver_axis_rect(), AXIS_BACKGROUND_COLOR);
        painter->fillRect(ver_show_rect(), AXIS_MASK_COLOR);

        QFontMetricsF font_metrics = painter->fontMetrics();
        // 垂直
        for (int i = 0; i <= tick_count * 10; i++) {
            qreal  x                   = drawable().left() - 1;
            qreal  y                   = std::round(ValueMap(i, drawable_ver_range(), {tick_count * 10, 0}));
            qreal  display_value       = map_to_show_ver_value_display(y);
            qreal  display_value_extra = map_to_ver_value_display(y);
            QColor color               = QColor(0x00cc00);
            QLineF line;
            QLineF line_extra;
            painter->setPen(color);
            if (i % 10 == 0) {
                line       = {x - 15, y, x, y};
                line_extra = {0, y, 14, y};
                if (i == (tick_count * 10)) {
                    color = QColor(255, 0, 0);
                    painter->setPen(color);
                } else {
                    color = QColor(0, 0, 0);
                    painter->setPen(color);
                }
                painter->save();
                QRectF fm_rect       = font_metrics.boundingRect(QString::number(display_value));
                QRectF fm_rect_extra = font_metrics.boundingRect(QString::number(display_value_extra));
                if (i == 0) {
                    fm_rect.moveTopRight(QPointF(-10, -5));
                    fm_rect_extra.moveTopRight(QPointF(-10, 10));
                } else {
                    fm_rect.moveTop(-5);
                    fm_rect_extra.moveTop(10);
                }
                painter->translate(line.p1().x() + 5, line.p1().y() + 5);
                painter->rotate(90);
                painter->drawText(fm_rect, Qt::AlignCenter, QString::number(display_value));
                painter->drawText(fm_rect_extra, Qt::AlignCenter, QString::number(display_value_extra));
                painter->restore();
            } else if (i % 5 == 0) {
                line       = {x - 10, y, x, y};
                line_extra = {0, y, 9, y};
            } else {
                line       = {x - 5, y, x, y};
                line_extra = {0, y, 4, y};
            }
            painter->drawLine(line_extra);
            painter->drawLine(line);
        };
    }

    void IScanView::draw_display_text(QPainter* painter) const {
        painter->fillRect(display_text_rect(), AXIS_BACKGROUND_COLOR);
        auto pen = painter->pen();
        pen.setColor(Qt::black);
        painter->setPen(pen);
        auto font = painter->font();
        font.setPointSize(20);
        painter->setFont(font);
        QRectF fm_rect = painter->fontMetrics().boundingRect(display_text_);
        fm_rect.moveCenter(display_text_rect().center());
        painter->drawText(fm_rect, Qt::AlignCenter, display_text_);
    }

    void IScanView::draw_measuring_line(QPainter* painter) const {
        Q_ASSERT(painter);

        QColor red_line_vertical    = Qt::red;
        QColor red_line_horizontal  = Qt::red;
        QColor blue_line_vertical   = Qt::blue;
        QColor blue_line_horizontal = Qt::blue;

        if (cursorLocation() & CursorLocation::LOCATION_NEAR_RED_VERTIVAL_LINE) {
            red_line_vertical = Qt::green;
        }

        if (cursorLocation() & CursorLocation::LOCATION_NEAR_RED_HORIZONTAL_LINE) {
            red_line_horizontal = Qt::green;
        }

        if (cursorLocation() & CursorLocation::LOCATION_NEAR_BLUE_VERTIVAL_LINE) {
            blue_line_vertical = Qt::green;
        }

        if (cursorLocation() & CursorLocation::LOCATION_NEAR_BLUE_HORIZONTAL_LINE) {
            blue_line_horizontal = Qt::green;
        }

        painter->setPen(QPen(blue_line_horizontal, 2));
        painter->drawLine(QPoint(drawable().left(), measuringPointBlue().y()), QPoint(drawable().right(), measuringPointBlue().y()));
        painter->setPen(QPen(blue_line_vertical, 2));
        painter->drawLine(QPoint(measuringPointBlue().x(), drawable().top()), QPoint(measuringPointBlue().x(), drawable().bottom()));

        painter->setPen(QPen(red_line_horizontal, 2));
        painter->drawLine(QPoint(drawable().left(), measuringPointRed().y()), QPoint(drawable().right(), measuringPointRed().y()));
        painter->setPen(QPen(red_line_vertical, 2));
        painter->drawLine(QPoint(measuringPointRed().x(), drawable().top()), QPoint(measuringPointRed().x(), drawable().bottom()));
    }

    void IScanView::draw_measuring_text(QPainter* painter) {
        painter->setPen(QPen(Qt::black, 1));
        auto font = painter->font();
        font.setBold(true);
        font.setPointSize(13);
        painter->setFont(font);
        QFontMetricsF font_metrics = painter->fontMetrics();

        const auto value_vertical_red = Common::KeepDecimals<1>(Common::ValueMap(
            measuringPointRed().x(),
            horShowRange(),
            drawable_hor_range()));

        setVerRedValue(value_vertical_red);

        const auto value_vertical_blue = Common::KeepDecimals<1>(Common::ValueMap(
            measuringPointBlue().x(),
            horShowRange(),
            drawable_hor_range()));

        setVerBlueValue(value_vertical_blue);

        const auto value_vertical_bias = Common::KeepDecimals<1>(value_vertical_blue - value_vertical_red);

        const auto value_horizontal_red = Common::KeepDecimals<1>(Common::ValueMap(
            measuringPointRed().y(),
            verShowRange(),
            drawable_ver_range()));

        setHorRedValue(value_horizontal_red);

        const auto value_horizontal_blue = Common::KeepDecimals<1>(Common::ValueMap(
            measuringPointBlue().y(),
            verShowRange(),
            drawable_ver_range()));

        setHorBlueValue(value_horizontal_blue);

        const auto value_horizontal_bias = Common::KeepDecimals<1>(value_horizontal_blue - value_horizontal_red);

        QString str  = QString::asprintf("%.1f", value_vertical_red);
        auto    rect = font_metrics.boundingRect(str);

        if (measuringPointRed().x() <= (drawable_size().width() / 2.0)) {
            rect.moveTo(measuringPointRed().x() + 5, 0);
        } else {
            rect.moveTo(measuringPointRed().x() - rect.width() - 5, 0);
        }
        painter->setPen(QPen(Qt::black, 1));
        painter->drawText(rect.bottomLeft(), str);

        str  = QString::asprintf("%.1f", value_vertical_blue);
        rect = font_metrics.boundingRect(str);

        if (measuringPointBlue().x() <= (drawable_size().width() / 2.0)) {
            rect.moveTo(measuringPointBlue().x() + 5, 20);
        } else {
            rect.moveTo(measuringPointBlue().x() - rect.width() - 5, 20);
        }
        painter->setPen(QPen(Qt::black, 1));
        painter->drawText(rect.bottomLeft(), str);

        str  = QString::asprintf("水平间距: %.1f", value_vertical_bias);
        rect = font_metrics.boundingRect(str);

        rect.moveTopRight(drawable().topRight() + QPoint(-10, 0));
        painter->setPen(QPen(Qt::black, 1));
        painter->drawText(rect.bottomLeft(), str);

        str  = QString::asprintf("%.1f", value_horizontal_red);
        rect = font_metrics.boundingRect(str);

        if (measuringPointRed().y() <= (drawable_size().height() / 2.0)) {
            if (measuringPointRed().x() < drawable().left() + rect.width() + 50) {
                rect.moveTopLeft(QPoint(drawable().left(), measuringPointRed().y()));
            } else {
                rect.moveTopRight(measuringPointRed() + QPoint(-50, 0));
            }
        } else {
            if (measuringPointRed().x() < drawable().left() + rect.width() + 50) {
                rect.moveTopLeft(QPoint(drawable().left(), measuringPointRed().y()));
            } else {
                rect.moveBottomRight(measuringPointRed() + QPoint(-50, -10));
            }
        }
        painter->setPen(QPen(Qt::black, 1));
        painter->drawText(rect.bottomLeft(), str);

        str  = QString::asprintf("%.1f", value_horizontal_blue);
        rect = font_metrics.boundingRect(str);

        if (measuringPointBlue().y() <= (drawable_size().height() / 2.0)) {
            if (measuringPointBlue().x() < drawable().right() - rect.width() - 50) {
                rect.moveTopLeft(measuringPointBlue() + QPoint(50, 0));
            } else {
                rect.moveTopRight(QPoint(drawable().right(), measuringPointBlue().y()));
            }
        } else {
            if (measuringPointBlue().x() < drawable().right() - rect.width() - 50) {
                rect.moveBottomLeft(measuringPointBlue() + QPoint(50, -10));
            } else {
                rect.moveBottomRight(QPoint(drawable().right(), measuringPointBlue().y()));
            }
        }

        painter->setPen(QPen(Qt::black, 1));
        painter->drawText(rect.bottomLeft(), str);

        str  = QString::asprintf("垂直间距: %.1f", value_horizontal_bias);
        rect = font_metrics.boundingRect(str);

        rect.moveTopRight(drawable().topRight() + QPoint(-10, rect.height()));
        painter->setPen(QPen(Qt::black, 1));
        painter->drawText(rect.bottomLeft(), str);
    }

    void IScanView::draw_box_selection(QPainter* painter) const {
        if (!box_selection_.has_value()) {
            return;
        }

        QPen pen = painter->pen();
        pen.setWidthF(1.5);
        pen.setBrush(Qt::black);
        painter->setPen(pen);
        painter->drawRect(box_selection_.value());
    }

    bool IScanView::is_near(qreal val, qreal target) const {
        if (std::abs(val - target) <= NEAR_THRESHOLD) {
            return true;
        }
        return false;
    }

    void IScanView::change_location_and_cursor_hor(QHoverEvent* event) {
        // 鼠标在X轴区域内
        if (is_near(event->pos().x(), show_hor_x_left())) {
            // 鼠标靠近显示掩膜的左边
            setCursor(Qt::SizeHorCursor);
            setCursorLocation(LOCATION_NEAR_HOR_AXIS_SHOW_LEFT);
        } else if (is_near(event->pos().x(), show_hor_x_right())) {
            // 鼠标靠近显示掩膜的右边
            setCursor(Qt::SizeHorCursor);
            setCursorLocation(LOCATION_NEAR_HOR_AXIS_SHOW_RIGHT);
        } else if (hor_show_rect().contains(event->pos())) {
            // 鼠标在掩膜区域内
            setCursor(Qt::PointingHandCursor);
            setCursorLocation(LOCATION_NEAR_HOR_AXIS_SHOW_RECT);
        } else {
            change_location_and_cursor_to_none();
        }
    }

    void IScanView::change_location_and_cursor_ver(QHoverEvent* event) {
        // 鼠标在Y轴区域内
        if (is_near(event->pos().y(), show_ver_y_top())) {
            // 鼠标靠近显示掩膜的上边
            setCursor(Qt::SizeVerCursor);
            setCursorLocation(LOCATION_NEAR_VER_AXIS_SHOW_TOP);
        } else if (is_near(event->pos().y(), show_ver_y_bottom())) {
            // 鼠标靠近显示掩膜的下边
            setCursor(Qt::SizeVerCursor);
            setCursorLocation(LOCATION_NEAR_VER_AXIS_SHOW_BOTTOM);
        } else if (ver_show_rect().contains(event->pos())) {
            // 鼠标在掩膜区域内
            setCursor(Qt::PointingHandCursor);
            setCursorLocation(LOCATION_NEAR_VER_AXIS_SHOW_RECT);
        } else {
            change_location_and_cursor_to_none();
        }
    }

    void IScanView::change_location_and_cursor_measuring_line(QHoverEvent* event) {
        bool size_horizontal_cursor = false;
        bool size_vertical_cursor   = false;

        auto cursor_location = CursorLocation::LOCATION_NEAR_NONE;

        if (is_near(event->pos().x(), measuringPointRed().x())) {
            cursor_location        = static_cast<CursorLocation>(cursor_location | CursorLocation::LOCATION_NEAR_RED_VERTIVAL_LINE);
            size_horizontal_cursor = true;
        } else if (is_near(event->pos().x(), measuringPointBlue().x())) {
            cursor_location        = static_cast<CursorLocation>(cursor_location | CursorLocation::LOCATION_NEAR_BLUE_VERTIVAL_LINE);
            size_horizontal_cursor = true;
        }

        if (is_near(event->pos().y(), measuringPointRed().y())) {
            cursor_location      = static_cast<CursorLocation>(cursor_location | CursorLocation::LOCATION_NEAR_RED_HORIZONTAL_LINE);
            size_vertical_cursor = true;
        } else if (is_near(event->pos().y(), measuringPointBlue().y())) {
            cursor_location      = static_cast<CursorLocation>(cursor_location | CursorLocation::LOCATION_NEAR_BLUE_HORIZONTAL_LINE);
            size_vertical_cursor = true;
        }

        if (size_horizontal_cursor && size_vertical_cursor) {
            setCursor(Qt::SizeAllCursor);
        } else if (size_horizontal_cursor) {
            setCursor(Qt::SizeHorCursor);
        } else if (size_vertical_cursor) {
            setCursor(Qt::SizeVerCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }

        setCursorLocation(cursor_location);
    }

    void IScanView::change_location_and_cursor_to_none() {
        setCursor(Qt::ArrowCursor);
        setCursorLocation(LOCATION_NEAR_NONE);
    }

    void IScanView::move_hor_show_left(double x) {
        auto range  = horShowRange();
        range.first = x;

        if (range.first > range.second) {
            return;
        }

        if (range.first < horRange().first) {
            range.first = horRange().first;
        }

        auto min_width = min_show_range_width();

        if (!min_width.has_value()) {
            return;
        }

        if (range.second - range.first < min_width.value()) {
            range.first = range.second - min_width.value();
        }

        setHorShowRange(range);
    }

    void IScanView::move_hor_show_right(double x) {
        auto range   = horShowRange();
        range.second = x;

        if (range.first > range.second) {
            return;
        }

        if (range.second > horRange().second) {
            range.second = horRange().second;
        }

        auto min_width = min_show_range_width();

        if (!min_width.has_value()) {
            return;
        }

        if (range.second - range.first < min_width.value()) {
            range.second = range.first + min_width.value();
        }

        setHorShowRange(range);
    }

    void IScanView::move_hor_show_rect(double delta) {
        auto range = horShowRange();
        range.first += delta;
        range.second += delta;

        if (range.first < horRange().first) {
            range.first  = horRange().first;
            range.second = horRange().first + (horShowRange().second - horShowRange().first);
        }

        if (range.second > horRange().second) {
            range.first  = horRange().second - (horShowRange().second - horShowRange().first);
            range.second = horRange().second;
        }

        setHorShowRange(range);
    }

    void IScanView::move_ver_show_top(double y) {
        auto range  = verShowRange();
        range.first = y;

        if (range.first > range.second) {
            return;
        }

        if (range.first < verRange().first) {
            range.first = verRange().first;
        }

        auto min_height = min_show_range_height();

        if (!min_height.has_value()) {
            return;
        }

        if (range.second - range.first < min_height.value()) {
            range.first = range.second - min_height.value();
        }

        setVerShowRange(range);
    }

    void IScanView::move_ver_show_bottom(double y) {
        auto range   = verShowRange();
        range.second = y;

        if (range.second < range.first) {
            return;
        }

        if (range.second > verRange().second) {
            range.second = verRange().second;
        }

        auto min_height = min_show_range_height();

        if (!min_height.has_value()) {
            return;
        }

        if (range.second - range.first < min_height.value()) {
            range.second = range.first + min_height.value();
        }

        setVerShowRange(range);
    }

    void IScanView::move_ver_show_rect(double delta) {
        auto range = verShowRange();
        range.first += delta;
        range.second += delta;

        if (range.first < verRange().first) {
            range.first  = verRange().first;
            range.second = range.first + (verShowRange().second - verShowRange().first);
        }

        if (range.second > verRange().second) {
            range.first  = verRange().second - (verShowRange().second - verShowRange().first);
            range.second = verRange().second;
        }

        setVerShowRange(range);
    }

    void IScanView::copy_image_visable_from_raw() {
        if (!image_raw_.has_value()) {
            image_visable_ = std::nullopt;
            return;
        }

        Range image_h_range = {0, image_raw_->width() - 1};
        Range image_v_range = {0, image_raw_->height() - 1};

        int left   = ValueMap(horShowRange().first, image_h_range, horRange());
        int right  = ValueMap(horShowRange().second, image_h_range, horRange());
        int top    = ValueMap(verShowRange().first, image_v_range, verRange());
        int bottom = ValueMap(verShowRange().second, image_v_range, verRange());
        qCDebug(TAG) << "image_raw size:" << image_raw_->size();
        qCDebug(TAG) << "copy image from:" << QRect(QPoint(left, top), QPoint(right, bottom));

        image_visable_ = image_raw_->copy(QRect(QPoint(left, top), QPoint(right, bottom)));
    }

    void IScanView::update_image_point() {
        auto image_pt = measuring_point_to_image_point(measuringPointRed());
        setImagePoint(image_pt.value_or(QPoint(0, 0)));
    }

    std::optional<QPoint> IScanView::measuring_point_to_image_point(QPoint pt) {
        if (!image_raw_.has_value() || !image_visable_.has_value()) {
            return std::nullopt;
        }

        Range image_h_range = {0, image_raw_->width() - 1};
        Range image_v_range = {0, image_raw_->height() - 1};

        double x_bias = ValueMap(horShowRange().first, image_h_range, horRange());
        double y_bias = ValueMap(verShowRange().first, image_v_range, verRange());

        int x = x_bias + (pt.x() - drawable().left()) / (drawable_size().width() / static_cast<double>(image_visable_->width()));
        int y = y_bias + pt.y() / (drawable_size().height() / static_cast<double>(image_visable_->height()));

        qCDebug(TAG) << "bias:" << x_bias << "," << y_bias;
        qCDebug(TAG) << "x:" << x << ", y:" << y;

        if (x < 0 || x >= image_raw_->width() || y < 0 || y >= image_raw_->height()) {
            return std::nullopt;
        }

        return QPoint(x, y);
    }

} // namespace Union::View
