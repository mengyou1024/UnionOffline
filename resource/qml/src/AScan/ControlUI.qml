import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls 1.4 as LQC
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import Qt.labs.platform 1.1
import "../../components"
import Union.AScan 1.0
import Union.Utils 1.0
import Qt.labs.settings 1.1

ScrollView {
    LoggingCategory {
        id: category
        name: "AScan.ctrl"
    }
    background: Rectangle {
        color: "transparent"
    }

    implicitWidth: layout_root.width + 20
    horizontalPadding: 10

    id: control
    clip: true

    property int replaySpeed: 1
    property bool isReplayStart: true

    property alias replayVisible: replay_area.visible // 连续回放区域显示
    property alias fileNameList: com_fileName_list.model
    property alias date: lb_date.text // 日期
    property alias softGain: gainSpinBox.value // 软件增益
    property alias replayValue: sl_timerLine.value // 时间线值
    property alias cursorMax: sl_timerLine.to
    property alias replayContinuous: cb_continuous.checked
    property alias replayFromHead: cb_start_from_head.checked
    property alias imageVisible: img_rect.visible
    property alias showRailWeldDigram: area_rail_weld_digram.visible
    property int replayTimerBaseInterval: 40
    property alias reportEnable: btn_report.enabled
    property alias softGainEnable: area_soft_gain.visible
    property bool dateEnable: true
    property alias railweldSpecial_ZeroPointInFoot: rail_weld_digram.zeroPointInFoot
    property alias showCMP001Special: area_t8_rail.visible

    property var aScanInteractor

    readonly property int maxLayoutWidth: Math.max(layout_file.width, layout_gain.width, 260)

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

    // ScrollBar.vertical.policy: ScrollBar.AlwaysOn
    ColumnLayout {
        id: layout_root
        CArea {
            text: qsTr("文件")
            Layout.margins: 2
            Layout.fillWidth: true
            Layout.preferredWidth: layout_file.width + horizontalPadding * 2
            Layout.preferredHeight: layout_file.height + bottomPadding + topPadding
            ColumnLayout {
                id: layout_file
                anchors.centerIn: parent
                GridLayout {
                    id: file_info
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: true
                    columns: 2
                    rows: 2
                    columnSpacing: 5
                    rowSpacing: 5
                    Label {
                        text: qsTr("子文件名:")
                        Layout.alignment: Qt.AlignHCenter
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    CComboBox {
                        id: com_fileName_list
                        Layout.preferredWidth: 160
                        Layout.preferredHeight: 28
                        popupImplicitHeight: 500
                        enabled: fileNameList ? fileNameList.length > 1 : false
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
                        enabled: dateEnable
                    }

                    Label {
                        id: lb_date
                        Layout.preferredWidth: 160
                        Layout.preferredHeight: 28
                        enabled: dateEnable
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
                    CButton {
                        id: btn_performance
                        text: qsTr("仪器性能")
                        implicitWidth: Math.max(btn_performance.labelImplWidth, btn_report.labelImplWidth, 80)
                        FileDialog {
                            Settings {
                                id: performance_file_dialog_cache
                                fileName: "setting.ini"
                                category: "Cache"
                                property url performanceCacheDir: StandardPaths.standardLocations(
                                                                      StandardPaths.DesktopLocation)[0] + "/" + qsTr(
                                                                      "仪器性能")
                            }
                            id: f_perf_dialog
                            fileMode: FileDialog.SaveFile
                            folder: performance_file_dialog_cache.performanceCacheDir
                            nameFilters: ["*.xlsx"]
                            currentFile: "file:///" + (fileNameList ? fileNameList[com_fileName_list.currentIndex] + "-" : "") + qsTr(
                                             "仪器性能")
                            title: qsTr("仪器性能")
                            onAccepted: {
                                console.log(category, currentFile)
                                performanceClicked(String(currentFile).substring(8))
                            }

                            onFolderChanged: {
                                performance_file_dialog_cache.performanceCacheDir = folder
                            }
                        }
                        onClicked: {
                            if (!FileManagement.isFileExists(performance_file_dialog_cache.performanceCacheDir)) {
                                FileManagement.createDir(performance_file_dialog_cache.performanceCacheDir)
                            }
                            f_perf_dialog.open()
                        }
                    }
                    spacing: 10
                    CButton {
                        id: btn_report
                        implicitWidth: Math.max(btn_performance.labelImplWidth, btn_report.labelImplWidth, 80)
                        text: qsTr("报表生成")

                        FileDialog {
                            id: f_report_dialog
                            Settings {
                                id: report_file_dialog_cache
                                fileName: "setting.ini"
                                category: "Cache"
                                property url reportCacheDir: StandardPaths.standardLocations(
                                                                 StandardPaths.DesktopLocation)[0] + "/" + qsTr("探伤记录")
                            }
                            folder: report_file_dialog_cache.reportCacheDir
                            fileMode: FileDialog.SaveFile
                            nameFilters: ["*.xlsx"]
                            currentFile: "file:///" + (fileNameList ? fileNameList[com_fileName_list.currentIndex] + "-" : "") + qsTr(
                                             "探伤报告")
                            title: qsTr("报表生成")
                            onAccepted: {
                                console.log(category, currentFile)
                                reportExportClicked(String(currentFile).substring(8))
                            }
                            onFolderChanged: {
                                report_file_dialog_cache.reportCacheDir = folder
                            }
                        }
                        onClicked: {
                            if (!FileManagement.isFileExists(report_file_dialog_cache.reportCacheDir)) {
                                FileManagement.createDir(report_file_dialog_cache.reportCacheDir)
                            }
                            f_report_dialog.open()
                        }
                    }
                }
            }
        }

        CArea {
            id: area_soft_gain
            text: qsTr("增益")
            Layout.margins: 2
            Layout.preferredWidth: layout_gain.width + horizontalPadding * 2
            Layout.preferredHeight: layout_gain.height + bottomPadding + topPadding
            Layout.fillWidth: true
            visible: true
            ColumnLayout {
                id: layout_gain
                anchors.centerIn: parent
                RowLayout {
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
            visible: false
            id: replay_area
            text: qsTr("操作")
            Layout.margins: 2
            Layout.fillWidth: true
            Layout.preferredWidth: layout_operation.width + horizontalPadding * 2
            Layout.preferredHeight: layout_operation.height + bottomPadding + topPadding

            ColumnLayout {
                id: layout_operation
                anchors.centerIn: parent

                GridLayout {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillHeight: true
                    rows: 2
                    columns: 2
                    CButton {
                        id: btn_play
                        text: isReplayStart ? qsTr("播放") : qsTr("停止")
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        onReleased: {
                            replayStartClicked(isReplayStart)
                            console.log(category, "isReplayStart:", isReplayStart)
                            if (isReplayStart === false) {
                                replayTimer.stop()
                                replayTimer.slTimerLineIncrease()
                            } else {
                                if (replayContinuous) {
                                    com_fileName_list.currentIndex = 0
                                }
                                if (replayFromHead) {
                                    sl_timerLine.value = 0
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
                        id: btn_play_speed
                        text: qsTr("×" + replaySpeed + ">>")
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        onReleased: {
                            replaySpeed *= 2
                            if (replaySpeed > 8) {
                                replaySpeed = 1
                            }
                            replaySpeedClicked(replaySpeed)
                        }
                    }
                    CButton {
                        id: btn_previous_frame
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: qsTr("上一帧")
                        autoRepeat: true
                        autoRepeatDelay: 1000
                        autoRepeatInterval: 50

                        function valueMinus(val) {
                            sl_timerLine.value -= val
                            if (sl_timerLine.value < 0) {
                                sl_timerLine.value = 0
                            }
                            timeSliderMoved(sl_timerLine.value)
                            lastFrameClicked()
                        }

                        onReleased: {
                            valueMinus(1)
                        }
                    }
                    CButton {
                        id: btn_next_frame
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        autoRepeat: true
                        autoRepeatDelay: 1000
                        autoRepeatInterval: 50
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
                    }
                }

                CSlider {
                    id: sl_timerLine
                    Layout.alignment: Qt.AlignHCenter
                    from: 0
                    to: 0
                    stepSize: 1
                    onMoved: {
                        timeSliderMoved(value)
                    }
                }

                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTr("第 ") + (sl_timerLine.value + 1) + qsTr(" 帧 / 共 ") + (sl_timerLine.to + 1) + qsTr(" 帧")
                    font.pixelSize: 16
                }

                LQC.CheckBox {
                    id: cb_start_from_head
                    text: qsTr("从头开始播放")
                    checked: true
                    Layout.alignment: Qt.AlignHCenter
                    HoverHandler {
                        cursorShape: Qt.PointingHandCursor
                    }
                }

                LQC.CheckBox {
                    id: cb_continuous
                    text: qsTr("连续播放所有子文件")
                    enabled: fileNameList ? fileNameList.length > 1 : false
                    checked: true
                    Layout.alignment: Qt.AlignHCenter
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
            visible: false
            id: area_rail_weld_digram
            text: qsTr("钢轨焊缝示意图")
            Layout.margins: 2
            Layout.fillWidth: true
            Layout.preferredWidth: 360
            Layout.preferredHeight: area_rail_weld_digram.width / 260 * 560

            RailWeldDigram {
                id: rail_weld_digram
                anchors.centerIn: parent
                anchors.fill: parent
                onZeroPointInFootChanged: {
                    update()
                }
            }
        }

        CArea {
            visible: false
            id: area_t8_rail
            text: qsTr("钢轨仿真")
            Layout.margins: 2
            Layout.fillWidth: true
            Layout.preferredHeight: width / 260 * 360

            RailWeldSimulation {
                id: t8_rail_simulation
                anchors.fill: parent
                anchors.centerIn: parent

                Connections {
                    target: aScanInteractor || null
                    ignoreUnknownSignals: true
                    enabled: aScanInteractor !== undefined && area_t8_rail.visible
                    function onAScanCursorChanged() {
                        t8_rail_simulation.cursorChanged(aScanInteractor.aScanCursor, softGain)
                    }
                }

                Connections {
                    target: gainSpinBox
                    ignoreUnknownSignals: true
                    enabled: aScanInteractor !== undefined && area_t8_rail.visible
                    function onValueChanged() {
                        t8_rail_simulation.cursorChanged(aScanInteractor.aScanCursor, softGain)
                    }
                }
            }
        }

        CArea {
            visible: aScanInteractor ? aScanInteractor.isSetWorkpieceThicknessSpecialEnabled : false
            id: area_workpiece_thickness
            text: qsTr("工件厚度设置")
            Layout.margins: 2
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            RowLayout {
                anchors.centerIn: parent
                Label {
                    text: qsTr("工件厚度")
                }

                CSpinBox {
                    id: text_field_workpiece_thickness
                    Layout.preferredWidth: 120
                    Layout.preferredHeight: 28
                    editable: true
                    from: 0
                    to: 100

                    onValueChanged: {
                        aScanInteractor.workpieceThicknessSpecialValue = value
                    }
                }

                Text {
                    text: "mm"
                }
            }
        }

        CArea {
            visible: aScanInteractor ? aScanInteractor.enableOverWriteGate : false
            id: overwirte_gate
            text: qsTr("波门重绘")
            Layout.margins: 2
            Layout.preferredHeight: layout_overwrite_gate.height + bottomPadding + topPadding
            Layout.fillWidth: true
            RowLayout {
                id: layout_overwrite_gate
                anchors.centerIn: parent
                CButton {
                    text: qsTr("重置波门")

                    onClicked: {
                        aScanInteractor.clearGate()
                    }
                }
            }
        }

        CArea {
            visible: aScanInteractor ? aScanInteractor.scanViewHandler : false
            id: b_c_scan_overwrite_gate_mode
            text: qsTr("B/C扫")
            Layout.margins: 2
            Layout.preferredHeight: layout_b_c_scan_overwrite_gate_mode.height + bottomPadding + topPadding
            Layout.fillWidth: true
            RowLayout {
                id: layout_b_c_scan_overwrite_gate_mode
                anchors.centerIn: parent
                LQC.CheckBox {
                    Layout.alignment: Qt.AlignVCenter
                    Layout.leftMargin: 10
                    text: qsTr("B扫仅显示门内数据")

                    checkedState: aScanInteractor ? aScanInteractor.bScanIsGateMode ? Qt.Checked : Qt.Unchecked : Qt.Unchecked

                    onCheckedStateChanged: {
                        if (checkedState === Qt.Checked) {
                            aScanInteractor.bScanIsGateMode = true
                        } else {
                            aScanInteractor.bScanIsGateMode = false
                        }
                    }
                }
            }
        }

        DefectListArea {

            visible: {
                if (!aScanInteractor) {
                    return false
                }

                if (aScanInteractor.showBScanView || aScanInteractor.showCScanView) {
                    return true
                }
                return false
            }

            Layout.margins: 2
            Layout.fillWidth: true

            viewItem: {
                if (!aScanInteractor) {
                    return null
                }

                if (aScanInteractor.showBScanView && aScanInteractor.scanViewHandler) {
                    return aScanInteractor.scanViewHandler
                }

                if (aScanInteractor.showCScanView && aScanInteractor.scanViewHandlerExtra) {
                    return aScanInteractor.scanViewHandlerExtra
                }

                return null
            }

            onClearDefect: {
                if (aScanInteractor.showBScanView) {
                    aScanInteractor.scanViewHandler.clearDefectList()
                }

                if (aScanInteractor.showCScanView) {
                    aScanInteractor.scanViewHandlerExtra.clearDefectList()
                }
            }

            onDeleteDefect: idx => {
                                if (aScanInteractor.showBScanView) {
                                    aScanInteractor.scanViewHandler.delectDefectItem(idx)
                                }

                                if (aScanInteractor.showCScanView) {
                                    aScanInteractor.scanViewHandlerExtra.delectDefectItem(idx)
                                }
                            }

            onLocateDefect: idx => {
                                if (aScanInteractor.showBScanView) {
                                    aScanInteractor.scanViewHandler.locateToDefect(idx)
                                }

                                if (aScanInteractor.showCScanView) {
                                    aScanInteractor.scanViewHandlerExtra.locateToDefect(idx)
                                }
                            }
        }
    }

    function init() {
        replayTimer.stop()
        isReplayStart = true
        replaySpeed = 1
        sl_timerLine.value = 0
        softGain = 0
        t8_rail_simulation.ascanIntf = Qt.binding(() => {
                                                      return aScanInteractor.aScanIntf
                                                  })
        t8_rail_simulation.cursorChanged(0, softGain)
    }

    Timer {
        id: replayTimer
        interval: replayTimerBaseInterval / replaySpeed
        onIntervalChanged: {
            console.log("interval:", interval)
        }

        repeat: true

        function slTimerLineIncrease() {
            var newValue = sl_timerLine.value + 1
            sl_timerLine.value += 1
            if (newValue > sl_timerLine.to) {
                if (replayContinuous === true && fileNameList.length >= 1) {
                    if (com_fileName_list.currentIndex < fileNameList.length - 1) {
                        com_fileName_list.currentIndex++
                        sl_timerLine.value = 0
                    } else {
                        isReplayStart = true
                        replayStartClicked(false)
                        stop()
                    }
                } else {
                    isReplayStart = true
                    replayStartClicked(false)
                    stop()
                }
            } else {
                sl_timerLine.value = newValue
                timeSliderMoved(sl_timerLine.value)
            }
        }

        onTriggered: {
            slTimerLineIncrease()
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
