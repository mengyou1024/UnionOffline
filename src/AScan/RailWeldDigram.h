#pragma once

#include <QQuickPaintedItem>

namespace Union::AScan::RailWeld {

    class RailWeldDigram : public QQuickPaintedItem {
        Q_OBJECT
        QML_ELEMENT
    private:
        int m_dotX = 10;
        int m_dotY = 20;
        int m_dotZ = 30;

        Q_PROPERTY(int dotX READ dotX WRITE setDotX NOTIFY dotXChanged FINAL)
        Q_PROPERTY(int dotY READ dotY WRITE setDotY NOTIFY dotYChanged FINAL)
        Q_PROPERTY(int dotZ READ dotZ WRITE setDotZ NOTIFY dotZChanged FINAL)

    public:
        RailWeldDigram();

        int  dotX() const;
        void setDotX(int newDotX);

        int  dotY() const;
        void setDotY(int newDotY);

        int  dotZ() const;
        void setDotZ(int newDotZ);

    signals:
        void dotXChanged();
        void dotYChanged();
        void dotZChanged();

    protected:
        void paint(QPainter* painter) override;
        void paintDigrameA(QPainter* painter) const;
        void paintDigrameB(QPainter* painter) const;
    };
} // namespace Union::AScan::RailWeld
