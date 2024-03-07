import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtCharts
import Union.Interactor

Rectangle {
    LoggingCategory {
        id: category
        name: "AScan.main"
    }

    clip: true
    color: "transparent"
    property alias controlTarget: cons.target
    property alias mainTarget: main_cons.target

    property alias replayVisible: interactor.replayVisible
    property alias fileName: interactor.fileName
    property alias date: interactor.date
    property alias cursorMax: interactor.aScanCursorMax

    readonly property var gateText: [{
            "amp": "A%:",
            "dist_a": "A→:",
            "dist_b": "A↓:",
            "dist_c": "A↘:",
            "equi": qsTr("当量:")
        }, {
            "amp": "B%:",
            "dist_a": "B→:",
            "dist_b": "B↓:",
            "dist_c": "B↘:",
            "equi": qsTr("当量:")
        }]

    readonly property var gateTextColor: ["red", "#8470ff"]

    ColumnLayout {
        anchors.fill: parent
        spacing: 10
        RowLayout {
            Layout.fillWidth: true
            spacing: 20
            Layout.alignment: Qt.AlignHCenter
            Text {
                Layout.leftMargin: 20
                width: 20
                text: `声程 (${interactor.distanceMode})`
                font.pixelSize: 20
            }
            ColumnLayout {
                Layout.topMargin: 20
                Layout.fillWidth: true
                spacing: 20
                Repeater {
                    model: 2
                    delegate: RowLayout {
                        property int gateIndex: index
                        Layout.alignment: Qt.AlignCenter
                        property var rowColor: gateTextColor[index]
                        Repeater {
                            model: ["amp", "dist_c", "dist_a", "dist_b", "equi"]
                            delegate: Rectangle {
                                Layout.alignment: Qt.AlignCenter
                                width: 200
                                height: 25
                                color: "transparent"
                                RowLayout {
                                    Label {
                                        Layout.preferredWidth: 40
                                        Layout.preferredHeight: 25
                                        background: Rectangle {
                                            color: "transparent"
                                        }
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                        text: gateText[gateIndex][modelData]
                                        font.pixelSize: 16
                                        color: rowColor
                                    }
                                    Label {
                                        background: Rectangle {
                                            border.color: "#d8d8d8"
                                            border.width: 1
                                        }
                                        Layout.preferredWidth: 130
                                        Layout.preferredHeight: 25
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                        text: interactor.gateValue[gateIndex][modelData]
                                        font.pixelSize: 16
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        ChartView {
            id: chart_view
            Layout.fillWidth: true
            Layout.fillHeight: true
            antialiasing: true
            legend.visible: false
        }
    }

    AScanInteractor {
        id: interactor
        softGain: cons.target.softGain ? cons.target.softGain : 0
        replayValue: cons.target.replayValue ? cons.target.replayValue : 0
        chartView: chart_view
        Binding {
            target: controlTarget
            property: "replayVisible"
            value: interactor.replayVisible
        }

        Binding {
            target: controlTarget
            property: "fileName"
            value: interactor.fileName
        }

        Binding {
            target: controlTarget
            property: "date"
            value: interactor.date
        }

        Binding {
            target: controlTarget
            property: "cursorMax"
            value: interactor.aScanCursorMax
        }
    }

    Connections {
        id: cons
        ignoreUnknownSignals: true

        function onReportExportClicked(fileName) {
            if (interactor.reportExportClicked(fileName)) {
                showSuccessful(qsTr("保存成功"))
            } else {
                showFailed(qsTr("保存失败"))
            }
        }

        function onPerformanceClicked(fileName) {
            if (interactor.performanceClicked(fileName)) {
                showSuccessful(qsTr("保存成功"))
            } else {
                showFailed(qsTr("保存失败"))
            }
        }

        function onGainValueModified(val) {
            interactor.gainValueModified(val)
        }

        function onReplayStartClicked(isStart) {
            interactor.replayStartClicked(isStart)
        }

        function onReplaySpeedClicked(val) {
            interactor.replaySpeedClicked(val)
        }

        function onLastFrameClicked() {
            interactor.lastFrameClicked()
        }

        function onNextFrameClicked() {
            interactor.nextFrameClicked()
        }

        function onTimeSliderMoved(val) {
            interactor.timeSliderMoved(val)
        }
    }

    Connections {
        id: main_cons
        ignoreUnknownSignals: true
        function onBtnParamClicked() {
            var comp = Qt.createComponent("ParamUI.qml")
            if (comp.status === Component.Ready) {
                // DONE: 获取参数
                const params = interactor.getTechnologicalParameter()
                console.log(category, params)
                var params_wnd = comp.createObject(parent, {
                                                       "params": params
                                                   })
                params_wnd.closing.connect(() => {
                                               comp.destroy()
                                               params_wnd.destroy()
                                           })
            } else if (comp.status === Component.Error) {
                console.error(category, comp.errorString())
            }
        }

        function onListviewIndexChanged(index, list_view) {
            console.log(category, "listview index changed, index:", index, "filepath:", list_view.get(index, "filePath"))
            controlTarget.init()
            interactor.setDefaultValue()
            if (interactor.openFile(list_view.get(index, "filePath"))) {
                showSuccessful(qsTr("打开成功"))
            } else {
                showFailed(qsTr("打开失败"))
            }
        }

        function onOpenFile(file) {
            console.log(category, "open file:", file)
            controlTarget.init()
            interactor.setDefaultValue()
            if (interactor.openFile(file)) {
                showSuccessful(qsTr("打开成功"))
            } else {
                showFailed(qsTr("打开失败"))
            }
        }
    }
}
