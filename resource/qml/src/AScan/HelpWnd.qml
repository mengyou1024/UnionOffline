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
        spacing: 10

        Text {
            Layout.fillWidth: true
            verticalAlignment: Qt.AlignVCenter
            font.pixelSize: 18
            text: qsTr("T8/390N:")
        }

        Text {
            Layout.fillWidth: true
            verticalAlignment: Qt.AlignVCenter
            font.pixelSize: 18
            text: qsTr("鼠标移动至坐标轴显示区域(灰色部分)\n按住并上下(或者左右)拖动\n可以修改B/C扫显示范围\n拖动边框缩放图像")
        }

        Text {
            Layout.fillWidth: true
            verticalAlignment: Qt.AlignVCenter
            font.pixelSize: 18
            text: qsTr("按住鼠标右键并拖动可以平移视图\n按住鼠标左键拖动，快速框选缺陷")
        }

        Text {
            Layout.fillWidth: true
            verticalAlignment: Qt.AlignVCenter
            font.pixelSize: 18
            text: qsTr("Ctrl + 鼠标左键: 快速定位红色线条\nCtrl + 鼠标右键: 快速定位蓝色线条\nWASD: 移动红色线条\n↑←↓→: 移动蓝色线条\n(鼠标需先进入图谱区域)")
        }

        Text {
            Layout.fillWidth: true
            verticalAlignment: Qt.AlignVCenter
            font.pixelSize: 18
            text: qsTr("鼠标点击A扫图像位置, 并移动可以绘制波门\n拖动波门头部附近, 可以调整门位和门高\n拖动波门尾部附近, 可以调整门宽\n")
        }
    }
}
