import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Templates 2.15 as T

T.Button {
    property color backgroundColor: "#00e3e6"
    readonly property alias labelImplHeight: label.implicitHeight
    readonly property alias labelImplWidth: label.implicitWidth
    id: control
    hoverEnabled: true
    flat: true
    implicitHeight: label.implicitHeight
    implicitWidth: label.implicitWidth
    contentItem: Label {
        id: label
        topPadding: 5
        bottomPadding: 5
        leftPadding: 10
        rightPadding: 10
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
