#pragma once

#include <QChartView>
#include <QJsonArray>
#include <QJsonObject>
#include <QLineSeries>
#include <QMetaObject>
#include <QQuickItem>
#include <QQuickItemGrabResult>
#include <QSlider>
#include <QTimer>
#include <UnionType>

class AScanInteractor : public QQuickItem {
    Q_OBJECT
    QML_ELEMENT
    bool        replayVisible = false;
    QString     fileName      = {};
    QString     date          = {};
    int         softGain      = {};
    int         replayValue   = {};
    QQuickItem* chartView     = nullptr;
    QJsonArray  gateValue     = {};
    int         replaySpeed   = 0;
    QString     distanceMode  = "N";

    Union::AScan::AScanType ascan          = {}; ///< A扫数据
    size_t                  aScanCurosr    = 0;
    size_t                  aScanCursorMax = 0;

    inline static constexpr auto ASCAN_SERIES_NAME = "AScan";
    inline static constexpr auto GATE_SERIES_NAME  = "Gate:%1";
    inline static constexpr auto AVG_SERIES_NAME   = "AVG:%1";
    inline static constexpr auto DAC_SERIES_NAME   = "DAC:%1";

    constexpr auto getDACSeriesSubName(int index) {
        switch (index) {
            case 0: return "BL";
            case 1: return "EL";
            case 2: return "SL";
            case 3: return "RL";
            default:
                throw std::runtime_error("index out of range");
        }
    }

    constexpr auto getAVGSeriesSubName(int index) {
        return getDACSeriesSubName(index);
    }

    bool checkAScanCursorValid();
    enum class QuadraticCurveSeriesType {
        DAC,
        AVG,
    };

public:
    AScanInteractor();
    ~AScanInteractor();

    Q_INVOKABLE void setDefaultValue();
    Q_INVOKABLE bool openFile(QString fileName);
    QAbstractSeries* createSeries(QAbstractSeries::SeriesType type, QString name, QAbstractAxis* axisX, QAbstractAxis* axisY);
    QLineSeries*     createAScanSeries(QPointF pt, QSizeF sz);
    void             updateAScanSeries(const QList<QPointF>& data, QPointF pt, QSizeF sz);
    void             updateAScanSeries(const Union::AScan::AScanData& data);
    QLineSeries*     createQuadraticCurveSeries(const QString& name, QPointF pt, QSizeF sz);
    void             updateQuadraticCurveSeries(QuadraticCurveSeriesType type);
    QLineSeries*     createGateSeries(int index = 0);
    void             updateGateSeries(Union::Base::Gate gate, int index = 0);
    template <int N = 2>
    void updateGateSeries(std::array<Union::Base::Gate, N> gate);

    QJsonArray CreateGateValue();

    void  setTimeSliderStepSize(qreal step);
    qreal getTimeSliderStepSize();
    void  setTimeSliderMax(qreal max);

    void removeAllSeries();

    QAbstractSeries* series(const QString& name);
    QAbstractSeries* series(int index);
    int              seriesCount() const;

    bool             getReplayVisible() const;
    void             setReplayVisible(bool newReplayVisible);
    QString          getFileName() const;
    void             setFileName(const QString& newFileName);
    QString          getDate() const;
    void             setDate(const QString& newDate);
    int              getSoftGain() const;
    void             setSoftGain(int newSoftGain);
    int              getReplayValue() const;
    void             setReplayValue(int newReplayValue);
    QQuickItem*      getChartView() const;
    void             setChartView(QQuickItem* newChartView);
    const QJsonArray getGateValue() const;
    void             setGateValue(const QJsonArray& newGateValue);
    size_t           getAScanCurosr() const;
    void             setAScanCurosr(size_t newAScanCurosr);
    size_t           getAScanCursorMax() const;
    void             setAScanCursorMax(size_t newAScanCursorMax);
    QString          getDistanceMode() const;
    void             setDistanceMode(const QString& newDistanceMode);

public slots:
    Q_INVOKABLE bool        reportExportClicked(QString fileName, QQuickItemGrabResult* img = nullptr);
    Q_INVOKABLE bool        performanceClicked(QString fileName);
    Q_INVOKABLE void        gainValueModified(qreal val);
    Q_INVOKABLE void        replayStartClicked(bool isStart);
    Q_INVOKABLE void        replaySpeedClicked(int val);
    Q_INVOKABLE void        lastFrameClicked(void);
    Q_INVOKABLE void        nextFrameClicked(void);
    Q_INVOKABLE void        timeSliderMoved(qreal val);
    Q_INVOKABLE void        seriesRemoved(QAbstractSeries* series);
    Q_INVOKABLE QVariantMap getTechnologicalParameter();

signals:
    void replayVisibleChanged();
    void fileNameChanged();
    void dateChanged();
    void softGainChanged();
    void replayValueChanged();
    void chartViewChanged();
    void gateValueChanged();
    void aScanCurosrChanged();
    void aScanCursorMaxChanged();
    void distanceModeChanged();

private:
    Q_PROPERTY(bool replayVisible READ getReplayVisible WRITE setReplayVisible NOTIFY replayVisibleChanged)
    Q_PROPERTY(QString fileName READ getFileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(QString date READ getDate WRITE setDate NOTIFY dateChanged)
    Q_PROPERTY(int softGain READ getSoftGain WRITE setSoftGain NOTIFY softGainChanged)
    Q_PROPERTY(int replayValue READ getReplayValue WRITE setReplayValue NOTIFY replayValueChanged)
    Q_PROPERTY(QQuickItem* chartView READ getChartView WRITE setChartView NOTIFY chartViewChanged)
    Q_PROPERTY(QJsonArray gateValue READ getGateValue WRITE setGateValue NOTIFY gateValueChanged)
    Q_PROPERTY(size_t aScanCurosr READ getAScanCurosr WRITE setAScanCurosr NOTIFY aScanCurosrChanged)
    Q_PROPERTY(size_t aScanCursorMax READ getAScanCursorMax WRITE setAScanCursorMax NOTIFY aScanCursorMaxChanged)
    Q_PROPERTY(QString distanceMode READ getDistanceMode WRITE setDistanceMode NOTIFY distanceModeChanged FINAL)
};
