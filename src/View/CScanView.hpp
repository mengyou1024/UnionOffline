#pragma once

#include "BasicView.hpp"
#include <QImage>

namespace Union::View {
    class CScanView : public BasicView {
        Q_OBJECT
        QML_ELEMENT

        QPoint m_dataCursor      = {};
        QColor m_cursorLineColor = Qt::black;
        int    m_cursorLineWidth = 1;
        int    m_dataCursorInt   = {};
        int    m_imgWidth        = 0;
        int    m_imgHeight       = 0;

    public:
        CScanView();

        void   replace(const std::vector<std::optional<uint8_t>>& data, int width, int height, bool set_size = true) noexcept;
        QPoint dataCursor() const;
        void   setDataCursor(QPoint newDataCursor);
        QColor cursorLineColor() const;
        void   setCursorLineColor(const QColor& newCursorLineColor);
        int    cursorLineWidth() const;
        void   setCursorLineWidth(int newCursorLineWidth);
        int    dataCursorInt() const;
        void   setDataCursorInt(int newDataCursorInt);

        int  imgWidth() const;
        void setImgWidth(int newImgWidth);

        int  imgHeight() const;
        void setImgHeight(int newImgHeight);

    signals:
        void dataCursorChanged();
        void cursorLineColorChanged();
        void cursorLineWidthChanged();
        void dataCursorIntChanged();

        void imgWidthChanged();

        void imgHeightChanged();

    protected:
        virtual void paint(QPainter* painter) override;

        void mousePressEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent*) override;

    private:
        QImage                m_image     = {};
        int                   _raw_width  = 0;
        int                   _raw_height = 0;
        std::optional<QPoint> m_drawPoint = std::nullopt;

        bool eventHandlerCommon(QMouseEvent* event) noexcept;

        Q_PROPERTY(QPoint dataCursor READ dataCursor WRITE setDataCursor NOTIFY dataCursorChanged FINAL)
        Q_PROPERTY(QColor cursorLineColor READ cursorLineColor WRITE setCursorLineColor NOTIFY cursorLineColorChanged FINAL)
        Q_PROPERTY(int cursorLineWidth READ cursorLineWidth WRITE setCursorLineWidth NOTIFY cursorLineWidthChanged FINAL)
        Q_PROPERTY(int dataCursorInt READ dataCursorInt WRITE setDataCursorInt NOTIFY dataCursorIntChanged FINAL)
        Q_PROPERTY(int imgWidth READ imgWidth WRITE setImgWidth NOTIFY imgWidthChanged FINAL)
        Q_PROPERTY(int imgHeight READ imgHeight WRITE setImgHeight NOTIFY imgHeightChanged FINAL)
    };
} // namespace Union::View
