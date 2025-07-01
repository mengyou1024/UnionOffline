#pragma once

#include <QObject>

class AppSetting : public QObject {
    Q_OBJECT
public:
    explicit AppSetting(QObject* parent = nullptr);

    static AppSetting* Instance();

    bool calculateGateResult() const;
    void setCalculateGateResult(bool newCalculateGateResult);
    bool displayCScanLayerLine() const;
    void setDisplayCScanLayerLine(bool newDisplayCScanLayerLine);
    bool roundToInt() const;
    void setRoundToInt(bool newRoundToInt);
    bool bScanImageSmoothing() const;
    void setBScanImageSmoothing(bool newBScanImageSmoothing);
    bool cScanImageSmoothing() const;
    void setCScanImageSmoothing(bool newCScanImageSmoothing);

signals:
    void calculateGateResultChanged();
    void displayCScanLayerLineChanged();
    void roundToIntChanged();
    void bScanImageSmoothingChanged();
    void cScanImageSmoothingChanged();

private:
    bool m_calculateGateResult   = true;
    bool m_displayCScanLayerLine = true;
    bool m_roundToInt            = true;
    bool m_bScanImageSmoothing   = true;
    bool m_cScanImageSmoothing   = true;

    static AppSetting _instance;

    Q_PROPERTY(bool calculateGateResult READ calculateGateResult WRITE setCalculateGateResult NOTIFY calculateGateResultChanged FINAL)
    Q_PROPERTY(bool displayCScanLayerLine READ displayCScanLayerLine WRITE setDisplayCScanLayerLine NOTIFY displayCScanLayerLineChanged FINAL)
    Q_PROPERTY(bool roundToInt READ roundToInt WRITE setRoundToInt NOTIFY roundToIntChanged FINAL)
    Q_PROPERTY(bool bScanImageSmoothing READ bScanImageSmoothing WRITE setBScanImageSmoothing NOTIFY bScanImageSmoothingChanged FINAL)
    Q_PROPERTY(bool cScanImageSmoothing READ cScanImageSmoothing WRITE setCScanImageSmoothing NOTIFY cScanImageSmoothingChanged FINAL)
};
