import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls 1.4 as LQC
import QtQuick.Dialogs 1.3
import Qt.labs.platform 1.1
import "../../components"

ScrollView {
    implicitWidth: 280
    contentWidth: 260

    ScrollBar.vertical.policy: ScrollBar.AlwaysOn

    LoggingCategory {
        id: category
        name: "TOFD_PE.ctrl"
    }
    background: Rectangle {
        color: "transparent"
    }

    Component.onCompleted: {
        console.log(ScrollBar.vertical, ScrollBar.vertical.active)
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
        anchors.fill: parent
        CArea {
            areaText: qsTr("显示")
            Layout.margins: 2
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            GridLayout {
                anchors.fill: parent
                anchors.margins: 20
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
                }

                LQC.CheckBox {
                    id: cb_vertical_line_following
                    text: qsTr("垂直线联动")
                    checked: true
                    HoverHandler {
                        cursorShape: Qt.PointingHandCursor
                    }
                }

                LQC.CheckBox {
                    id: cb_horizontal_line_following
                    text: qsTr("水平线联动")
                    checked: false
                    HoverHandler {
                        cursorShape: Qt.PointingHandCursor
                    }
                }
            }
        }

        CArea {
            areaText: qsTr("报表")
            Layout.margins: 2
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            ColumnLayout {
                anchors.fill: parent
                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.topMargin: 15
                    Layout.bottomMargin: 5
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
                                console.log(category, "emit --> reportExportClicked:", currentFile)
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
            areaText: qsTr("TOFD软件增益")
            Layout.margins: 2
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            visible: tofdShow
            ColumnLayout {
                anchors.fill: parent
                anchors.topMargin: 15
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
            areaText: qsTr("PE软件增益:")
            Layout.margins: 2
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            visible: peShow
            ColumnLayout {
                anchors.topMargin: 15
                anchors.fill: parent
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
            areaText: qsTr("TOFD测量线")
            Layout.margins: 2
            Layout.fillWidth: true
            Layout.preferredHeight: 180
            visible: tofdShow
            ColumnLayout {
                anchors.fill: parent
                GridLayout {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.topMargin: 20
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
            areaText: qsTr("PE测量线")
            Layout.margins: 2
            Layout.fillWidth: true
            Layout.preferredHeight: 150
            visible: peShow
            ColumnLayout {
                anchors.fill: parent
                GridLayout {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.topMargin: 20
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
            areaText: qsTr("TOFD间距")
            Layout.margins: 2
            Layout.fillWidth: true
            Layout.preferredHeight: 65
            visible: tofdShow
            ColumnLayout {
                anchors.fill: parent
                GridLayout {
                    Layout.margins: 10
                    Layout.alignment: Qt.AlignHCenter
                    rows: 1
                    columns: 3
                    TextField {
                        id: tf_tofd_space
                        Layout.fillWidth: true
                        Layout.preferredHeight: 28
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
                    }
                    Text {
                        text: qsTr("mm")
                    }
                    CButton {
                        text: qsTr("TOFD更改")
                        onClicked: {
                            let val = parseFloat(tf_tofd_space.text)
                            if (isNaN(val)) {
                                tf_tofd_space.text = "0"
                                val = 0
                            }
                            console.log(category, "emit --> tofdSpaceValueModified:", parseFloat(val))
                            tofdSpaceValueModified(val)
                        }
                    }
                }
            }
        }

        CArea {
            areaText: qsTr("PE间距")
            Layout.margins: 2
            Layout.fillWidth: true
            Layout.preferredHeight: 65
            visible: peShow
            ColumnLayout {
                anchors.fill: parent
                GridLayout {
                    Layout.margins: 10
                    Layout.alignment: Qt.AlignHCenter
                    rows: 1
                    columns: 3
                    TextField {
                        id: tf_pe_space
                        Layout.fillWidth: true
                        Layout.preferredHeight: 28
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
                    }
                    Text {
                        text: qsTr("mm")
                    }
                    CButton {
                        text: qsTr("PE更改")
                        onClicked: {
                            let val = parseFloat(tf_pe_space.text)
                            if (isNaN(val)) {
                                tf_pe_space.text = "0"
                                val = 0
                            }
                            console.log(category, "emit --> peSpaceValueModified:", val)
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
    }
}
