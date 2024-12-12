import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15

RowLayout {
    property alias key: _key.text
    property alias value: _value.text
    property color textColor: "black"
    clip: true
    spacing: -1
    Rectangle {
        width: _key.width
        height: _key.height
        border.color: "#7c95c4"
        Layout.alignment: Qt.AlignCenter
        color: "#afeeee"
        Text {
            id: _key

            anchors.centerIn: parent

            padding: 5
            leftPadding: 15
            rightPadding: 15
            font.pointSize: 12
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            color: textColor
        }
    }
    Rectangle {
        Layout.alignment: Qt.AlignCenter
        Layout.fillWidth: true

        width: _value.width
        height: _value.height
        border.color: "#7c95c4"
        color: "white"
        Text {
            id: _value

            anchors.centerIn: parent

            padding: 5
            font.pointSize: 12
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            color: textColor
        }
    }
}
