import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Templates 2.15 as T

T.Button {
    property color backgroundColor: "#00e3e6"
    id: control
    hoverEnabled: true
    flat: true
    implicitHeight: 28
    implicitWidth: 96
    contentItem: Label {
        background: Rectangle {
            color: down ? Qt.darker(backgroundColor, 1.1) : (hovered ? Qt.lighter(backgroundColor, 1.1) : backgroundColor)
            radius: 3
        }
        text: control.text
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 13
    }
    HoverHandler {
        cursorShape: Qt.PointingHandCursor
    }
}
