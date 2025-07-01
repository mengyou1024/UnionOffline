import QtQuick 2.15

Canvas {
    id: canvas

    property real gate_pos: 0
    property real gate_width: 0
    property real gate_height: 0
    property bool gate_enable: false

    property var interactor

    onPaint: {
        let ctx = getContext("2d")
        ctx.clearRect(0, 0, width, height)

        if (gate_enable) {
            ctx.strokeStyle = "#000000"
            ctx.beginPath()
            ctx.moveTo(gate_pos * width, (1 - gate_height) * height)
            ctx.lineTo((gate_pos + gate_width) * width, (1 - gate_height) * height)
            ctx.stroke()
        }
    }

    MouseArea {
        id: mouse_area
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled: true
        propagateComposedEvents: true

        property alias gate_pos: canvas.gate_pos
        property alias gate_width: canvas.gate_width
        property alias gate_height: canvas.gate_height
        property alias gate_enable: canvas.gate_enable

        property bool is_near_head: false //鼠标点击位置靠近头部, 调整波门位置和高度
        property bool is_near_tail: false //鼠标点击位置靠近尾部, 调整波门的宽度
        property real mouse_x_when_pressed: 0
        property real mouse_y_when_pressed: 0
        property int move_gate_idx: 0

        onPressed: event => {
                       is_near_head = false
                       is_near_tail = false

                       if (interactor) {
                           let near_gate = interactor.isNearGate(mouseX, mouseY, width, height, 15)
                           if (near_gate && near_gate.length === 2) {
                               move_gate_idx = near_gate[0]
                               if (near_gate[1] === 0) {
                                   is_near_head = true
                               } else {
                                   is_near_tail = true
                               }
                               mouse_x_when_pressed = mouseX
                               mouse_y_when_pressed = mouseY
                               return
                           }
                       }

                       gate_enable = true
                       gate_pos = mouseX / width
                       gate_height = (height - mouseY) / height
                       canvas.requestPaint()
                   }

        onMouseYChanged: {

            if (pressed) {
                if (is_near_head) {
                    let y = mouseY
                    interactor.drawGateDelta(move_gate_idx, 0, 0, (mouse_y_when_pressed - y) / height)
                    mouse_y_when_pressed = y
                }
            }
        }

        onMouseXChanged: {
            if (pressed) {
                if (is_near_head) {
                    let x = mouseX
                    interactor.drawGateDelta(move_gate_idx, (x - mouse_x_when_pressed) / width, 0, 0)
                    mouse_x_when_pressed = x
                    return
                } else if (is_near_tail) {
                    let x = mouseX
                    interactor.drawGateDelta(move_gate_idx, 0, (x - mouse_x_when_pressed) / width, 0)
                    mouse_x_when_pressed = x
                    return
                }

                if (mouseX > width) {
                    gate_width = 1 - gate_pos
                } else if (mouseX < 0) {
                    gate_width = -gate_pos
                } else {
                    gate_width = (mouseX / width) - gate_pos
                }
                canvas.requestPaint()
            }
        }

        onEntered: {
            forceActiveFocus()
        }

        onExited: {
            if (gate_enable) {
                gate_enable = false
                if (gate_width >= 0) {
                    gate_width = 1.0 - gate_pos
                } else {
                    gate_width = gate_pos
                    gate_pos = 0
                }

                interactor.drawGate(0, gate_pos, gate_width, gate_height)
                canvas.requestPaint()
            }
        }

        onReleased: {
            if (is_near_head || is_near_tail) {
                is_near_head = false
                is_near_tail = false
                return
            }

            gate_enable = false
            if (gate_width < 0) {
                gate_pos = gate_pos + gate_width
                gate_width = -gate_width
            }
            if (gate_width < 0.03) {
                return
            }

            interactor.drawGate(0, gate_pos, gate_width, gate_height)
            canvas.requestPaint()
        }
    }
}
