import QtQuick 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15
import QtQuick.Templates 2.15 as T

T.Frame {
    id: control
    property alias text: text.text
    property color color: "#e0eeee"
    property alias radius: bg.radius
    property alias border: bg.border

    default property alias items: container.children

    topInset: text.contentHeight / 2
    topPadding: Math.max(text.contentHeight, 5)
    horizontalPadding: 5
    bottomPadding: 5

    Item {
        id: container
        anchors.fill: parent
    }

    background: Rectangle {
        id: bg
        color: control.color
        radius: 5
        border.width: 1
        border.color: "#7c95c4"
        antialiasing: false
        Label {
            id: text
            anchors.left: parent.left
            anchors.verticalCenter: parent.top
            anchors.leftMargin: 10
            leftPadding: 2
            rightPadding: 2
            background: Rectangle {
                color: control.color
            }
        }
    }
}
