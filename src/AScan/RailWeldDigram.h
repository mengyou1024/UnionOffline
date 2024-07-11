#pragma once

#include <QQuickPaintedItem>

namespace Union::AScan::RailWeld {

    class RailWeldDigram : public QQuickPaintedItem {
        Q_OBJECT
        QML_ELEMENT
    private:
        int  m_dotX            = 0;
        int  m_dotY            = 0;
        int  m_dotZ            = 0;
        bool m_zeroPointInFoot = false;

        Q_PROPERTY(int dotX READ dotX WRITE setDotX NOTIFY dotXChanged FINAL)
        Q_PROPERTY(int dotY READ dotY WRITE setDotY NOTIFY dotYChanged FINAL)
        Q_PROPERTY(int dotZ READ dotZ WRITE setDotZ NOTIFY dotZChanged FINAL)

        Q_PROPERTY(bool zeroPointInFoot READ zeroPointInFoot WRITE setZeroPointInFoot NOTIFY zeroPointInFootChanged FINAL)

    public:
        RailWeldDigram();

        int  dotX() const;
        void setDotX(int newDotX);
        int  dotY() const;
        void setDotY(int newDotY);
        int  dotZ() const;
        void setDotZ(int newDotZ);

        bool zeroPointInFoot() const;
        void setZeroPointInFoot(bool newZeroPointInFoot);

    signals:
        void dotXChanged();
        void dotYChanged();
        void dotZChanged();

        void zeroPointInFootChanged();

    protected:
        void paint(QPainter* painter) override;
        void paintDigrameA(QPainter* painter) const;
        void paintDigrameB(QPainter* painter) const;
    };
} // namespace Union::AScan::RailWeld
