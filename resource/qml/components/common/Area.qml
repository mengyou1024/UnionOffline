import QtQuick 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15

Control {
    id: control
    property alias areaText: text.text
    property color bkColor: "#e0eeee"
    property alias fontSize: text.font.pointSize

    topInset: text.contentHeight / 2
    topPadding: text.contentHeight

    background: Rectangle {

        color: bkColor
        radius: 3
        border.width: 1
        border.color: "#7c95c4"

        Label {
            id: text
            anchors.left: parent.left
            anchors.verticalCenter: parent.top
            anchors.leftMargin: 10
            leftPadding: 2
            rightPadding: 2
            background: Rectangle {
                color: bkColor
            }
        }
    }
}
