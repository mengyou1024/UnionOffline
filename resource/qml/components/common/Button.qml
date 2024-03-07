import QtQuick
import QtQuick.Controls
import QtQuick.Templates as T

T.Button {
    property color backgroundColor: "#00e3e6"
    id: control
    hoverEnabled: true
    flat: true
    height: 28
    width: 96
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
}
