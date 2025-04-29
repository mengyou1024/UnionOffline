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

    public:
        CScanView();

        void   replace(const std::vector<std::optional<uint8_t>> &data, int width, int height) noexcept;
        QPoint dataCursor() const;
        void   setDataCursor(QPoint newDataCursor);
        QColor cursorLineColor() const;
        void   setCursorLineColor(const QColor &newCursorLineColor);
        int    cursorLineWidth() const;
        void   setCursorLineWidth(int newCursorLineWidth);
        int    dataCursorInt() const;
        void   setDataCursorInt(int newDataCursorInt);

    signals:
        void dataCursorChanged();
        void cursorLineColorChanged();
        void cursorLineWidthChanged();
        void dataCursorIntChanged();

    protected:
        virtual void paint(QPainter *painter) override;

        void mousePressEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *) override;

    private:
        QImage                m_image     = {};
        std::optional<QPoint> m_drawPoint = std::nullopt;

        bool eventHandlerCommon(QMouseEvent *event) noexcept;

        Q_PROPERTY(QPoint dataCursor READ dataCursor WRITE setDataCursor NOTIFY dataCursorChanged FINAL)
        Q_PROPERTY(QColor cursorLineColor READ cursorLineColor WRITE setCursorLineColor NOTIFY cursorLineColorChanged FINAL)
        Q_PROPERTY(int cursorLineWidth READ cursorLineWidth WRITE setCursorLineWidth NOTIFY cursorLineWidthChanged FINAL)
        Q_PROPERTY(int dataCursorInt READ dataCursorInt WRITE setDataCursorInt NOTIFY dataCursorIntChanged FINAL)
    };
} // namespace Union::View
