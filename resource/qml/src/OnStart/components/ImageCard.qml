import QtQuick 2.15
import QtQuick.Layouts 1.15
   import QtQuick.Controls 2.15

Control {
    property alias imgSource: img.source
    property alias imgWidth: img.width
    property alias imgHeight: img.height
    property alias text: _text.text
    property alias border: background.border
    implicitWidth: 200
    implicitHeight: 260
    background: Rectangle {
        id: background
        radius: 5
        color: "transparent"
    }
    leftInset: 10
    rightInset: 10

    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        Image {
            id: img
            Layout.alignment: Qt.AlignHCenter
            clip: true
            fillMode: Image.PreserveAspectCrop
        }

        Text {
            id: _text
            Layout.alignment: Qt.AlignHCenter
            font.pointSize: 16
        }
    }
}
