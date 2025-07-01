#include "AppSetting.hpp"

AppSetting AppSetting::_instance = AppSetting();

AppSetting::AppSetting(QObject* parent) :
QObject{parent} {}

AppSetting* AppSetting::Instance() {
    return &_instance;
}

bool AppSetting::calculateGateResult() const {
    return m_calculateGateResult;
}

void AppSetting::setCalculateGateResult(bool newCalculateGateResult) {
    if (m_calculateGateResult == newCalculateGateResult)
        return;
    m_calculateGateResult = newCalculateGateResult;
    emit calculateGateResultChanged();
}

bool AppSetting::displayCScanLayerLine() const {
    return m_displayCScanLayerLine;
}

void AppSetting::setDisplayCScanLayerLine(bool newDisplayCScanLayerLine) {
    if (m_displayCScanLayerLine == newDisplayCScanLayerLine)
        return;
    m_displayCScanLayerLine = newDisplayCScanLayerLine;
    emit displayCScanLayerLineChanged();
}

bool AppSetting::roundToInt() const {
    return m_roundToInt;
}

void AppSetting::setRoundToInt(bool newRoundToInt) {
    if (m_roundToInt == newRoundToInt)
        return;
    m_roundToInt = newRoundToInt;
    emit roundToIntChanged();
}

bool AppSetting::bScanImageSmoothing() const {
    return m_bScanImageSmoothing;
}

void AppSetting::setBScanImageSmoothing(bool newBScanImageSmoothing) {
    if (m_bScanImageSmoothing == newBScanImageSmoothing)
        return;
    m_bScanImageSmoothing = newBScanImageSmoothing;
    emit bScanImageSmoothingChanged();
}

bool AppSetting::cScanImageSmoothing() const {
    return m_cScanImageSmoothing;
}

void AppSetting::setCScanImageSmoothing(bool newCScanImageSmoothing) {
    if (m_cScanImageSmoothing == newCScanImageSmoothing)
        return;
    m_cScanImageSmoothing = newCScanImageSmoothing;
    emit cScanImageSmoothingChanged();
}
