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
        int    m_imgWidth        = 0;
        int    m_imgHeight       = 0;
        bool   m_enableCursor    = true;

    public:
        BScanView();

        void                 replace(const std::vector<std::optional<uint8_t>>& data, int width, int height, bool set_size = true) noexcept;
        [[nodiscard]] int    dataCursor() const;
        void                 setDataCursor(int newDataCursor);
        void                 resetDrawLine();
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

    signals:

        void dataCursorChanged();
        void cursorLineColorChanged();
        void cursorLineWidthChanged();
        void imgWidthChanged();
        void imgHeightChanged();
        void enableCursorChanged();

    protected:
        virtual void paint(QPainter* painter) override;

        void mousePressEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent*) override;

    private:
        QImage             m_image    = {};
        std::optional<int> m_drawLine = {};

        bool eventHandlerCommon(QMouseEvent* event) noexcept;

        Q_PROPERTY(int dataCursor READ dataCursor WRITE setDataCursor NOTIFY dataCursorChanged FINAL)
        Q_PROPERTY(QColor cursorLineColor READ cursorLineColor WRITE setCursorLineColor NOTIFY cursorLineColorChanged FINAL)
        Q_PROPERTY(int cursorLineWidth READ cursorLineWidth WRITE setCursorLineWidth NOTIFY cursorLineWidthChanged FINAL)
        Q_PROPERTY(int imgWidth READ imgWidth WRITE setImgWidth NOTIFY imgWidthChanged FINAL)
        Q_PROPERTY(int imgHeight READ imgHeight WRITE setImgHeight NOTIFY imgHeightChanged FINAL)
        Q_PROPERTY(bool enableCursor READ enableCursor WRITE setEnableCursor NOTIFY enableCursorChanged FINAL)
    };
} // namespace Union::View
