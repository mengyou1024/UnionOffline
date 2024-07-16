import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    property alias imgSource: img.source
    property alias imgWidth: img.width
    property alias imgHeight: img.height
    property alias text: _text.text
    implicitWidth: 200
    implicitHeight: 260
    color: "transparent"
    radius: 5

    ColumnLayout {
        anchors.fill: parent
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
