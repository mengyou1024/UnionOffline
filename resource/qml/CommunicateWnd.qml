import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "./components"
import Union.Extra 1.0

ApplicationWindow {

    signal saveTemporaryFile(string file)

    id: root_wnd

    minimumWidth: 240
    minimumHeight: 200
    maximumWidth: 240
    maximumHeight: 200

    LoggingCategory {
        id: tag
        name: "Communicate"
    }

    title: qsTr("通讯")
    visible: true
    flags: Qt.Dialog
    modality: Qt.WindowModal
    background: Rectangle {
        color: "#e0eeee"
    }

    GridLayout {
        anchors.fill: parent
        anchors.margins: 5
        anchors.topMargin: 13
        columns: 2
        Item {
            GridLayout.columnSpan: 2
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            Text {
                anchors.fill: parent
                text: serial.isRunning ? qsTr("正在等待通讯 ...\n请在仪器上点击<数据通讯>") : qsTr("选择端口号后\n点击确定按钮开始通讯")
                font.pointSize: 12
                wrapMode: Text.Wrap
                horizontalAlignment: Qt.AlignHCenter
            }
        }

        CComboBox {
            id: com
            GridLayout.columnSpan: 2
            model: serial.listSerialName()
            popupImplicitHeight: root_wnd.height - 10
            Layout.fillWidth: true
            Layout.preferredHeight: 32
            onPressedChanged: {
                if (pressed) {
                    model = serial.listSerialName()
                }
            }
        }
        CButton {
            Layout.fillWidth: true
            Layout.preferredHeight: 32
            text: qsTr("确定")
            onClicked: {
                serial.readSerialAndSaveFile(com.currentText, 15000)
            }
        }

        CButton {
            Layout.fillWidth: true
            Layout.preferredHeight: 32
            text: qsTr("取消")
            onClicked: {
                close()
            }
        }

        Item {
            GridLayout.columnSpan: 2
            Layout.fillWidth: true
            Layout.preferredHeight: 8
            ProgressBar {
                anchors.fill: parent
                indeterminate: true
                visible: serial.isRunning
            }
        }
    }

    SerialRunner {
        id: serial
        onFileNameChanged: {
            console.log("fileName:", fileName, "fileName.length:", fileName.length)
            if (fileName.length !== 0) {
                saveTemporaryFile(fileName)
                close()
            } else {
                toast.showFailed(qsTr("未收到数据"))
            }
        }
    }

    CToast {
        id: toast
    }
}
