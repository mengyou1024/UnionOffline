#pragma once
#include "BasicView.hpp"

namespace Union::View {
    class AScanView : public BasicView {
        Q_OBJECT
        QML_ELEMENT
    public:
        AScanView();

        QColor lineColor() const;
        void   setLineColor(const QColor &newLineColor);
        void   replace(const std::vector<uint8_t> &data) noexcept;
        void   hook(const std::shared_ptr<const std::vector<uint8_t>> &data) noexcept;

    signals:
        void lineColorChanged();

    protected:
        virtual void paint(QPainter *painter) override;

    private:
        QColor                                      m_lineColor = {255, 255, 0}; ///< A扫线条颜色
        std::shared_ptr<const std::vector<uint8_t>> m_lineData  = nullptr;       ///< A扫数据指针

        QVector<QPointF> getLines() const;

        Q_PROPERTY(QColor lineColor READ lineColor WRITE setLineColor NOTIFY lineColorChanged FINAL)
    };
} // namespace Union::View
