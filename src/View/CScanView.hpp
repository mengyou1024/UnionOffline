#pragma once

#include <IScanView.hpp>
#include <QQuickPaintedItem>

namespace Union::View {
    class CScanView : public IScanView {
    public:
    private:
        Q_OBJECT
        QML_ELEMENT

        int m_dataCursor = {};

    public:
        CScanView();
        ~CScanView() override;

        void replace(const std::vector<std::optional<uint8_t>>& data, int width, int height, bool set_size = true) noexcept;

        int  dataCursor() const;
        void setDataCursor(int newDataCursor);

        Q_INVOKABLE int toIntDataCursor(QPoint pt);

    signals:

        void dataCursorChanged();

    protected:
        virtual void paint(QPainter* painter) override;
        virtual void draw_image(const QImage& img, QPainter* painter) override;

    private:
        int _raw_width  = {};
        int _raw_height = {};
        Q_PROPERTY(int dataCursor READ dataCursor WRITE setDataCursor NOTIFY dataCursorChanged FINAL)
    };
} // namespace Union::View
