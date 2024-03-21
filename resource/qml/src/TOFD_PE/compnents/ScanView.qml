import QtQuick
import QtQuick.Layouts
import QtCharts
import Union.TOFD_PE

Item {
    property alias intr: _view.intr
    property alias isPe: _view.isPe
    property alias softGain: _view.softGain

    function update() {
        _view.update()
    }

    GridLayout {
        anchors.fill: parent
        rows: 2
        columns: 2
        columnSpacing: 10
        rowSpacing: 0
        ChartView {
            id: cv_ascan
            Layout.preferredWidth: parent.width / 5
            Layout.fillHeight: true
            antialiasing: false
            legend.visible: false
            dropShadowEnabled: true
            backgroundRoundness: 0
            plotAreaColor: "black"
            margins.left: 0
            margins.right: 0
            margins.top: 0
            margins.bottom: 0
            plotArea: Qt.rect(0, 0, width, height)
        }

        TofdPeScanView {
            id: _view
            chartView: cv_ascan
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.rowSpan: 2
        }

        Rectangle {
            color: "transparent"
            height: 35
        }
    }
    Component.onCompleted: {
        console.log(category, cv_ascan.plotArea)
    }
}
