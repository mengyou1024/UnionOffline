#pragma once

#include "./TofdPeInteractor.hpp"
#include <QJsonArray>
#include <QJsonObject>
#include <QLineSeries>
#include <QMetaObject>
#include <QQuickItem>
#include <QQuickItemGrabResult>
#include <QQuickPaintedItem>
#include <QSlider>
#include <QTimer>
#include <UnionType>

namespace TOFD_PE {
    class TofdPeAScanView : public QQuickPaintedItem {
        Q_OBJECT
        QML_ELEMENT

        using _INTR  = TofdPeInteractor;
        using _QITEM = QQuickItem;

        std::vector<uint8_t> m_data = {};

    public:
        TofdPeAScanView()  = default;
        ~TofdPeAScanView() = default;

        void replace(const std::vector<uint8_t>& v);

    protected:
        void paint(QPainter* painter) override;

    private:
        void drawGridLines(QPainter* painter);
        void drawAScanLine(QPainter* painter);
    };

} // namespace TOFD_PE
