import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import "../../components"

Rectangle {
    LoggingCategory {
        id: category
        name: "AScan.ctrl"
    }
    implicitHeight: 520
    implicitWidth: 240
    color: "transparent"
    property int replaySpeed: 1
    property bool isReplayStart: true

    property alias replayVisible: replay_area.visible // 连续回放区域显示
    property alias fileName: lb_fileName.text // 文件名
    property alias date: lb_date.text // 日期
    property alias softGain: gainSpinBox.value // 软件增益
    property alias replayValue: sl_timerLine.value // 时间线值
    property alias cursorMax: sl_timerLine.to

    // 报表导出信号
    signal reportExportClicked(var fileName)
    // 仪器性能信号
    signal performanceClicked(var fileName)

    // 软件增益变化信号
    signal gainValueModified(real val)
    // 回放按钮点击信号
    signal replayStartClicked(bool isStart)

    // 倍速播放信号
    signal replaySpeedClicked(int val)
    // 上一帧信号
    signal lastFrameClicked
    // 下一帧信号
    signal nextFrameClicked
    // 时间线变化信号
    signal timeSliderMoved(real val)

    ColumnLayout {
        width: parent.width
        height: replay_area.visible ? 520 : 320
        anchors.centerIn: parent
        CArea {
            areaText: qsTr("文件")
            Layout.margins: 2
            Layout.fillWidth: true
            Layout.preferredHeight: 160
            ColumnLayout {
                anchors.fill: parent
                GridLayout {
                    id: file_info
                    Layout.alignment: Qt.AlignHCenter
                    Layout.topMargin: 20
                    Layout.fillWidth: true
                    columns: 2
                    rows: 2
                    columnSpacing: 5
                    rowSpacing: 20
                    Label {
                        text: qsTr("文件名:")
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    Label {
                        id: lb_fileName
                        Layout.preferredWidth: 160
                        Layout.preferredHeight: 24
                        background: Rectangle {
                            border.color: "#d8d8d8"
                            border.width: 1
                            color: "white"
                        }
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    Label {
                        text: qsTr("日   期:")
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    Label {
                        id: lb_date
                        Layout.preferredWidth: 160
                        Layout.preferredHeight: 24
                        background: Rectangle {
                            border.color: "#d8d8d8"
                            border.width: 1
                            color: "white"
                        }
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: 15
                    CButton {
                        text: qsTr("仪器性能")
                        FileDialog {
                            id: f_perf_dialog
                            fileMode: FileDialog.SaveFile
                            nameFilters: ["*.xlsx"]
                            currentFile: "file:///" + qsTr("仪器性能")
                            title: qsTr("仪器性能")
                            onAccepted: {
                                console.log(category, currentFile)
                                performanceClicked(String(currentFile).substring(8))
                            }
                        }
                        onClicked: {
                            f_perf_dialog.open()
                        }
                    }
                    spacing: 5
                    CButton {
                        text: qsTr("报表生成")
                        FileDialog {
                            id: f_report_dialog
                            fileMode: FileDialog.SaveFile
                            nameFilters: ["*.xlsx"]
                            currentFile: "file:///" + qsTr("报表生成")
                            title: qsTr("报表生成")
                            onAccepted: {
                                console.log(category, currentFile)
                                reportExportClicked(String(currentFile).substring(8))
                            }
                        }
                        onClicked: {
                            f_report_dialog.open()
                        }
                    }
                }
            }
        }

        CArea {
            areaText: qsTr("增益")
            Layout.margins: 2
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            ColumnLayout {
                anchors.fill: parent
                RowLayout {
                    Layout.topMargin: 20
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 20
                    Label {
                        text: qsTr("软件增益:")
                    }
                    CSpinBox {
                        id: gainSpinBox
                        from: -50
                        to: 50
                        value: 0
                    }
                }

                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: 20
                    CSlider {
                        from: -50
                        to: 50
                        value: gainSpinBox.value
                        onValueChanged: {
                            gainSpinBox.value = value
                            gainValueModified(value)
                        }
                    }
                }
            }
        }

        CArea {
            id: replay_area
            areaText: qsTr("操作")
            Layout.margins: 2
            Layout.fillWidth: true
            Layout.preferredHeight: 220
            ColumnLayout {
                anchors.fill: parent
                GridLayout {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.topMargin: 20
                    rows: 2
                    columns: 2
                    CButton {
                        text: isReplayStart ? qsTr("播放") : qsTr("停止")
                        onClicked: {
                            replayStartClicked(isReplayStart)
                            console.log(category, "isReplayStart:", isReplayStart)
                            if (isReplayStart === false) {
                                replayTimer.stop()
                            } else {
                                if (sl_timerLine.value === sl_timerLine.to) {
                                    sl_timerLine.value = sl_timerLine.from
                                }
                                replayTimer.start()
                            }
                            isReplayStart = !isReplayStart
                        }
                    }

                    CButton {
                        text: qsTr("×" + replaySpeed + ">>")
                        onClicked: {
                            replaySpeed *= 2
                            if (replaySpeed > 8) {
                                replaySpeed = 1
                            }
                            replaySpeedClicked(replaySpeed)
                        }
                    }
                    CButton {
                        text: qsTr("上一帧")
                        onClicked: {
                            sl_timerLine.value -= 1
                            if (sl_timerLine.value < 0) {
                                sl_timerLine.value = 0
                            }
                            timeSliderMoved(sl_timerLine.value)
                            lastFrameClicked()
                        }
                    }
                    CButton {
                        text: qsTr("下一帧")
                        onClicked: {
                            sl_timerLine.value += 1
                            if (sl_timerLine.value > sl_timerLine.to) {
                                sl_timerLine.value = sl_timerLine.to
                            }
                            timeSliderMoved(sl_timerLine.value)
                            nextFrameClicked()
                        }
                    }
                }

                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: 20
                    CSlider {
                        id: sl_timerLine
                        from: 0
                        to: 0
                        stepSize: 1
                        onMoved: {
                            timeSliderMoved(value)
                        }
                    }
                }
                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: 20
                    Label {
                        text: qsTr("第 ") + (sl_timerLine.value + 1) + qsTr(" 帧 / 共 ") + (sl_timerLine.to + 1) + qsTr(" 帧")
                        font.pixelSize: 16
                    }
                }
            }
        }
    }

    function init() {
        isReplayStart = true
        replaySpeed = 1
        sl_timerLine.value = 0
        softGain = 0
    }

    Timer {
        id: replayTimer
        interval: 40 / replaySpeed
        repeat: true
        onTriggered: {
            var newValue = sl_timerLine.value + 1
            sl_timerLine.value += 1
            if (newValue > sl_timerLine.to) {
                isReplayStart = true
                stop()
            } else {
                sl_timerLine.value = newValue
                timeSliderMoved(sl_timerLine.value)
            }
        }
    }
}
