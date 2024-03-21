#pragma once

#include <QChartView>
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
    class TofdPeInteractor : public QQuickItem {
        Q_OBJECT
        QML_ELEMENT

        using TPE_SUB_DATA = std::optional<Union::__TOFD_PE::SubData>;
        using TPE_DATA     = std::optional<Union::__TOFD_PE::Data>;
        TPE_DATA m_data    = std::nullopt;

    public:
        TofdPeInteractor()  = default;
        ~TofdPeInteractor() = default;
        const TPE_DATA&     getData() const;
        const TPE_SUB_DATA& getSubData() const;
        Q_INVOKABLE bool    openFile(const QString& fileName);
        Q_INVOKABLE int     getLines() const;
        Q_INVOKABLE int     getSubLines() const;
    };

    class ScanView : public QQuickPaintedItem {
        Q_OBJECT
        QML_ELEMENT

        using _INTR  = TofdPeInteractor;
        using _QITEM = QQuickItem;

        QQuickItem*       m_chartView = nullptr;
        TofdPeInteractor* m_intr      = nullptr;
        bool              m_isPe      = false;
        int               m_softGain  = 0;
        int               m_cursor    = 0;

    public:
        ScanView();
        ~ScanView();

        _QITEM* chartView() const;
        void    setChartView(_QITEM* newChartView);
        _INTR*  intr() const;
        void    setIntr(_INTR* newIntr);
        bool    isPe() const;
        void    setIsPe(bool newIsPe);
        int     softGain() const;
        void    setSoftGain(int newSoftGain);
        int     cursor() const;
        void    setCursor(int newCursor);

    signals:
        void chartViewChanged();
        void intrChanged();
        void isPeChanged();
        void softGainChanged();
        void cursorChanged();

    protected:
        void paint(QPainter* painter) override;

    private:
        inline static constexpr auto AXIS_WIDTH = 35;

        QPointF m_abscissaRange = {0.0, 100.0};
        QPointF m_ordinateRange = {0.0, 100.0};

        QRect getDrawable(void) const;
        void  drawAxis(QPainter* painter) const;
        void  drawDScan(QPainter* painter) const;
        void  drawSecondDScan(QPainter* painter) const;
        void  updateChartView(void) const;

        Q_PROPERTY(QQuickItem* chartView READ chartView WRITE setChartView NOTIFY chartViewChanged FINAL)
        Q_PROPERTY(TofdPeInteractor* intr READ intr WRITE setIntr NOTIFY intrChanged FINAL)
        Q_PROPERTY(bool isPe READ isPe WRITE setIsPe NOTIFY isPeChanged FINAL)
        Q_PROPERTY(int softGain READ softGain WRITE setSoftGain NOTIFY softGainChanged FINAL)
        Q_PROPERTY(int cursor READ cursor WRITE setCursor NOTIFY cursorChanged FINAL)
    };

} // namespace TOFD_PE
