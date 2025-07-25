import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Templates 2.15 as T

T.SpinBox {
    id: control

    property bool borderVisible: true //显示边框
    property color borderColor: "#d8d8d8" //边框颜色
    property color textColor: "black" //文本颜色
    property color bgNormalColor: "white" //文本背景色
    property color bgFocusColor: bgNormalColor //文本背景焦点色
    property color indicatorNormalColor: "black" //按钮图标颜色
    property color indicatorDisableColor: "gray"
    property color btnNormalColor: "#00e3e6" //按钮背景颜色
    property color btnHoverColor: Qt.lighter(btnNormalColor, 1.1)
    property color btnPressColor: Qt.darker(btnNormalColor, 1.1)

    editable: true

    value: 0
    implicitWidth: 120
    implicitHeight: 30

    padding: 0
    leftPadding: padding + (down.indicator ? down.indicator.width : 0)
    rightPadding: padding + (up.indicator ? up.indicator.width : 0)

    textFromValue: function (value, locale) {
        return Number(value).toString()
    }

    valueFromText: function (text, locale) {
        return parseInt(text)
    }

    validator: IntValidator {
        bottom: Math.min(control.from, control.to)
        top: Math.max(control.from, control.to)
    }

    contentItem: Rectangle {
        z: 2
        color: control.activeFocus ? bgFocusColor : bgNormalColor

        TextInput {
            anchors.fill: parent
            text: control.displayText
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            readOnly: !control.editable
            validator: control.validator
            inputMethodHints: control.inputMethodHints
            color: control.textColor
            selectByMouse: true
            selectedTextColor: "white"
            selectionColor: "black"
            font: control.font
            renderType: Text.NativeRendering

            Keys.onPressed: event => {
                                if (event.key === Qt.Key_Plus) {
                                    control.increase()
                                } else if (event.key === Qt.Key_Minus) {
                                    control.decrease()
                                }
                            }

            onEditingFinished: {
                value = valueFromText(text)
            }
        }

        Rectangle {
            width: parent.width
            height: 1
            color: borderVisible ? borderColor : "transparent"
        }
        Rectangle {
            width: parent.width
            height: 1
            anchors.bottom: parent.bottom
            color: borderVisible ? borderColor : "transparent"
        }
    }

    up.indicator: Rectangle {
        x: parent.width - width
        width: height
        implicitHeight: parent.height
        color: up.pressed ? btnPressColor : up.hovered ? btnHoverColor : btnNormalColor
        border.width: borderVisible ? 1 : 0
        border.color: borderColor
        Rectangle {
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            width: parent.width / 3
            height: 2
            color: enabled ? indicatorNormalColor : indicatorDisableColor
        }
        Rectangle {
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            width: 2
            height: parent.width / 3
            color: enabled ? indicatorNormalColor : indicatorDisableColor
        }
        HoverHandler {
            cursorShape: Qt.PointingHandCursor
        }
    }

    up.onPressedChanged: {
        if (up.pressed) {
            forceActiveFocus()
        }
    }

    down.onPressedChanged: {
        if (down.pressed) {
            forceActiveFocus()
        }
    }

    down.indicator: Rectangle {
        x: 0
        width: height
        implicitHeight: parent.height
        color: down.pressed ? btnPressColor : down.hovered ? btnHoverColor : btnNormalColor
        border.width: borderVisible ? 1 : 0
        border.color: borderColor
        Rectangle {
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            width: parent.width / 3
            height: 2
            color: enabled ? indicatorNormalColor : indicatorDisableColor
        }
        HoverHandler {
            cursorShape: Qt.PointingHandCursor
        }
    }

    background: Item {
        implicitWidth: 120
        implicitHeight: 30
    }
}
