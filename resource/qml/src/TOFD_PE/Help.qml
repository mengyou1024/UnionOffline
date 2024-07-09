import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../components"

ApplicationWindow {
    minimumWidth: 640
    maximumWidth: 640
    minimumHeight: 480
    maximumHeight: 480

    LoggingCategory {
        id: tag
        name: "TOFD/PE.help"
    }

    title: qsTr("帮助")
    visible: true
    flags: Qt.Dialog
    modality: Qt.WindowModal
    background: Rectangle {
        color: "#e0eeee"
    }
    ColumnLayout {
        anchors.centerIn: parent

        Layout.fillHeight: true
        spacing: 26
        Text {
            Layout.fillWidth: true
            verticalAlignment: Qt.AlignVCenter
            font.pixelSize: 18
            text: qsTr("鼠标右键: 功能菜单")
        }

        Text {
            Layout.fillWidth: true
            verticalAlignment: Qt.AlignVCenter
            font.pixelSize: 18
            text: qsTr("Ctrl + 鼠标左键: 快速定位红色线条")
        }

        Text {
            Layout.fillWidth: true
            verticalAlignment: Qt.AlignVCenter
            font.pixelSize: 18
            text: qsTr("Ctrl + 鼠标右键: 快速定位蓝色线条")
        }

        Text {
            Layout.fillWidth: true
            verticalAlignment: Qt.AlignVCenter
            font.pixelSize: 18
            text: qsTr("WASD: 移动红色线条")
        }
        Text {
            Layout.fillWidth: true
            verticalAlignment: Qt.AlignVCenter
            font.pixelSize: 18
            text: qsTr("↑←↓→: 移动蓝色线条")
        }
    }
}
