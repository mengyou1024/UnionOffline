import QtQuick
import QtQuick.Controls
import QtQuick.Templates as T

T.Slider {
    id: control
    property bool acceptWheel: true //滚轮滑动
    property color handleBorderColor: "#708090" //按钮边框颜色
    property color handleNormalColor: "#00e3e6" //按钮颜色
    property color completeColor: "#575757" // 已完成区域颜色
    property color incompleteColor: "#d8d8d8" // 未完成区域颜色
    property color handleHoverColor: Qt.lighter(handleNormalColor, 1.1)
    property color handlePressColor: Qt.darker(handleNormalColor, 1.1)

    implicitWidth: horizontal ? 200 : 24
    implicitHeight: horizontal ? 24 : 200

    stepSize: 1
    padding: horizontal ? height / 3 : width / 3

    handle: Rectangle {
        x: control.leftPadding + (control.horizontal ? control.visualPosition * (control.availableWidth - width) : (control.availableWidth - width) / 2)
        y: control.topPadding + (control.horizontal ? (control.availableHeight - height) / 2 : control.visualPosition * (control.availableHeight - height))
        width: 20
        height: 20
        radius: width / 2
        color: control.pressed ? handlePressColor : control.hovered ? handleHoverColor : handleNormalColor
        border.width: 2
        border.color: handleBorderColor
    }

    background: Rectangle {
        x: control.leftPadding + (control.horizontal ? 0 : (control.availableWidth - width) / 2)
        y: control.topPadding + (control.horizontal ? (control.availableHeight - height) / 2 : 0)
        implicitWidth: control.horizontal ? 200 : 6
        implicitHeight: control.horizontal ? 6 : 200
        width: control.horizontal ? control.availableWidth : implicitWidth
        height: control.horizontal ? implicitHeight : control.availableHeight
        radius: 3
        color: control.incompleteColor
        scale: control.horizontal && control.mirrored ? -1 : 1

        Rectangle {
            y: control.horizontal ? 0 : control.visualPosition * parent.height
            width: control.horizontal ? control.position * parent.width : parent.width
            height: control.horizontal ? parent.height : control.position * parent.height
            radius: 3
            color: control.completeColor
        }
    }
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        onWheel: {
            if (wheel.angleDelta.y < 0) {
                control.decrease()
            } else {
                control.increase()
            }
        }
        cursorShape: Qt.PointingHandCursor
    }
}
