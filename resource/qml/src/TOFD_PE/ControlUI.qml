import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls 1.4 as LQC
import QtQuick.Dialogs 1.3
import Qt.labs.platform 1.1
import "../../components"

ScrollView {
    implicitHeight: 900

    implicitWidth: 280
    contentHeight: 900
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
    property alias fileName: lb_fileName.text // 文件名
    property alias date: lb_date.text // 日期

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
            Layout.preferredHeight: 50
            RowLayout {
                anchors.fill: parent
                LQC.CheckBox {
                    id: cb_tofd_show
                    Layout.leftMargin: 20
                    text: qsTr("TOFD显示")
                    checked: true
                    HoverHandler {
                        cursorShape: Qt.PointingHandCursor
                    }
                }
                LQC.CheckBox {
                    id: cb_pe_show
                    Layout.rightMargin: 20
                    text: qsTr("PE显示")
                    checked: true
                    HoverHandler {
                        cursorShape: Qt.PointingHandCursor
                    }
                }
            }
        }
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
            areaText: qsTr("增益")
            Layout.margins: 2
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            ColumnLayout {
                anchors.fill: parent
                RowLayout {
                    Layout.topMargin: 20
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 20
                    Label {
                        text: qsTr("TOFD软件增益:")
                    }
                    CSpinBox {
                        id: tofdGainSpinBox
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
                        value: tofdGainSpinBox.value
                        onValueChanged: {
                            tofdGainSpinBox.value = value
                            tofdGainValueModified(value)
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 20
                    Label {
                        text: qsTr("PE软件增益:")
                    }
                    CSpinBox {
                        id: peGainSpinBox
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
            areaText: qsTr("测量线距离1")
            Layout.margins: 2
            Layout.fillWidth: true
            Layout.preferredHeight: 180
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
            areaText: qsTr("测量线距离2")
            Layout.margins: 2
            Layout.fillWidth: true
            Layout.preferredHeight: 150
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
            areaText: qsTr("TOFD/PE间距")
            Layout.margins: 2
            Layout.fillWidth: true
            Layout.preferredHeight: 95
            ColumnLayout {
                anchors.fill: parent
                GridLayout {
                    Layout.margins: 10
                    Layout.alignment: Qt.AlignHCenter
                    rows: 2
                    columns: 3
                    TextField {
                        id: tf_tofd_space
                        Layout.fillWidth: true
                        Layout.preferredHeight: 24
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        background: Rectangle {
                            border.color: "#d8d8d8"
                            border.width: 1
                            color: "white"
                        }
                        validator: DoubleValidator {
                            decimals: 1
                            notation: DoubleValidator.StandardNotation
                            top: 1000
                            bottom: -1000
                        }
                    }
                    Text {
                        text: qsTr("mm")
                    }
                    CButton {
                        text: qsTr("TOFD更改")
                        onClicked: {
                            let val = parseFloat(tf_tofd_space.text)
                            if (isNaN(val)) {
                                val = 0
                            }
                            console.log(category, "emit --> tofdSpaceValueModified:", parseFloat(val))
                            tofdSpaceValueModified(val)
                        }
                    }

                    TextField {
                        id: tf_pe_space
                        Layout.fillWidth: true
                        Layout.preferredHeight: 24
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        background: Rectangle {
                            border.color: "#d8d8d8"
                            border.width: 1
                            color: "white"
                        }
                        validator: DoubleValidator {
                            decimals: 1
                            notation: DoubleValidator.StandardNotation
                            top: 1000
                            bottom: -1000
                        }
                    }
                    Text {
                        text: qsTr("mm")
                    }
                    CButton {
                        text: qsTr("PE更改")
                        onClicked: {
                            let val = parseFloat(tf_pe_space.text)
                            if (isNaN(val)) {
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
}
