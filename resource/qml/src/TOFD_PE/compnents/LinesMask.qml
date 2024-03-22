import QtQuick
import Union.TOFD_PE

Canvas {
    LoggingCategory {
        id: tag
        name: "LineMask"
    }
    id: control
    renderTarget: Canvas.FramebufferObject
    property int cursorWidth: 5
    property string horizontalText: ""
    property string verticalText: ""
    property real horizontalValue1: 0.1
    property real horizontalValue2: 0.9
    property real verticalValue1: 0.1
    property real verticalValue2: 0.9

    readonly property real x1: Math.round((width - 1) * horizontalValue1) + 0.5
    readonly property real x2: Math.round((width - 1) * horizontalValue2) + 0.5
    readonly property real y1: Math.round((height - 1) * verticalValue1) + 0.5
    readonly property real y2: Math.round((height - 1) * verticalValue2) + 0.5

    onPaint: {
        console.time("Canvas Paint")
        let ctx = getContext("2d")
        ctx.clearRect(0, 0, width, height)
        ctx.lineWidth = 1
        ctx.strokeStyle = "red"
        ctx.beginPath() // 开始一条路径
        ctx.moveTo(x1, 0)
        ctx.lineTo(x1, height)
        ctx.moveTo(0, y1)
        ctx.lineTo(width, y1)
        ctx.stroke()
        ctx.beginPath()
        ctx.strokeStyle = "blue"
        ctx.moveTo(x2, 0)
        ctx.lineTo(x2, height)
        ctx.moveTo(0, y2)
        ctx.lineTo(width, y2)
        ctx.stroke()
        console.timeEnd("Canvas Paint")
    }

    MouseArea {
        anchors.fill: parent
        // 1. 一根线上 2. 两根线上
        hoverEnabled: true

        property var moveEventStatusEnum: LinesMaskEnum.Normal

        property bool enableMove: false

        onPressed: {
            enableMove = true
            moveEventStatusEnum = getStatusEnum(mouseX, mouseY)
            console.log(tag, "current status:", moveEventStatusEnum)
        }

        onReleased: {
            enableMove = false
        }

        onPositionChanged: {
            if (enableMove) {
                moveLineEvent(moveEventStatusEnum, mouseX, mouseY)
                requestPaint()
            } else {
                const val_enum = getStatusEnum(mouseX, mouseY)
                cursorShape = getCursorShape(val_enum)
            }
        }
    }

    function getCursorShape(status) {
        if (status === LinesMaskEnum.Normal) {
            return Qt.ArrowCursor
        } else if (status === LinesMaskEnum.ModeX1 || status === LinesMaskEnum.ModeX2) {
            return Qt.SizeHorCursor
        } else if (status === LinesMaskEnum.ModeY1 || status === LinesMaskEnum.ModeY2) {
            return Qt.SizeVerCursor
        } else {
            return Qt.SizeAllCursor
        }
    }

    function moveLineEvent(status, x, y) {
        console.log(tag, "moveLineEvent status:", status)
        if (status & LinesMaskEnum.ModeX1) {
            horizontalValue1 = x / control.width
        } else if (status & LinesMaskEnum.ModeX2) {
            horizontalValue2 = x / control.width
        }
        if (status & LinesMaskEnum.ModeY1) {
            verticalValue1 = y / control.height
        } else if (status & LinesMaskEnum.ModeY2) {
            verticalValue2 = y / control.height
        }
    }

    function getStatusEnum(x, y) {
        let ret = LinesMaskEnum.Normal
        // 判断鼠标状态
        if (Math.abs(x - horizontalValue1 * control.width) < cursorWidth) {
            // X1线上
            ret |= LinesMaskEnum.ModeX1
        }

        if (Math.abs(x - horizontalValue2 * control.width) < cursorWidth) {
            // X2线上
            ret |= LinesMaskEnum.ModeX2
        }

        if (Math.abs(y - verticalValue1 * control.height) < cursorWidth) {
            // Y1线上
            ret |= LinesMaskEnum.ModeY1
        }

        if (Math.abs(y - verticalValue2 * control.height) < cursorWidth) {
            // Y1线上
            ret |= LinesMaskEnum.ModeY2
        }
        return ret
    }
}
