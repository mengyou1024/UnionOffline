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
        name: "AScan.help"
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
            text: qsTr("Ctrl + 鼠标左键: 快速定位红色线条\nCtrl + 鼠标右键: 快速定位蓝色线条\nWASD: 移动红色线条\n↑←↓→: 移动蓝色线条\n(仅T8/390N B/C扫功能 鼠标需先进入图谱区域)")
        }

        Text {
            Layout.fillWidth: true
            verticalAlignment: Qt.AlignVCenter
            font.pixelSize: 18
            text: qsTr("鼠标点击A扫图像位置, 并移动可以绘制波门\n拖动波门头部附近, 可以调整门位和门高\n拖动波门尾部附近, 可以调整门宽\n(仅T8/390N)")
        }
    }
}
