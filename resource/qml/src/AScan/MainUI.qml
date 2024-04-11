import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtCharts 2.15
import Union.Interactor 1.0

Rectangle {
    LoggingCategory {
        id: category
        name: "AScan.main"
    }

    clip: true
    color: "transparent"
    property alias controlTarget: cons.target
    property alias mainTarget: main_cons.target

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
                text: qsTr("声程") + `(${interactor.distanceMode})`
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
            readonly property bool hiddenOnResing: false
            Layout.fillWidth: true
            Layout.fillHeight: true
            antialiasing: false
            legend.visible: false
            dropShadowEnabled: true
        }
    }

    AScanInteractor {
        id: interactor
        softGain: controlTarget !== null ? controlTarget.softGain : 0
        replayValue: controlTarget !== null ? controlTarget.replayValue : 0
        chartView: chart_view
        Binding {
            when: controlTarget !== null
            target: controlTarget
            property: "replayVisible"
            value: interactor.replayVisible
        }

        Binding {
            when: controlTarget !== null
            target: controlTarget
            property: "fileName"
            value: interactor.fileName
        }

        Binding {
            when: controlTarget !== null
            target: controlTarget
            property: "date"
            value: interactor.date
        }

        Binding {
            when: controlTarget !== null
            target: controlTarget
            property: "cursorMax"
            value: interactor.aScanCursorMax
        }
    }

    Connections {
        id: cons
        ignoreUnknownSignals: true
        target: null

        function onReportExportClicked(fileName) {
            chart_view.grabToImage(function (result) {
                console.log(category, result)
                if (interactor.reportExportClicked(fileName, result)) {
                    showSuccessful(qsTr("保存成功"))
                } else {
                    showFailed(qsTr("保存失败"))
                }
            })
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
            const filePath = list_view.get(index, "filePath")
            console.log(category, "listview index changed, index:", index, "filepath:", filePath)
            controlTarget.init()
            interactor.setDefaultValue()
            if (interactor.openFile(filePath)) {
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

        function onSplitViewResizingChanged(resizing) {
            console.log(category, "onSplitViewResizingChanged:", resizing)
            if (chart_view.hiddenOnResing) {
                if (resizing) {
                    chart_view.visible = false
                } else {
                    chart_view.visible = true
                }
            }
        }
    }
}
