import QtQuick
import QtQuick.Controls

Rectangle {
    id: control
    property string areaText: qsTr("未定义")
    property color borderColor: "#d8d8d8"
    property int borderWidth: 1
    property int borderRadius: 5
    property color bkColor: "#e0eeee"
    color: bkColor
    clip: true
    Rectangle {
        anchors.fill: parent
        anchors.topMargin: 8
        Label {
            anchors.verticalCenter: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 8
            text: areaText
            background: Rectangle {
                color: control.color
            }
        }

        border.color: borderColor
        border.width: borderWidth
        radius: borderRadius
        color: "transparent"
    }
}
