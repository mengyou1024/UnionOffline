import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtCharts 2.15
import Union.Interactor 1.0
import "../../components"
import Union.AScan 1.0

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

    readonly property var gateTextColor: ["red", Qt.darker("#8470ff", 1.5)]

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
                    delegate: Flow {
                        property int gateIndex: index
                        Layout.alignment: Qt.AlignCenter
                        property var rowColor: gateTextColor[index]
                        spacing: 20
                        leftPadding: 20
                        rightPadding: 20
                        Repeater {
                            model: ["amp", "dist_c", "dist_a", "dist_b", "equi"]
                            delegate: CKeyValue {
                                displayColon: false
                                key: gateText[gateIndex][modelData]
                                value: interactor.gateValue[gateIndex][modelData]
                                textColor: rowColor
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
        onAScanCursorChanged: {
            console.log("onAScanCursorChanged & setimage")
            if (interactor.hasCameraImage) {
                controlTarget.setImage(interactor.getCameraImage())
                if (pop_img.opened) {
                    img_popup.setImageAndZoomIn(interactor.getCameraImage(), 2)
                }
            }
        }

        Binding {
            when: controlTarget !== null
            target: controlTarget
            property: "replayVisible"
            value: interactor.replayVisible
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

        Binding {
            when: controlTarget !== null
            target: controlTarget
            property: "imageVisible"
            value: interactor.hasCameraImage
        }

        onReplayValueChanged: {
            controlTarget.replayValue = replayValue
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

        function onSetFileNameIndex(idx) {
            interactor.setFileNameIndex(idx)
        }

        function onShowImage() {
            console.log("on click image")
            img_popup.setImageAndZoomIn(interactor.getCameraImage(), 2)
            pop_img.open()
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
            openFileAction(filePath)
        }

        function onOpenFile(filePath) {
            openFileAction(filePath)
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

    function openFileAction(filePath) {
        controlTarget.init()
        interactor.setDefaultValue()
        if (interactor.openFile(filePath)) {
            controlTarget.fileNameList = interactor.getFileNameList()
            if (interactor.hasCameraImage) {
                controlTarget.setImage(interactor.getCameraImage())
            }
            showSuccessful(qsTr("打开成功"))
        } else {
            showFailed(qsTr("打开失败"))
        }
    }

    Popup {
        id: pop_img
        anchors.centerIn: parent
        width: 0
        height: 0
        modal: true
        background: Rectangle {
            color: "transparent"
        }

        CImage {
            id: img_popup
            anchors.centerIn: parent
        }
    }
}
