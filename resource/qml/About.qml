import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import "./components"

ApplicationWindow {
    minimumWidth: 640
    maximumWidth: 640
    minimumHeight: 480
    maximumHeight: 480

    signal winClose(var x)
    title: qsTr("关于")
    visible: true
    flags: Qt.Dialog
    modality: Qt.WindowModal
    background: Rectangle {
        color: "#e0eeee"
    }
    RowLayout {
        x: 50
        y: 150
        height: 120
        spacing: 10
        Image {
            Layout.preferredWidth: 120
            source: "qrc:/img/morose.ico"
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            fillMode: Image.PreserveAspectFit
        }
        ColumnLayout {
            Layout.fillHeight: true
            spacing: 40
            Text {
                Layout.fillWidth: true
                verticalAlignment: Qt.AlignVCenter
                font.pixelSize: 18
                text: `通用离线分析软件 ${MOROSE_APP_VERSION}`
            }
            Text {
                Layout.fillWidth: true
                verticalAlignment: Qt.AlignVCenter
                font.pixelSize: 18
                text: `版权所有(C) 南通友联数码技术开发有限公司`
            }
        }
    }
}
