import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15
import QtQuick.Templates 2.15 as T

T.ComboBox {
    id: control

    implicitWidth: implicitBackgroundWidth
    implicitHeight: implicitBackgroundHeight
    leftPadding: padding + (!control.mirrored || !indicator || !indicator.visible ? 0 : indicator.width + spacing)
    rightPadding: padding + (control.mirrored || !indicator || !indicator.visible ? 0 : indicator.width + spacing)

    // 可在此定义多为改变的特殊属性，在调用时直接指定此属性即可
    // font.family: "Microsoft YaHei"
    property color fontColor: "black" // 字体颜色
    property color backgroundColor: "#00e3e6" // 背景色
    hoverEnabled: true

    onHoveredChanged: canvas.requestPaint()

    // 弹出框行委托
    delegate: ItemDelegate {
        width: parent.width
        height: 28
        // 行字体样式
        contentItem: Text {
            text: modelData
            font: control.font
            color: control.fontColor
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            renderType: Text.NativeRendering
            horizontalAlignment: Text.AlignHCenter
        }

        palette.text: control.palette.text
        palette.highlightedText: control.palette.highlightedText
        font.weight: control.currentIndex === index ? Font.DemiBold : Font.Normal
        highlighted: control.highlightedIndex === index
        hoverEnabled: control.hoverEnabled
    }

    // 右侧下拉箭头
    indicator: Canvas {
        id: canvas
        x: control.width - width - control.rightPadding
        y: control.topPadding + (control.availableHeight - height) / 2
        width: 10
        height: 6
        contextType: "2d"
        property color drawColor: "#00e3e6"

        Connections {
            target: control
            function onPressedChanged() {
                canvas.requestPaint()
            }
        }

        onPaint: {
            context.reset()
            context.moveTo(0, 0)

            context.lineWidth = 2
            context.lineTo(width / 2, height * 0.8)
            context.lineTo(width, 0)
            if (control.pressed) {
                context.strokeStyle = Qt.darker(drawColor, 1.3)
            } else {
                context.strokeStyle = control.hovered ? Qt.lighter(drawColor, 1.2) : drawColor
            }
            context.stroke()
        }
    }

    // ComboBox的文字位置样式
    contentItem: T.TextField {
        leftPadding: !control.mirrored ? 12 : control.editable && activeFocus ? 3 : 1
        rightPadding: control.mirrored ? 12 : control.editable && activeFocus ? 3 : 1
        topPadding: 6 - control.padding
        bottomPadding: 6 - control.padding

        text: control.editable ? control.editText : control.displayText

        horizontalAlignment: TextField.AlignHCenter

        enabled: control.editable
        autoScroll: control.editable
        readOnly: control.down
        inputMethodHints: control.inputMethodHints
        validator: control.validator

        font: control.font
        color: control.fontColor
        //color: control.editable ? control.palette.text : control.palette.buttonText
        selectionColor: control.palette.highlight
        selectedTextColor: control.palette.highlightedText
        verticalAlignment: Text.AlignVCenter
        renderType: Text.NativeRendering

        background: Rectangle {
            visible: control.enabled && control.editable && !control.flat
            border.width: parent && parent.activeFocus ? 2 : 1
            border.color: parent && parent.activeFocus ? control.palette.highlight : control.palette.button
            color: control.palette.base
        }
    }

    // ComboBox 的背景样式
    background: Rectangle {
        implicitWidth: 120
        implicitHeight: 24

        radius: 3
        color: control.enabled ? "#FFFFFF" : control.backgroundColor
        border.color: control.backgroundColor
        border.width: !control.editable && control.visualFocus ? 2 : 1
        visible: !control.flat || control.down
    }

    // 弹出窗口样式
    popup: T.Popup {
        y: control.height
        width: control.width
        //height: Math.min(contentItem.implicitHeight, control.Window.height - topMargin - bottomMargin);
        height: contentItem.implicitHeight
        topMargin: 3
        bottomMargin: 3

        contentItem: ListView {
            // 防止显示过界
            clip: true
            //禁止滑动
            interactive: false
            //禁用橡皮筋效果
            boundsBehavior: Flickable.StopAtBounds

            implicitHeight: contentHeight
            model: control.delegateModel
            currentIndex: control.highlightedIndex
            highlightMoveDuration: 0

            Rectangle {
                z: 10
                width: parent.width
                height: parent.height
                color: "transparent"
                border.color: control.palette.mid
            }

            T.ScrollIndicator.vertical: ScrollIndicator {}
        }

        background: Rectangle {
            color: control.pressed ? "#EEEFF7" : control.palette.window
            border.width: 1
            border.color: control.backgroundColor
            radius: 3
        }
    }
}