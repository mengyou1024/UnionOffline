#pragma once

#include "BasicView.hpp"
#include <QImage>

namespace Union::View {
    class CScanView : public BasicView {
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

        QPoint         m_dataCursor          = {};
        QColor         m_cursorLineColor     = Qt::black;
        int            m_cursorLineWidth     = 1;
        int            m_dataCursorInt       = {};
        int            m_imgWidth            = 0;
        int            m_imgHeight           = 0;
        bool           m_isPressed           = false;
        bool           m_isControlPressed    = false;
        CursorLocation m_cursorLocation      = CursorLocation::LOCATION_NEAR_NONE;
        qreal          m_extraBScanRedValue  = {};
        qreal          m_extraBScanBlueValue = {};

    public:
        CScanView();
        ~CScanView() override;

        void           replace(const std::vector<std::optional<uint8_t>>& data, int width, int height, bool set_size = true) noexcept;
        QPoint         dataCursor() const;
        void           setDataCursor(QPoint newDataCursor);
        QColor         cursorLineColor() const;
        void           setCursorLineColor(const QColor& newCursorLineColor);
        int            cursorLineWidth() const;
        void           setCursorLineWidth(int newCursorLineWidth);
        int            dataCursorInt() const;
        void           setDataCursorInt(int newDataCursorInt);
        int            imgWidth() const;
        void           setImgWidth(int newImgWidth);
        int            imgHeight() const;
        void           setImgHeight(int newImgHeight);
        bool           isPressed() const;
        void           setIsPressed(bool newIsPressed);
        bool           isControlPressed() const;
        void           setIsControlPressed(bool newIsControlPressed);
        CursorLocation cursorLocation() const;
        void           setCursorLocation(CursorLocation newCursorLocation);

        Q_INVOKABLE void setRedVLineFromBScan(qreal pos);
        Q_INVOKABLE void setBlueVLineFromBScan(qreal pos);

        qreal extraBScanRedValue() const;
        void  setExtraBScanRedValue(qreal newExtraBScanRedValue);

        qreal extraBScanBlueValue() const;
        void  setExtraBScanBlueValue(qreal newExtraBScanBlueValue);

    signals:
        void dataCursorChanged();
        void cursorLineColorChanged();
        void cursorLineWidthChanged();
        void dataCursorIntChanged();
        void imgWidthChanged();
        void imgHeightChanged();
        void isPressedChanged();
        void isControlPressedChanged();
        void cursorLocationChanged();
        void updateExtraBScanRedHLine(qreal pos);
        void updateExtraBScanBlueHLine(qreal pos);
        void extraBScanRedValueChanged();
        void extraBScanBlueValueChanged();

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
        QImage m_image              = {};
        int    _raw_width           = 0;
        int    _raw_height          = 0;
        QPoint m_measuringPointRed  = {};
        QPoint m_measuringPointBlue = {};

        bool eventHandlerCommon(QMouseEvent* event) noexcept;
        void drawMeasuringLine(QPainter* painter);
        void drawMeasuringText(QPainter* painter);

        Q_PROPERTY(QPoint dataCursor READ dataCursor WRITE setDataCursor NOTIFY dataCursorChanged FINAL)
        Q_PROPERTY(QColor cursorLineColor READ cursorLineColor WRITE setCursorLineColor NOTIFY cursorLineColorChanged FINAL)
        Q_PROPERTY(int cursorLineWidth READ cursorLineWidth WRITE setCursorLineWidth NOTIFY cursorLineWidthChanged FINAL)
        Q_PROPERTY(int dataCursorInt READ dataCursorInt WRITE setDataCursorInt NOTIFY dataCursorIntChanged FINAL)
        Q_PROPERTY(int imgWidth READ imgWidth WRITE setImgWidth NOTIFY imgWidthChanged FINAL)
        Q_PROPERTY(int imgHeight READ imgHeight WRITE setImgHeight NOTIFY imgHeightChanged FINAL)
        Q_PROPERTY(bool isPressed READ isPressed WRITE setIsPressed NOTIFY isPressedChanged FINAL)
        Q_PROPERTY(bool isControlPressed READ isControlPressed WRITE setIsControlPressed NOTIFY isControlPressedChanged FINAL)
        Q_PROPERTY(CursorLocation cursorLocation READ cursorLocation WRITE setCursorLocation NOTIFY cursorLocationChanged FINAL)
        Q_PROPERTY(qreal extraBScanRedValue READ extraBScanRedValue WRITE setExtraBScanRedValue NOTIFY extraBScanRedValueChanged FINAL)
        Q_PROPERTY(qreal extraBScanBlueValue READ extraBScanBlueValue WRITE setExtraBScanBlueValue NOTIFY extraBScanBlueValueChanged FINAL)
    };
} // namespace Union::View
