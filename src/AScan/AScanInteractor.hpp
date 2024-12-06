#pragma once

#include <QAbstractSeries>
#include <QChartView>
#include <QJsonArray>
#include <QJsonObject>
#include <QLineSeries>
#include <QMetaObject>
#include <QQuickItem>
#include <QQuickItemGrabResult>
#include <QSlider>
#include <QTimer>
#include <QtCharts>
#include <UnionType>
#include <chrono>

class AScanInteractor : public QQuickItem {
    Q_OBJECT
    QML_ELEMENT

    using _STD_TP = std::chrono::system_clock::time_point;

    bool        replayVisible             = false;
    QString     date                      = {};
    int         softGain                  = {};
    int         replayValue               = {};
    QQuickItem* chartView                 = nullptr;
    QJsonArray  gateValue                 = {};
    int         replaySpeed               = 0;
    QString     distanceMode              = "N";
    bool        hasCameraImage            = false;
    bool        showRailWeldDigramSpecial = false;
    int         replayTimerInterval       = 0;
    bool        reportEnabled             = true;
    bool        dateEnabled               = true;
    bool        m_isPlaying               = false;
    bool        showCMP001Special         = false;
    _STD_TP     m_lastUpdateGateValueTime = std::chrono::system_clock::now();

    using ASCAN_TYPE = std::shared_ptr<Union::AScan::AScanIntf>;

    ASCAN_TYPE m_aScanIntf    = {}; ///< A扫数据
    int        aScanCursor    = 0;
    int        aScanCursorMax = 0;

    inline static constexpr auto ASCAN_SERIES_NAME = "AScan";
    inline static constexpr auto GATE_SERIES_NAME  = "Gate:%1";
    inline static constexpr auto AVG_SERIES_NAME   = "AVG:%1";
    inline static constexpr auto DAC_SERIES_NAME   = "DAC:%1";

    inline static constexpr auto DAC_AVG_SUB_NAME_MAX = 7;

    constexpr auto getDACSeriesSubName(int index) {
        switch (index) {
            case 0: return "BL";
            case 1: return "EL";
            case 2: return "SL";
            case 3: return "RL";
            case 4: return "L4";
            case 5: return "L5";
            case 6: return "L6";
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

    [[deprecated("该函数需要重构, 向标准格式转换")]]
    void update330N_DAC_AVG_Series();

    void CMP000Special_UpdateDacSeries();

public:
    AScanInteractor();
    ~AScanInteractor();

    Q_INVOKABLE void setDefaultValue();
    Q_INVOKABLE bool openFile(QString fileName);
    QAbstractSeries* createSeries(QAbstractSeries::SeriesType type, QString name, QAbstractAxis* axisX, QAbstractAxis* axisY);
    QLineSeries*     createAScanSeries(QPointF pt, QSizeF sz);
    void             updateAScanSeries(const QList<QPointF>& data, QPointF pt, QSizeF sz);
    void             updateAScanSeries(void);
    QLineSeries*     createQuadraticCurveSeries(const QString& name, QPointF pt = {0.0, 0.0}, QSizeF sz = {100.0, 100.0});
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
    int              getAScanCursor() const;
    void             setAScanCursor(int newAScanCursor);
    int              getAScanCursorMax() const;
    void             setAScanCursorMax(int newAScanCursorMax);
    QString          getDistanceMode() const;
    void             setDistanceMode(const QString& newDistanceMode);

    Q_INVOKABLE QImage       getCameraImage(void) const;
    Q_INVOKABLE QVariantList getRailWeldDot(void) const;
    Q_INVOKABLE bool         isGateEnable(int gate_idx) const;

    bool       getHasCameraImage() const;
    void       setHasCameraImage(bool newHasCameraImage);
    bool       getShowRailWeldDigramSpecial() const;
    void       setShowRailWeldDigramSpecial(bool newShowRailWeldDigramSpecial);
    int        getReplayTimerInterval() const;
    void       setReplayTimerInterval(int newReplayTimerInterval);
    bool       getReportEnabled() const;
    void       setReportEnabled(bool newReportEnabled);
    bool       getDateEnabled() const;
    void       setDateEnabled(bool newDateEnabled);
    bool       getShowCMP001Special() const;
    void       setShowCMP001Special(bool newShowCMP001Special);
    ASCAN_TYPE aScanIntf() const;
    void       setAScanIntf(const ASCAN_TYPE& newAScanIntf);

public slots:
    Q_INVOKABLE bool         reportExportClicked(QString fileName, QQuickItemGrabResult* img = nullptr);
    Q_INVOKABLE bool         performanceClicked(QString fileName);
    Q_INVOKABLE void         gainValueModified(qreal val);
    Q_INVOKABLE void         replayStartClicked(bool isStart);
    Q_INVOKABLE void         replaySpeedClicked(int val);
    Q_INVOKABLE void         lastFrameClicked(void);
    Q_INVOKABLE void         nextFrameClicked(void);
    Q_INVOKABLE void         timeSliderMoved(qreal val);
    Q_INVOKABLE void         seriesRemoved(QAbstractSeries* series);
    Q_INVOKABLE QVariantMap  getTechnologicalParameter();
    Q_INVOKABLE QVariantList getFileNameList(void);
    Q_INVOKABLE void         setFileNameIndex(int idx);
    Q_INVOKABLE bool         railWeldSpecial_ZeroPointInFoot();

signals:
    void replayVisibleChanged();
    void dateChanged();
    void softGainChanged();
    void replayValueChanged();
    void chartViewChanged();
    void gateValueChanged();
    void aScanCursorChanged();
    void aScanCursorMaxChanged();
    void distanceModeChanged();
    void hasCameraImageChanged();
    void showRailWeldDigramSpecialChanged();
    void replayTimerIntervalChanged();
    void reportEnabledChanged();
    void dateEnabledChanged();
    void showCMP001SpecialChanged();
    void aScanIntfChanged();

private:
    void changeDataCursor(void);
    void updateCurrentFrame(void);

    Q_PROPERTY(bool replayVisible READ getReplayVisible WRITE setReplayVisible NOTIFY replayVisibleChanged)
    Q_PROPERTY(QString date READ getDate WRITE setDate NOTIFY dateChanged)
    Q_PROPERTY(int softGain READ getSoftGain WRITE setSoftGain NOTIFY softGainChanged)
    Q_PROPERTY(int replayValue READ getReplayValue WRITE setReplayValue NOTIFY replayValueChanged)
    Q_PROPERTY(QQuickItem* chartView READ getChartView WRITE setChartView NOTIFY chartViewChanged)
    Q_PROPERTY(QJsonArray gateValue READ getGateValue WRITE setGateValue NOTIFY gateValueChanged)
    Q_PROPERTY(int aScanCursor READ getAScanCursor WRITE setAScanCursor NOTIFY aScanCursorChanged)
    Q_PROPERTY(int aScanCursorMax READ getAScanCursorMax WRITE setAScanCursorMax NOTIFY aScanCursorMaxChanged)
    Q_PROPERTY(QString distanceMode READ getDistanceMode WRITE setDistanceMode NOTIFY distanceModeChanged FINAL)
    Q_PROPERTY(bool hasCameraImage READ getHasCameraImage WRITE setHasCameraImage NOTIFY hasCameraImageChanged FINAL)
    Q_PROPERTY(bool showRailWeldDigramSpecial READ getShowRailWeldDigramSpecial WRITE setShowRailWeldDigramSpecial NOTIFY showRailWeldDigramSpecialChanged FINAL)
    Q_PROPERTY(int replayTimerInterval READ getReplayTimerInterval WRITE setReplayTimerInterval NOTIFY replayTimerIntervalChanged FINAL)
    Q_PROPERTY(bool reportEnabled READ getReportEnabled WRITE setReportEnabled NOTIFY reportEnabledChanged FINAL)
    Q_PROPERTY(bool dateEnabled READ getDateEnabled WRITE setDateEnabled NOTIFY dateEnabledChanged FINAL)
    Q_PROPERTY(bool showCMP001Special READ getShowCMP001Special WRITE setShowCMP001Special NOTIFY showCMP001SpecialChanged FINAL)
    Q_PROPERTY(std::shared_ptr<Union::AScan::AScanIntf> aScanIntf READ aScanIntf WRITE setAScanIntf NOTIFY aScanIntfChanged FINAL)
};
