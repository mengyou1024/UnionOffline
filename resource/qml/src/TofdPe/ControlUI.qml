import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls 1.4 as LQC
import QtQuick.Dialogs 1.3
import Qt.labs.platform 1.1
import "../../components"
import Union.Utils 1.0
import Qt.labs.settings 1.1

ScrollView {

    id: control

    implicitWidth: layout_root.width + 20
    horizontalPadding: 10

    property string reportFilename

    LoggingCategory {
        id: category
        name: "TofdPe.ctrl"
    }

    background: Rectangle {
        color: "transparent"
    }

    property alias tofdShow: cb_tofd_show.checked // TOFD显示
    property alias peShow: cb_pe_show.checked //PE显示
    property alias verticalLineFollowing: cb_vertical_line_following.checked // 垂直坐标跟随
    property alias horizontalLineFollowing: cb_horizontal_line_following.checked // 水平坐标跟随

    property alias tofdSoftGain: tofdGainSpinBox.value
    property alias peSoftGain: peGainSpinBox.value

    property alias line1XLeft: lb_line1_x_left.text
    property alias line1XRight: lb_line1_x_right.text
    property alias line1ZLeft: lb_line1_z_left.text
    property alias line1ZRight: lb_line1_z_right.text
    property alias line1XSpace: lb_line1_x_space.text
    property alias line1ZSpace: lb_line1_z_space.text

    property alias showTime: cb_show_time.checked
    property alias depth: lb_depth.text

    property alias line2XLeft: lb_line2_x_left.text
    property alias line2XRight: lb_line2_x_right.text
    property alias line2ZLeft: lb_line2_z_left.text
    property alias line2ZRight: lb_line2_z_right.text
    property alias line2XSpace: lb_line2_x_space.text
    property alias line2ZSpace: lb_line2_z_space.text

    property alias editTofdSpace: tf_tofd_space.text
    property alias editPeSpace: tf_pe_space.text

    property alias hasPe: cb_pe_show.enabled

    // 报表生成按钮信号
    signal reportExportClicked(string fileName)
    // TOFD软件增益变化信号
    signal tofdGainValueModified(real val)
    // PE软件增益变化信号
    signal peGainValueModified(real val)
    // TOFD间距更改
    signal tofdSpaceValueModified(real val)
    // PE间距更改
    signal peSpaceValueModified(real val)

    ColumnLayout {
        id: layout_root
        CArea {
            text: qsTr("显示")
            Layout.margins: 2
            Layout.preferredWidth: layout_show.width + horizontalPadding * 2
            Layout.preferredHeight: layout_show.height + bottomPadding + topPadding
            Layout.fillWidth: true
            GridLayout {
                id: layout_show
                anchors.centerIn: parent
                rows: 2
                columns: 2
                columnSpacing: 10
                rowSpacing: 10
                LQC.CheckBox {
                    id: cb_tofd_show
                    text: qsTr("TOFD显示")
                    checked: true
                    HoverHandler {
                        cursorShape: Qt.PointingHandCursor
                    }
                }

                LQC.CheckBox {
                    id: cb_pe_show
                    text: qsTr("PE显示")
                    checked: true
                    HoverHandler {
                        cursorShape: Qt.PointingHandCursor
                    }
                    onEnabledChanged: {
                        if (enabled === false) {
                            checked = false
                        }
                    }
                }

                LQC.CheckBox {
                    id: cb_vertical_line_following
                    text: qsTr("垂直线联动")
                    checked: true
                    enabled: hasPe
                    HoverHandler {
                        cursorShape: Qt.PointingHandCursor
                    }
                    onEnabledChanged: {
                        if (enabled === false) {
                            checked = false
                        }
                    }
                }

                LQC.CheckBox {
                    id: cb_horizontal_line_following
                    text: qsTr("水平线联动")
                    checked: false
                    enabled: hasPe
                    HoverHandler {
                        cursorShape: Qt.PointingHandCursor
                    }
                    onEnabledChanged: {
                        if (enabled === false) {
                            checked = false
                        }
                    }
                }
            }
        }

        CArea {
            text: qsTr("报表")
            Layout.margins: 2
            Layout.preferredWidth: layout_report.width + horizontalPadding * 2
            Layout.preferredHeight: layout_report.height + bottomPadding + topPadding
            Layout.fillWidth: true
            ColumnLayout {
                id: layout_report
                anchors.centerIn: parent
                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 5
                    CButton {
                        text: qsTr("报表生成")
                        FileDialog {

                            Settings {
                                id: report_file_dialog_cache
                                fileName: "setting.ini"
                                category: "Cache"
                                property url reportCacheDir: StandardPaths.standardLocations(
                                                                 StandardPaths.DesktopLocation)[0] + "/" + qsTr("探伤记录")
                            }

                            id: f_report_dialog
                            folder: report_file_dialog_cache.reportCacheDir
                            fileMode: FileDialog.SaveFile
                            nameFilters: ["*.xlsx"]
                            currentFile: "file:///" + reportFilename + "-" + qsTr("探伤报告")
                            title: qsTr("报表生成")
                            onAccepted: {
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
            text: qsTr("TOFD软件增益")
            Layout.margins: 2
            Layout.preferredWidth: layout_tofd_gain.width + horizontalPadding * 2
            Layout.preferredHeight: layout_tofd_gain.height + bottomPadding + topPadding
            Layout.fillWidth: true
            visible: tofdShow
            ColumnLayout {
                id: layout_tofd_gain
                anchors.centerIn: parent
                RowLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 20
                    CSpinBox {
                        id: tofdGainSpinBox
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
                        value: tofdGainSpinBox.value
                        onValueChanged: {
                            tofdGainSpinBox.value = value
                            tofdGainValueModified(value)
                        }
                    }
                }
            }
        }

        CArea {
            text: qsTr("PE软件增益:")
            Layout.margins: 2
            Layout.preferredWidth: layout_pe_gain.width + horizontalPadding * 2
            Layout.preferredHeight: layout_pe_gain.height + bottomPadding + topPadding
            Layout.fillWidth: true
            visible: peShow
            ColumnLayout {
                id: layout_pe_gain
                anchors.centerIn: parent
                RowLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    CSpinBox {
                        id: peGainSpinBox
                        from: -50
                        to: 50
                        value: 0
                    }
                }

                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: 5
                    CSlider {
                        from: -50
                        to: 50
                        value: peGainSpinBox.value
                        onValueChanged: {
                            peGainSpinBox.value = value
                            peGainValueModified(value)
                        }
                    }
                }
            }
        }

        CArea {
            text: qsTr("TOFD测量线")
            Layout.margins: 2
            Layout.preferredWidth: layout_tofd_line.width + horizontalPadding * 2
            Layout.preferredHeight: layout_tofd_line.height + bottomPadding + topPadding
            Layout.fillWidth: true
            visible: tofdShow
            ColumnLayout {
                id: layout_tofd_line
                anchors.centerIn: parent
                GridLayout {
                    Layout.alignment: Qt.AlignHCenter
                    rows: 2
                    columns: 4
                    Label {
                        text: qsTr("X轴")
                        color: "red"
                    }
                    Label {
                        id: lb_line1_x_left
                        Layout.preferredWidth: 80
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
                        text: qsTr("X轴")
                        color: "blue"
                    }
                    Label {
                        id: lb_line1_x_right
                        Layout.preferredWidth: 80
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
                        text: qsTr("Z轴")
                        color: "red"
                    }
                    Label {
                        id: lb_line1_z_left
                        Layout.preferredWidth: 80
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
                        text: qsTr("Z轴")
                        color: "blue"
                    }
                    Label {
                        id: lb_line1_z_right
                        Layout.preferredWidth: 80
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
                    Layout.leftMargin: 10
                    Layout.rightMargin: 10
                    Label {
                        text: qsTr("X轴间距")
                    }
                    Label {
                        id: lb_line1_x_space
                        Layout.fillWidth: true
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
                    Layout.leftMargin: 10
                    Layout.rightMargin: 10
                    Label {
                        text: qsTr("Z轴间距")
                    }
                    Label {
                        id: lb_line1_z_space
                        Layout.fillWidth: true
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
                    Layout.leftMargin: 10
                    Layout.rightMargin: 10
                    Layout.bottomMargin: 10
                    LQC.CheckBox {
                        id: cb_show_time
                        text: qsTr("显示时间")
                        HoverHandler {
                            cursorShape: Qt.PointingHandCursor
                        }
                    }
                    Label {
                        Layout.leftMargin: 20
                        text: qsTr("深度")
                    }
                    Label {
                        id: lb_depth
                        Layout.fillWidth: true
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
            }
        }

        CArea {
            text: qsTr("PE测量线")
            Layout.margins: 2
            Layout.preferredWidth: layout_pe_line.width + horizontalPadding * 2
            Layout.preferredHeight: layout_pe_line.height + bottomPadding + topPadding
            Layout.fillWidth: true
            visible: peShow
            ColumnLayout {
                id: layout_pe_line
                anchors.centerIn: parent
                GridLayout {
                    Layout.alignment: Qt.AlignHCenter
                    rows: 2
                    columns: 4
                    Label {
                        text: qsTr("X轴")
                        color: "red"
                    }
                    Label {
                        id: lb_line2_x_left
                        Layout.preferredWidth: 80
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
                        text: qsTr("X轴")
                        color: "blue"
                    }
                    Label {
                        id: lb_line2_x_right
                        Layout.preferredWidth: 80
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
                        text: qsTr("Z轴")
                        color: "red"
                    }
                    Label {
                        id: lb_line2_z_left
                        Layout.preferredWidth: 80
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
                        text: qsTr("Z轴")
                        color: "blue"
                    }
                    Label {
                        id: lb_line2_z_right
                        Layout.preferredWidth: 80
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
                    Layout.leftMargin: 10
                    Layout.rightMargin: 10
                    Label {
                        text: qsTr("X轴间距")
                    }
                    Label {
                        id: lb_line2_x_space
                        Layout.fillWidth: true
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
                    Layout.leftMargin: 10
                    Layout.rightMargin: 10
                    Label {
                        text: qsTr("Z轴间距")
                    }
                    Label {
                        id: lb_line2_z_space
                        Layout.fillWidth: true
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
            }
        }

        CArea {
            text: qsTr("TOFD间距")
            Layout.margins: 2
            Layout.preferredWidth: layout_tofd_space.width + horizontalPadding * 2
            Layout.preferredHeight: layout_tofd_space.height + bottomPadding + topPadding
            Layout.fillWidth: true
            visible: tofdShow
            ColumnLayout {
                id: layout_tofd_space
                anchors.centerIn: parent
                GridLayout {
                    Layout.alignment: Qt.AlignHCenter
                    rows: 1
                    columns: 3
                    TextField {
                        id: tf_tofd_space
                        Layout.preferredWidth: 120
                        Layout.preferredHeight: 28
                        selectByMouse: true
                        selectionColor: "black"
                        selectedTextColor: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        background: Rectangle {
                            border.color: "#d8d8d8"
                            border.width: 1
                            color: "white"
                        }
                        validator: IntValidator {
                            top: 1000
                            bottom: -1000
                        }
                        text: "0"
                        onAccepted: {
                            btn_tofd_changed.clicked()
                        }
                    }
                    Text {
                        text: qsTr("mm")
                    }
                    CButton {
                        id: btn_tofd_changed
                        text: qsTr("TOFD更改")
                        implicitWidth: Math.max(labelImplWidth, 80)
                        onClicked: {
                            let val = parseFloat(tf_tofd_space.text)
                            if (isNaN(val)) {
                                tf_tofd_space.text = "0"
                                val = 0
                            }
                            tofdSpaceValueModified(val)
                        }
                    }
                }
            }
        }

        CArea {
            text: qsTr("PE间距")
            Layout.margins: 2
            Layout.preferredWidth: layout_pe_space.width + horizontalPadding * 2
            Layout.preferredHeight: layout_pe_space.height + bottomPadding + topPadding
            Layout.fillWidth: true
            visible: peShow
            ColumnLayout {
                id: layout_pe_space
                anchors.centerIn: parent
                GridLayout {
                    Layout.alignment: Qt.AlignHCenter
                    rows: 1
                    columns: 3
                    TextField {
                        id: tf_pe_space
                        Layout.preferredWidth: 120
                        Layout.preferredHeight: 28
                        selectByMouse: true
                        selectionColor: "black"
                        selectedTextColor: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        background: Rectangle {
                            border.color: "#d8d8d8"
                            border.width: 1
                            color: "white"
                        }
                        validator: IntValidator {
                            top: 1000
                            bottom: -1000
                        }
                        text: "0"
                        onAccepted: {
                            btn_pe_changed.clicked()
                        }
                    }
                    Text {
                        text: qsTr("mm")
                    }
                    CButton {
                        id: btn_pe_changed
                        text: qsTr("PE更改")
                        implicitWidth: Math.max(labelImplWidth, 80)
                        onClicked: {
                            let val = parseFloat(tf_pe_space.text)
                            if (isNaN(val)) {
                                tf_pe_space.text = "0"
                                val = 0
                            }
                            peSpaceValueModified(val)
                        }
                    }
                }
            }
        }
    }

    function reInit() {
        tf_pe_space.text = "0"
        tf_tofd_space.text = "0"
        cb_pe_show.checked = true
        peGainSpinBox.value = 0
        tofdGainSpinBox.value = 0
        cb_horizontal_line_following.checked = false
        cb_vertical_line_following.checked = true
        cb_show_time.checked = false
        hasPe = true
    }
}
