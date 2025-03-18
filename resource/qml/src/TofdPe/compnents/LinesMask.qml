import QtQuick 2.15
import Union.TofdPe 1.0
import Qt.labs.platform 1.1

Canvas {
    LoggingCategory {
        id: category
        name: "TofdPe.LineMask"
    }
    id: control
    renderTarget: Canvas.FramebufferObject

    property bool showRightMenu: true
    property int cursorWidth: 5
    property string horizontalText1: "Error"
    property string verticalText1: "Error"
    property string horizontalText2: "Error"
    property string verticalText2: "Error"
    property real horizontalValue1: 0.05
    property real horizontalValue2: 0.1
    property real verticalValue1: 0.45
    property real verticalValue2: 0.55
    property bool inArea: false
    property var cursorUiStatus: MaskStatusEnum.Normal

    property real drawRectX1: 0
    property real drawRectY1: 0
    property real drawRectX2: 0
    property real drawRectY2: 0
    property real drawRectW: drawRectX2 - drawRectX1
    property real drawRectH: drawRectY2 - drawRectY1

    signal thorughWaveEventPrivate(real x, real y, real w, real h, bool isPull)
    signal rollBackPrivate
    signal adjustDepthPrivate(real depth)

    readonly property real x1: Math.round((width - 1) * horizontalValue1) + 0.5
    readonly property real x2: Math.round((width - 1) * horizontalValue2) + 0.5
    readonly property real y1: Math.round((height - 1) * verticalValue1) + 0.5
    readonly property real y2: Math.round((height - 1) * verticalValue2) + 0.5

    onHorizontalText1Changed: requestPaint()
    onHorizontalText2Changed: requestPaint()
    onVerticalText1Changed: requestPaint()
    onVerticalText2Changed: requestPaint()
    onHorizontalValue1Changed: requestPaint()
    onHorizontalValue2Changed: requestPaint()
    onVerticalValue1Changed: requestPaint()
    onVerticalValue2Changed: requestPaint()
    onCursorUiStatusChanged: requestPaint()

    signal keyClickEvent(var kl)

    onPaint: {
        let ctx = getContext("2d")
        // 清除画布
        ctx.clearRect(0, 0, width, height)
        if (cursorUiStatus === MaskStatusEnum.Normal) {
            let cursorStatus = mouseArea.currentEventStatusEnum
            // 蓝线
            if (cursorStatus & LinesMaskEnum.ModeX2 && inArea && cursorUiStatus === MaskStatusEnum.Normal && !(cursorStatus & LinesMaskEnum.ModeX1)) {
                ctx.strokeStyle = "#00ff00"
            } else {
                ctx.strokeStyle = "blue"
            }
            ctx.beginPath()
            ctx.moveTo(x2, 0)
            ctx.lineTo(x2, height)
            ctx.stroke()
            if (cursorStatus & LinesMaskEnum.ModeY2 && inArea && cursorUiStatus === MaskStatusEnum.Normal && !(cursorStatus & LinesMaskEnum.ModeY1)) {
                ctx.strokeStyle = "#00ff00"
            } else {
                ctx.strokeStyle = "blue"
            }
            ctx.beginPath()
            ctx.moveTo(0, y2)
            ctx.lineTo(width, y2)
            ctx.stroke()

            if (((cursorStatus & LinesMaskEnum.ModeY2) || (cursorStatus & LinesMaskEnum.ModeX2)) && inArea && cursorUiStatus === MaskStatusEnum.Normal && (!(cursorStatus & LinesMaskEnum.ModeY1) || !(cursorStatus & LinesMaskEnum.ModeX1))) {
                ctx.strokeStyle = "#00ff00"
            } else {
                ctx.strokeStyle = "blue"
            }
            ctx.beginPath()
            ctx.lineWidth = 1
            ctx.moveTo(x2, y2)
            ctx.quadraticCurveTo(x2 + 4, y2, x2 + 10, y2 + 10)
            ctx.moveTo(x2, y2)
            ctx.quadraticCurveTo(x2 - 4, y2, x2 - 10, y2 + 10)
            ctx.stroke()
            ctx.font = "16px 微软雅黑"
            ctx.textAlign = "right"
            if (verticalValue2 < 0.5) {
                ctx.textBaseline = "top"
            } else {
                ctx.textBaseline = "bottom"
            }
            ctx.fillStyle = "blue"
            ctx.fillText(verticalText2, width, y2)
            if (horizontalValue2 < 0.5) {
                ctx.textAlign = "left"
            } else {
                ctx.textAlign = "right"
            }
            ctx.textBaseline = "bottom"
            ctx.fillText(horizontalText2, x2, height)

            // 红线
            if (cursorStatus & LinesMaskEnum.ModeX1 && inArea && cursorUiStatus === MaskStatusEnum.Normal && cursorUiStatus === MaskStatusEnum.Normal) {
                ctx.strokeStyle = "#00ff00"
            } else {
                ctx.strokeStyle = "red"
            }
            ctx.beginPath() // 开始一条路径
            ctx.moveTo(x1, 0)
            ctx.lineTo(x1, height)
            ctx.stroke()
            if (cursorStatus & LinesMaskEnum.ModeY1 && inArea && cursorUiStatus === MaskStatusEnum.Normal) {
                ctx.strokeStyle = "#00ff00"
            } else {
                ctx.strokeStyle = "red"
            }
            ctx.beginPath()
            ctx.moveTo(0, y1)
            ctx.lineTo(width, y1)
            ctx.stroke()

            //绘制曲线
            if (((cursorStatus & LinesMaskEnum.ModeY1) || (cursorStatus & LinesMaskEnum.ModeX1)) && inArea && cursorUiStatus === MaskStatusEnum.Normal) {
                ctx.strokeStyle = "#00ff00"
            } else {
                ctx.strokeStyle = "red"
            }
            ctx.beginPath()
            ctx.lineWidth = 1
            ctx.moveTo(x1, y1)
            ctx.quadraticCurveTo(x1 + 4, y1, x1 + 10, y1 + 10)
            ctx.moveTo(x1, y1)
            ctx.quadraticCurveTo(x1 - 4, y1, x1 - 10, y1 + 10)
            ctx.stroke()
            ctx.closePath()

            if (verticalValue1 < 0.5) {
                ctx.textBaseline = "top"
            } else {
                ctx.textBaseline = "bottom"
            }
            ctx.textAlign = "left"
            // ctx.textBaseline = "bottom"
            ctx.fillStyle = "red"
            ctx.fillText(verticalText1, 0, y1)
            if (horizontalValue1 < 0.5) {
                ctx.textAlign = "left"
            } else {
                ctx.textAlign = "right"
            }
            ctx.textBaseline = "top"
            ctx.fillText(horizontalText1, x1, 0)
        } else if (cursorUiStatus === MaskStatusEnum.RmThroughWave || cursorUiStatus === MaskStatusEnum.PullThroughWave) {
            console.log(category, "drawRectangle")
            ctx.lineWidth = 1
            ctx.strokeStyle = 'yellow'
            ctx.strokeRect(drawRectX1 * width, drawRectY1 * height, drawRectW * width, drawRectH * height)
        }
    }

    Menu {
        id: _menu
        MenuItem {
            text: qsTr("调校")
            onTriggered: {
                console.log(category, qsTr("调校"))
                adjustDepthPrivate(verticalValue1)
                control.requestPaint()
            }
        }
        MenuItem {
            text: qsTr("去直通波")
            onTriggered: {
                console.log(category, qsTr("去直通波"))
                mouseArea.cursorShape = Qt.ArrowCursor
                cursorUiStatus = MaskStatusEnum.RmThroughWave
            }
        }
        MenuItem {
            text: qsTr("拉直通波")
            onTriggered: {
                console.log(category, qsTr("拉直通波"))
                mouseArea.cursorShape = Qt.ArrowCursor
                cursorUiStatus = MaskStatusEnum.PullThroughWave
            }
        }
        MenuItem {
            text: qsTr("撤销")
            onTriggered: {
                console.log(category, qsTr("撤销"))
                mouseArea.cursorShape = Qt.ArrowCursor
                cursorUiStatus = MaskStatusEnum.Normal
                rollBackPrivate()
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        property var moveEventStatusEnum: LinesMaskEnum.Normal
        property bool enableMove: false
        property bool enableDrawRectangle: false
        property var currentEventStatusEnum: LinesMaskEnum.Normal
        property bool enableLineFastMove: false

        onCurrentEventStatusEnumChanged: requestPaint()

        onPressed: event => {
                       if (cursorUiStatus === MaskStatusEnum.Normal && event.button === Qt.LeftButton) {
                           enableMove = true
                           moveEventStatusEnum = getStatusEnum(mouseX, mouseY)
                       } else if (event.button === Qt.LeftButton && (cursorUiStatus === MaskStatusEnum.RmThroughWave || cursorUiStatus === MaskStatusEnum.PullThroughWave)) {
                           enableDrawRectangle = true
                           drawRectX1 = mouseX / width
                           drawRectY1 = mouseY / height
                           console.log(category, `onPressed, drawRectX1:${drawRectX1} drawRectY1:${drawRectY1}`)
                       }
                   }

        onClicked: event => {
                       if (event.button === Qt.RightButton) {
                           if (showRightMenu) {
                               if (enableLineFastMove === false) {
                                   _menu.open()
                               }
                           }
                       }
                       if (enableLineFastMove) {
                           if (cursorUiStatus === MaskStatusEnum.Normal) {
                               if (event.button === Qt.LeftButton) {
                                   horizontalValue1 = mouseX / width
                                   verticalValue1 = mouseY / height
                               } else if (event.button === Qt.RightButton) {
                                   horizontalValue2 = mouseX / width
                                   verticalValue2 = mouseY / height
                               }
                           }
                       }
                   }

        onEntered: {
            inArea = true
            forceActiveFocus()
            requestPaint()
        }

        onExited: {
            focus = false
            inArea = false
            requestPaint()
        }

        onReleased: event => {
                        if (cursorUiStatus === MaskStatusEnum.Normal && event.button === Qt.LeftButton) {
                            enableMove = false
                        } else if (event.button === Qt.LeftButton && (cursorUiStatus === MaskStatusEnum.RmThroughWave || cursorUiStatus === MaskStatusEnum.PullThroughWave)) {
                            if (drawRectX2 < drawRectX1) {
                                const temp = drawRectX2
                                drawRectX2 = drawRectX1
                                drawRectX1 = temp
                            }

                            if (drawRectY2 < drawRectY1) {
                                const temp = drawRectY2
                                drawRectY2 = drawRectY1
                                drawRectY1 = temp
                            }
                            console.log(category, `drawRectX1:${drawRectX1}, drawRectX2:${drawRectX2}, drawRectY1:${drawRectY1}, drawRectY2:${drawRectY2}`)
                            enableDrawRectangle = false
                            thorughWaveEventPrivate(drawRectX1, drawRectY1, drawRectW, drawRectH, cursorUiStatus === MaskStatusEnum.PullThroughWave)
                            cursorUiStatus = MaskStatusEnum.Normal
                            drawRectX1 = 0
                            drawRectX2 = 0
                            drawRectY1 = 0
                            drawRectY2 = 0
                        }
                    }

        onPositionChanged: {
            if (cursorUiStatus === MaskStatusEnum.Normal) {
                if (enableMove) {
                    moveLineEvent(moveEventStatusEnum, mouseX, mouseY)
                } else {
                    const val_enum = getStatusEnum(mouseX, mouseY)
                    if (currentEventStatusEnum !== val_enum) {
                        currentEventStatusEnum = val_enum
                    }
                    cursorShape = getCursorShape(val_enum)
                }
            } else if (cursorUiStatus === MaskStatusEnum.RmThroughWave || cursorUiStatus === MaskStatusEnum.PullThroughWave) {
                if (enableDrawRectangle) {
                    drawRectX2 = mouseX / width
                    drawRectY2 = mouseY / height
                    requestPaint()
                    console.log(category, "onPositionChanged")
                }
            }
        }
    }

    function getCursorShape(status) {
        if (status === LinesMaskEnum.Normal || cursorUiStatus !== LinesMaskEnum.Normal) {
            return Qt.ArrowCursor
        } else if (status === LinesMaskEnum.ModeX1 || status === LinesMaskEnum.ModeX2 || (status === (LinesMaskEnum.ModeX1 | LinesMaskEnum.ModeX2))) {
            return Qt.SizeHorCursor
        } else if (status === LinesMaskEnum.ModeY1 || status === LinesMaskEnum.ModeY2 || (status === (LinesMaskEnum.ModeY1 | LinesMaskEnum.ModeY2))) {
            return Qt.SizeVerCursor
        } else {
            return Qt.SizeAllCursor
        }
    }

    function moveLineEvent(status, x, y) {
        if (x < 0) {
            x = 0
        } else if (x > control.width) {
            x = control.width
        }

        if (y < 0) {
            y = 0
        } else if (y > control.height) {
            y = control.height
        }

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

    property var _keyEventList: []

    Keys.onPressed: event => {
                        if (event.key === Qt.Key_Control) {
                            mouseArea.enableLineFastMove = true
                        }

                        if (_keyEventList.indexOf(event.key) < 0) {
                            _keyEventList.push(event.key)
                        }
                        keyClickEvent(_keyEventList)
                    }
    Keys.onReleased: event => {
                         if (event.key === Qt.Key_Control) {
                             mouseArea.enableLineFastMove = false
                         }
                         _keyEventList = _keyEventList.filter(it => {
                                                                  return it !== event.key
                                                              })
                     }

    function reInitLines() {
        horizontalValue1 = 0
        horizontalValue2 = 0
        verticalValue1 = 0
        verticalValue2 = 0

        horizontalValue1 = 0.05
        horizontalValue2 = 0.1
        verticalValue1 = 0.45
        verticalValue2 = 0.55
    }
}
