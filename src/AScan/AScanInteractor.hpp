#pragma once

#include <BScanView.hpp>
#include <CScanView.hpp>
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

    using _STD_TP    = std::chrono::system_clock::time_point;
    using ASCAN_TYPE = std::shared_ptr<::Union::UniversalApparatus::AScan::AScanIntf>;

    bool        m_replayVisible             = false;
    QString     m_date                      = {};
    int         m_softGain                  = {};
    int         m_replayValue               = {};
    QQuickItem* m_chartView                 = nullptr;
    QJsonArray  m_gateValue                 = {};
    int         m_aScanCursor               = 0;
    int         m_aScanCursorMax            = 0;
    QString     m_distanceMode              = "N";
    bool        m_hasCameraImage            = false;
    bool        m_showRailWeldDigramSpecial = false;
    int         m_replayTimerInterval       = 0;
    bool        m_reportEnabled             = true;
    bool        m_dateEnabled               = true;
    bool        m_showCMP001Special         = false;
    ASCAN_TYPE  m_aScanIntf                 = {}; ///< A扫接口
    bool        m_showBScanView             = false;
    bool        m_showCScanView             = false;
    QQuickItem* m_scanViewHandler           = nullptr;
    bool        m_softGainEnable            = false;
    QQuickItem* m_scanViewHandlerExtra      = nullptr;

    int                                     m_replaySpeed             = 0;
    bool                                    m_isPlaying               = false;
    _STD_TP                                 m_lastUpdateGateValueTime = std::chrono::system_clock::now();
    std::shared_ptr<Union::View::BasicView> m_scanViewSp              = nullptr;
    std::shared_ptr<Union::View::BasicView> m_scanViewSpExtra         = nullptr;

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

public:
    AScanInteractor();
    ~AScanInteractor();

    Q_INVOKABLE void setDefaultValue();
    Q_INVOKABLE bool openFile(QString filename);
    QAbstractSeries* createSeries(QAbstractSeries::SeriesType type, QString name, QAbstractAxis* axisX, QAbstractAxis* axisY);
    QLineSeries*     createAScanSeries(QPointF pt, QSizeF sz);
    void             updateAScanSeries(const QList<QPointF>& data, QPointF pt, QSizeF sz);
    void             updateAScanSeries(void);
    QLineSeries*     createQuadraticCurveSeries(const QString& name, QPointF pt = {0.0, 0.0}, QSizeF sz = {100.0, 100.0});
    void             updateQuadraticCurveSeries(QuadraticCurveSeriesType type);
    QLineSeries*     createGateSeries(int index = 0);
    void             updateGateSeries(::Union::BasicType::Gate gate, int index = 0);
    void             updateGateSeries(std::array<::Union::BasicType::Gate, 2> gate);

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

    bool        getHasCameraImage() const;
    void        setHasCameraImage(bool newHasCameraImage);
    bool        getShowRailWeldDigramSpecial() const;
    void        setShowRailWeldDigramSpecial(bool newShowRailWeldDigramSpecial);
    int         getReplayTimerInterval() const;
    void        setReplayTimerInterval(int newReplayTimerInterval);
    bool        getReportEnabled() const;
    void        setReportEnabled(bool newReportEnabled);
    bool        getDateEnabled() const;
    void        setDateEnabled(bool newDateEnabled);
    bool        getShowCMP001Special() const;
    void        setShowCMP001Special(bool newShowCMP001Special);
    ASCAN_TYPE  aScanIntf() const;
    void        setAScanIntf(const ASCAN_TYPE& newAScanIntf);
    bool        showBScanView() const;
    void        setShowBScanView(bool newShowBScanView);
    bool        showCScanView() const;
    void        setShowCScanView(bool newShowCScanView);
    QQuickItem* scanViewHandler() const;
    void        setScanViewHandler(QQuickItem* newScanViewHandler);
    bool        softGainEnable() const;
    void        setSoftGainEnable(bool newSoftGainEnable);
    QQuickItem* scanViewHandlerExtra() const;
    void        setScanViewHandlerExtra(QQuickItem* newScanViewHandlerExtra);

public slots:
    bool         reportExportClicked(QString fileName, QQuickItemGrabResult* img = nullptr);
    bool         performanceClicked(QString fileName);
    void         gainValueModified(qreal val);
    void         replayStartClicked(bool isStart);
    void         replaySpeedClicked(int val);
    void         lastFrameClicked(void);
    void         nextFrameClicked(void);
    void         timeSliderMoved(qreal val);
    void         seriesRemoved(QAbstractSeries* series);
    QVariantMap  getTechnologicalParameter();
    QVariantList getFileNameList(void);
    void         setFileNameIndex(int idx);
    bool         railWeldSpecial_ZeroPointInFoot();

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
    void showBScanViewChanged();
    void showCScanViewChanged();
    void scanViewHandlerChanged();
    void softGainEnableChanged();
    void updateBOrCScanHandler();
    void scanViewHandlerExtraChanged();
    void updateBScanExtraHandler();

private:
    void changeDataCursor(void);
    void updateCurrentFrame(void);
    void updateBOrCScanView(bool set_size);
    void updateBOrCScanViewRange(void);
    void updateExtraBScanView(bool set_size);
    void updateExtraBScanViewRange(void);

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
    Q_PROPERTY(std::shared_ptr<::Union::UniversalApparatus::AScan::AScanIntf> aScanIntf READ aScanIntf WRITE setAScanIntf NOTIFY aScanIntfChanged FINAL)
    Q_PROPERTY(bool showBScanView READ showBScanView WRITE setShowBScanView NOTIFY showBScanViewChanged FINAL)
    Q_PROPERTY(bool showCScanView READ showCScanView WRITE setShowCScanView NOTIFY showCScanViewChanged FINAL)
    Q_PROPERTY(QQuickItem* scanViewHandler READ scanViewHandler WRITE setScanViewHandler NOTIFY scanViewHandlerChanged FINAL)
    Q_PROPERTY(bool softGainEnable READ softGainEnable WRITE setSoftGainEnable NOTIFY softGainEnableChanged FINAL)
    Q_PROPERTY(QQuickItem* scanViewHandlerExtra READ scanViewHandlerExtra WRITE setScanViewHandlerExtra NOTIFY scanViewHandlerExtraChanged FINAL)
};
