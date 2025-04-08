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
#include <memory>
#include <vector>

namespace TofdPe {
    class TofdPeInteractor : public QQuickItem {
        Q_OBJECT
        QML_ELEMENT

        using DATA_PTR        = std::unique_ptr<Union::UniversalApparatus::TofdPe::TofdPeIntf>;
        using ADJUST_FUNC     = std::optional<std::function<double(double)>>;
        using VAXIS_ZERO_FUNC = std::optional<std::function<double(void)>>;

        DATA_PTR        m_data                = nullptr;
        qreal           m_tofdSpace           = 0.0;
        qreal           m_peSpace             = 0.0;
        QString         m_file                = "";
        ADJUST_FUNC     m_adjsutDepthFunc     = std::nullopt;
        VAXIS_ZERO_FUNC m_getVerticalAixsZero = std::nullopt;
        QString         m_fileName            = "";

    public:
        TofdPeInteractor()  = default;
        ~TofdPeInteractor() = default;
        Q_INVOKABLE bool              openFile(const QString& filename);
        Q_INVOKABLE int               getAScanSize() const;
        Q_INVOKABLE int               getLines() const;
        Q_INVOKABLE int               getSubLines() const;
        Q_INVOKABLE int               getMaxLines() const;
        Q_INVOKABLE bool              reportExport(QString filePath, QQuickItemGrabResult* img = nullptr) const;
        const std::vector<uint8_t>&   getTofdData() const;
        const std::vector<uint8_t>&   getPeData() const;
        Q_INVOKABLE qreal             getDelay() const;
        Q_INVOKABLE qreal             getRange() const;
        Q_INVOKABLE qreal             getSubRange() const;
        Q_INVOKABLE qreal             getSubAngle() const;
        Q_INVOKABLE void              throughWaveEvent(qreal x, qreal y, qreal w, qreal h, bool isPull);
        Q_INVOKABLE void              adjustDepth(qreal depth);
        Q_INVOKABLE void              rollBack();
        qreal                         tofdSpace() const;
        void                          setTofdSpace(qreal newTofdSpace);
        qreal                         peSpace() const;
        void                          setPeSpace(qreal newPeSpace);
        Q_INVOKABLE bool              hasPe() const;
        Q_INVOKABLE const QJsonObject getTofdParam() const;
        Q_INVOKABLE const QJsonObject getPeParam() const;
        Q_INVOKABLE double            getTofdDepth(double val) const;
        double                        getVerticalAxisZeroPoint() const;

        QString fileName() const;
        void    setFileName(const QString& newFileName);

    signals:
        void tofdSpaceChanged();
        void peSpaceChanged();
        void updatePrivateData();

        void fileNameChanged();

    private:
        void removeThroughWaveEvent(qreal x, qreal y, qreal w, qreal h);
        void pullThroughWaveEvent(qreal x, qreal y, qreal w, qreal h);
        Q_PROPERTY(qreal tofdSpace READ tofdSpace WRITE setTofdSpace NOTIFY tofdSpaceChanged FINAL)
        Q_PROPERTY(qreal peSpace READ peSpace WRITE setPeSpace NOTIFY peSpaceChanged FINAL)
        Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged FINAL)
    };
} // namespace TofdPe
