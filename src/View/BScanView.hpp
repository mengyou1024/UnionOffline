#pragma once

#include "BasicView.hpp"
#include <QImage>

namespace Union::View {
    class BScanView : public BasicView {
        Q_OBJECT
        QML_ELEMENT

        int    m_dataCursor      = {};
        QColor m_cursorLineColor = Qt::black;
        int    m_cursorLineWidth = 1;

    public:
        BScanView();

        void                 replace(const std::vector<std::optional<uint8_t>> &data, int width, int height) noexcept;
        [[nodiscard]] int    dataCursor() const;
        void                 setDataCursor(int newDataCursor);
        void                 resetDrawLine();
        [[nodiscard]] QColor cursorLineColor() const;
        void                 setCursorLineColor(const QColor &newCursorLineColor);
        [[nodiscard]] int    cursorLineWidth() const;
        void                 setCursorLineWidth(int newCursorLineWidth);

    signals:

        void dataCursorChanged();
        void cursorLineColorChanged();
        void cursorLineWidthChanged();

    protected:
        virtual void paint(QPainter *painter) override;

        void mousePressEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *) override;

    private:
        QImage             m_image    = {};
        std::optional<int> m_drawLine = {};

        bool eventHandlerCommon(QMouseEvent *event) noexcept;

        Q_PROPERTY(int dataCursor READ dataCursor WRITE setDataCursor NOTIFY dataCursorChanged FINAL)
        Q_PROPERTY(QColor cursorLineColor READ cursorLineColor WRITE setCursorLineColor NOTIFY cursorLineColorChanged FINAL)
        Q_PROPERTY(int cursorLineWidth READ cursorLineWidth WRITE setCursorLineWidth NOTIFY cursorLineWidthChanged FINAL)
    };
} // namespace Union::View
