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

        using TPE_SUB_DATA                   = std::optional<Union::__TOFD_PE::SubData>;
        using TPE_DATA                       = std::optional<Union::__TOFD_PE::Data>;
        TPE_DATA                m_data       = std::nullopt;
        qreal                   m_tofdSpace  = 0.0;
        qreal                   m_peSpace    = 0.0;
        std::optional<TPE_DATA> m_dataMirror = std::nullopt;

    public:
        TofdPeInteractor()  = default;
        ~TofdPeInteractor() = default;
        const TPE_DATA&     getData() const;
        const TPE_SUB_DATA& getSubData() const;
        Q_INVOKABLE bool    openFile(const QString& fileName);
        Q_INVOKABLE int     getLines() const;
        Q_INVOKABLE int     getSubLines() const;
        Q_INVOKABLE int     getMaxLines() const;
        Q_INVOKABLE bool    reportExport(QString filePath) const;
        Q_INVOKABLE qreal   getDelay() const;
        Q_INVOKABLE qreal   getRange() const;
        Q_INVOKABLE qreal   getSubRange() const;
        Q_INVOKABLE qreal   getSubAngle() const;
        Q_INVOKABLE void    throughWaveEvent(qreal x, qreal y, qreal w, qreal h, bool isPull);
        Q_INVOKABLE void    rollBack();
        qreal               tofdSpace() const;
        void                setTofdSpace(qreal newTofdSpace);
        qreal               peSpace() const;
        void                setPeSpace(qreal newPeSpace);

    signals:
        void tofdSpaceChanged();
        void peSpaceChanged();
        void updatePrivateData();

    private:
        void removeThroughWaveEvent(qreal x, qreal y, qreal w, qreal h);
        void pullThroughWaveEvent(qreal x, qreal y, qreal w, qreal h);
        Q_PROPERTY(qreal tofdSpace READ tofdSpace WRITE setTofdSpace NOTIFY tofdSpaceChanged FINAL)
        Q_PROPERTY(qreal peSpace READ peSpace WRITE setPeSpace NOTIFY peSpaceChanged FINAL)
    };
} // namespace TOFD_PE
