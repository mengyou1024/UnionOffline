import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls 1.4 as LQC
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import Qt.labs.platform 1.1
import "../../components"
import Union.AScan 1.0

ScrollView {
    LoggingCategory {
        id: category
        name: "AScan.ctrl"
    }
    background: Rectangle {
        color: "transparent"
    }

    contentWidth: 260
    implicitWidth: 280

    property int replaySpeed: 1
    property bool isReplayStart: true

    property alias replayVisible: replay_area.visible // 连续回放区域显示
    property alias fileNameList: com_fileName_list.model
    property alias date: lb_date.text // 日期
    property alias softGain: gainSpinBox.value // 软件增益
    property alias replayValue: sl_timerLine.value // 时间线值
    property alias cursorMax: sl_timerLine.to
    property alias replayContinuous: cb_continuous.checked
    property alias imageVisible: img_rect.visible
    property alias showRailWeldDigram: area_rail_weld_digram.visible

    signal showImage

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
    // 设置文件名索引
    signal setFileNameIndex(int idx)

    ScrollBar.vertical.policy: ScrollBar.AlwaysOn

    ColumnLayout {
        width: 260
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
                        text: qsTr("子文件名:")
                        Layout.alignment: Qt.AlignHCenter
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    CComboBox {
                        id: com_fileName_list
                        Layout.preferredWidth: 160
                        Layout.preferredHeight: 24
                        onCurrentIndexChanged: {
                            console.log(category, "id: com_fileName_list ComboBox onCurrentIndexChanged")
                            setFileNameIndex(currentIndex)
                        }
                    }

                    Label {
                        text: qsTr("日期:")
                        Layout.alignment: Qt.AlignHCenter
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
                            radius: 3
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
                        onReleased: {
                            replayStartClicked(isReplayStart)
                            console.log(category, "isReplayStart:", isReplayStart)
                            if (isReplayStart === false) {
                                replayTimer.stop()
                            } else {
                                if (replayContinuous && fileNameList.length >= 1) {
                                    com_fileName_list.currentIndex = 0
                                }

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
                        onReleased: {
                            replaySpeed *= 2
                            if (replaySpeed > 8) {
                                replaySpeed = 1
                            }
                            replaySpeedClicked(replaySpeed)
                        }
                    }
                    CButton {
                        autoRepeat: true
                        text: qsTr("上一帧")
                        function valueMinus(val) {
                            sl_timerLine.value -= val
                            if (sl_timerLine.value < 0) {
                                sl_timerLine.value = 0
                            }
                            timeSliderMoved(sl_timerLine.value)
                            lastFrameClicked()
                        }

                        onPressAndHold: {
                            valueMinus(1)
                        }

                        onReleased: {
                            valueMinus(1)
                        }
                    }
                    CButton {
                        autoRepeat: true
                        text: qsTr("下一帧")
                        function valuePlus(val) {
                            sl_timerLine.value += val
                            if (sl_timerLine.value > sl_timerLine.to) {
                                sl_timerLine.value = sl_timerLine.to
                            }
                            timeSliderMoved(sl_timerLine.value)
                            nextFrameClicked()
                        }

                        onReleased: {
                            valuePlus(1)
                        }

                        onPressAndHold: {
                            valueMinus(1)
                        }
                    }
                }

                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: 10
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
                    Layout.bottomMargin: 10
                    Label {
                        text: qsTr("第 ") + (sl_timerLine.value + 1) + qsTr(" 帧 / 共 ") + (sl_timerLine.to + 1) + qsTr(" 帧")
                        font.pixelSize: 16
                    }
                }

                LQC.CheckBox {
                    id: cb_continuous
                    text: qsTr("连续播放所有子文件")
                    checked: true
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: 10
                    HoverHandler {
                        cursorShape: Qt.PointingHandCursor
                    }
                }
            }
        }

        Rectangle {
            property color borderColor: "#00e3e6"
            id: img_rect
            Layout.margins: 2
            Layout.preferredWidth: childrenRect.width + 6
            Layout.preferredHeight: childrenRect.height + 6
            Layout.alignment: Qt.AlignHCenter
            color: "transparent"
            border.color: borderColor
            border.width: 3
            CImage {
                id: img_camera
                anchors.centerIn: parent
                anchors.margins: 3
                maxHeight: 180
                maxWidth: 240
                MouseArea {
                    id: mouseArea_ic
                    anchors.fill: parent
                    hoverEnabled: true

                    //接受左键和右键输入
                    acceptedButtons: Qt.LeftButton

                    onClicked: showImage()

                    onEntered: {
                        img_rect.border.color = Qt.darker(img_rect.borderColor, 1.5)
                        cursorShape = Qt.PointingHandCursor
                    }

                    onExited: {
                        img_rect.border.color = img_rect.borderColor
                        cursorShape = Qt.ArrowCursor
                    }
                }
            }
        }

        CArea {
            id: area_rail_weld_digram
            areaText: qsTr("钢轨焊缝示意图")
            Layout.margins: 2
            Layout.fillWidth: true
            Layout.preferredHeight: 460
            RailWeldDigram {
                id: rail_weld_digram
                anchors.fill: parent
                anchors.margins: 10
                anchors.topMargin: 20
            }
        }
    }

    function init() {
        replayTimer.stop()
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
                if (replayContinuous === true && fileNameList.length >= 1) {
                    if (com_fileName_list.currentIndex < fileNameList.length - 1) {
                        com_fileName_list.currentIndex++
                        sl_timerLine.value = 0
                    } else {
                        isReplayStart = true
                        stop()
                    }
                } else {
                    isReplayStart = true
                    stop()
                }
            } else {
                sl_timerLine.value = newValue
                timeSliderMoved(sl_timerLine.value)
            }
        }
    }

    function setImage(img) {
        img_camera.setImage(img, true)
    }

    function setRailWeldDot(val) {
        if (val.length === 3) {
            rail_weld_digram.dotX = val[0]
            rail_weld_digram.dotY = val[1]
            rail_weld_digram.dotZ = val[2]
        }
    }
}
