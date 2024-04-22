#pragma once

#include "./TofdPeAScanView.hpp"
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
    class TofdPeDScanView : public QQuickPaintedItem {
        Q_OBJECT
        QML_ELEMENT

        using _INTR                       = TofdPeInteractor;
        using _QITEM                      = QQuickItem;
        TofdPeAScanView*  m_aScanView     = nullptr;
        TofdPeInteractor* m_intr          = nullptr;
        bool              m_isPe          = false;
        int               m_softGain      = 0;
        int               m_cursor        = 0;
        QPointF           m_abscissaRange = {0.0, 100.0};
        QPointF           m_ordinateRange = {0.0, 100.0};

    public:
        TofdPeDScanView();
        ~TofdPeDScanView();

        _INTR*            intr() const;
        void              setIntr(_INTR* newIntr);
        bool              isPe() const;
        void              setIsPe(bool newIsPe);
        int               softGain() const;
        void              setSoftGain(int newSoftGain);
        int               cursor() const;
        void              setCursor(int newCursor);
        Q_INVOKABLE QRect getDrawable(void) const;
        Q_INVOKABLE void  updateAScan(void) const;
        QPointF           abscissaRange() const;
        void              setAbscissaRange(QPointF newAbscissaRange);
        QPointF           ordinateRange() const;
        void              setOrdinateRange(QPointF newOrdinateRange);

        TofdPeAScanView* aScanView() const;
        void             setAScanView(TofdPeAScanView* newAScanView);

    signals:
        void intrChanged();
        void isPeChanged();
        void softGainChanged();
        void cursorChanged();
        void abscissaRangeChanged();
        void ordinateRangeChanged();

        void aScanViewChanged();

    protected:
        void paint(QPainter* painter) override;

    private:
        inline static constexpr auto AXIS_WIDTH = 35;
        void                         drawAxis(QPainter* painter) const;
        void                         drawDScan(QPainter* painter) const;
        void                         drawSecondDScan(QPainter* painter) const;

        Q_PROPERTY(TofdPeInteractor* intr READ intr WRITE setIntr NOTIFY intrChanged FINAL)
        Q_PROPERTY(bool isPe READ isPe WRITE setIsPe NOTIFY isPeChanged FINAL)
        Q_PROPERTY(int softGain READ softGain WRITE setSoftGain NOTIFY softGainChanged FINAL)
        Q_PROPERTY(int cursor READ cursor WRITE setCursor NOTIFY cursorChanged FINAL)
        Q_PROPERTY(QPointF abscissaRange READ abscissaRange WRITE setAbscissaRange NOTIFY abscissaRangeChanged FINAL)
        Q_PROPERTY(QPointF ordinateRange READ ordinateRange WRITE setOrdinateRange NOTIFY ordinateRangeChanged FINAL)
        Q_PROPERTY(TofdPeAScanView* aScanView READ aScanView WRITE setAScanView NOTIFY aScanViewChanged FINAL)
    };

} // namespace TOFD_PE
