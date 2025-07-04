#pragma once

#include "BasicView.hpp"
#include <QImage>

namespace Union::View {
    class BScanView : public BasicView {
    public:
        enum CursorLocation {
            LOCATION_NEAR_NONE                 = 0,
            LOCATION_NEAR_RED_VERTIVAL_LINE    = 1,
            LOCATION_NEAR_RED_HORIZONTAL_LINE  = 1 << 1,
            LOCATION_NEAR_BLUE_VERTIVAL_LINE   = 1 << 2,
            LOCATION_NEAR_BLUE_HORIZONTAL_LINE = 1 << 3,

            LOCATION_TOP_LEFT     = LOCATION_NEAR_RED_VERTIVAL_LINE | LOCATION_NEAR_RED_HORIZONTAL_LINE,
            LOCATION_TOP_RIGHT    = LOCATION_NEAR_RED_HORIZONTAL_LINE | LOCATION_NEAR_BLUE_VERTIVAL_LINE,
            LOCATION_BOTTOM_LEFT  = LOCATION_NEAR_RED_VERTIVAL_LINE | LOCATION_NEAR_BLUE_HORIZONTAL_LINE,
            LOCATION_BOTTOM_RIGHT = LOCATION_NEAR_BLUE_VERTIVAL_LINE | LOCATION_NEAR_BLUE_HORIZONTAL_LINE,

        };

    private:
        Q_OBJECT
        QML_ELEMENT

        int            m_dataCursor       = {};
        QColor         m_cursorLineColor  = Qt::black;
        int            m_cursorLineWidth  = 1;
        int            m_imgWidth         = 0;
        int            m_imgHeight        = 0;
        bool           m_enableCursor     = true;
        bool           m_isPressed        = false;
        bool           m_isControlPressed = false;
        CursorLocation m_cursorLocation   = CursorLocation::LOCATION_NEAR_NONE;

    public:
        BScanView();
        ~BScanView() override;

        void              replace(const std::vector<std::optional<uint8_t>>& data, int width, int height, bool set_size = true) noexcept;
        [[nodiscard]] int dataCursor() const;
        void              setDataCursor(int newDataCursor);

        [[nodiscard]] QColor cursorLineColor() const;
        void                 setCursorLineColor(const QColor& newCursorLineColor);
        [[nodiscard]] int    cursorLineWidth() const;
        void                 setCursorLineWidth(int newCursorLineWidth);
        [[nodiscard]] int    imgWidth() const;
        void                 setImgWidth(int newImgWidth);
        [[nodiscard]] int    imgHeight() const;
        void                 setImgHeight(int newImgHeight);
        [[nodiscard]] bool   enableCursor() const;
        void                 setEnableCursor(bool newEnableCursor);
        CursorLocation       cursorLocation() const;
        void                 setCursorLocation(CursorLocation newCursorLocation);
        bool                 isPressed() const;
        void                 setIsPressed(bool newIsPressed);
        bool                 isControlPressed() const;
        void                 setIsControlPressed(bool newIsControlPressed);

        Q_INVOKABLE void setRedHLineFromCScan(qreal pos);
        Q_INVOKABLE void setBlueHLineFromCScan(qreal pos);
        Q_INVOKABLE void setValueFromCScan(qreal red, qreal blue);
        Q_INVOKABLE void setRedValueFromCScan(qreal red);
        Q_INVOKABLE void setBlueValueFromCScan(qreal blue);
        Q_INVOKABLE void clearValueFromCScan();

    signals:

        void dataCursorChanged();
        void cursorLineColorChanged();
        void cursorLineWidthChanged();
        void imgWidthChanged();
        void imgHeightChanged();
        void enableCursorChanged();
        void cursorLocationChanged();
        void isPressedChanged();
        void isControlPressedChanged();

        void updateCScanRedVLine(qreal pos);
        void updateCScanBlueVLine(qreal pos);

    public slots:
        void resetLines();

    protected:
        virtual void paint(QPainter* painter) override;

        void mousePressEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent*) override;
        void hoverEnterEvent(QHoverEvent* event) override;
        void hoverMoveEvent(QHoverEvent* event) override;
        void hoverLeaveEvent(QHoverEvent* event) override;
        void keyPressEvent(QKeyEvent* event) override;
        void keyReleaseEvent(QKeyEvent* event) override;

    private:
        QImage m_image = {};

        QPoint m_measuringPointRed  = {};
        QPoint m_measuringPointBlue = {};

        std::optional<double> _red_h_value  = std::nullopt;
        std::optional<double> _blue_h_value = std::nullopt;

        bool eventHandlerCommon(QMouseEvent* event) noexcept;
        void drawMeasuringLine(QPainter* painter);
        void drawMeasuringText(QPainter* painter);

        Q_PROPERTY(int dataCursor READ dataCursor WRITE setDataCursor NOTIFY dataCursorChanged FINAL)
        Q_PROPERTY(QColor cursorLineColor READ cursorLineColor WRITE setCursorLineColor NOTIFY cursorLineColorChanged FINAL)
        Q_PROPERTY(int cursorLineWidth READ cursorLineWidth WRITE setCursorLineWidth NOTIFY cursorLineWidthChanged FINAL)
        Q_PROPERTY(int imgWidth READ imgWidth WRITE setImgWidth NOTIFY imgWidthChanged FINAL)
        Q_PROPERTY(int imgHeight READ imgHeight WRITE setImgHeight NOTIFY imgHeightChanged FINAL)
        Q_PROPERTY(bool enableCursor READ enableCursor WRITE setEnableCursor NOTIFY enableCursorChanged FINAL)
        Q_PROPERTY(CursorLocation cursorLocation READ cursorLocation WRITE setCursorLocation NOTIFY cursorLocationChanged FINAL)
        Q_PROPERTY(bool isPressed READ isPressed WRITE setIsPressed NOTIFY isPressedChanged FINAL)
        Q_PROPERTY(bool isControlPressed READ isControlPressed WRITE setIsControlPressed NOTIFY isControlPressedChanged FINAL)
    };
} // namespace Union::View
