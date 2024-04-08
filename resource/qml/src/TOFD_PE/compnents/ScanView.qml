import QtQuick
import QtQuick.Layouts
import QtCharts
import Union.TOFD_PE

Item {
    id: control
    property alias intr: _view.intr
    property alias isPe: _view.isPe
    property alias softGain: _view.softGain
    property alias abscissaRange: _view.abscissaRange
    property alias ordinateRange: _view.ordinateRange

    property alias hValue1: mask.horizontalValue1
    property alias hValue2: mask.horizontalValue2
    property alias hText1: mask.horizontalText1
    property alias hText2: mask.horizontalText2
    property alias vValue1: mask.verticalValue1
    property alias vValue2: mask.verticalValue2
    property alias vText1: mask.verticalText1
    property alias vText2: mask.verticalText2
    property real vSpace: ((ordinateRange.x + vValue2 * ordinateRange.y) - (ordinateRange.x + vValue1 * ordinateRange.y)).toFixed(1)
    property real hSpace: ((abscissaRange.x + hValue2 * abscissaRange.y) - (abscissaRange.x + hValue1 * abscissaRange.y)).toFixed(1)
    property alias showRightMenu: mask.showRightMenu

    signal thorughWaveEvent(real x, real y, real w, real h, bool isPull)
    signal rollBack

    LoggingCategory {
        id: category
        name: "TOFD_PE.ScanView"
    }

    function update() {
        _view.update()
        _view.updateChartView()
    }

    GridLayout {
        anchors.fill: parent
        rows: 2
        columns: 2
        columnSpacing: 10
        rowSpacing: 0
        ChartView {
            id: cv_ascan
            Layout.preferredWidth: parent.width / 5
            Layout.fillHeight: true
            antialiasing: false
            legend.visible: false
            dropShadowEnabled: true
            backgroundRoundness: 0
            plotAreaColor: "black"
            margins.left: 0
            margins.right: 0
            margins.top: 0
            margins.bottom: 0
            plotArea: Qt.rect(0, 0, width, height)

            Canvas {
                id: charvView_canvas
                anchors.fill: parent
                onPaint: {
                    let ctx = getContext("2d")
                    // 清除画布
                    ctx.clearRect(0, 0, width, height)
                    ctx.beginPath()
                    ctx.strokeStyle = "red"
                    ctx.moveTo(0.5, height * mask.verticalValue1 + 0.5)
                    ctx.lineTo(width + 0.5, height * mask.verticalValue1 + 0.5)
                    ctx.stroke()
                    ctx.beginPath()
                    ctx.strokeStyle = "blue"
                    ctx.moveTo(0.5, height * mask.verticalValue2 + 0.5)
                    ctx.lineTo(width + 0.5, height * mask.verticalValue2 + 0.5)
                    ctx.stroke()
                    ctx.closePath()
                }
            }
        }

        TofdPeScanView {
            id: _view
            chartView: cv_ascan
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.rowSpan: 2
            cursor: intr ? Math.round(intr.getLines() * mask.horizontalValue1) : 0
            LinesMask {
                id: mask
                x: _view.getDrawable().x
                y: _view.getDrawable().y
                width: _view.getDrawable().width
                height: _view.getDrawable().height

                verticalText1: (ordinateRange.x + verticalValue1 * ordinateRange.y).toFixed(1) + " mm"
                verticalText2: (ordinateRange.x + verticalValue2 * ordinateRange.y).toFixed(1) + " mm"
                horizontalText1: (abscissaRange.x + horizontalValue1 * abscissaRange.y).toFixed(1) + " mm"
                horizontalText2: (abscissaRange.x + horizontalValue2 * abscissaRange.y).toFixed(1) + " mm"

                onVerticalValue1Changed: charvView_canvas.requestPaint()
                onVerticalValue2Changed: charvView_canvas.requestPaint()

                onThorughWaveEventPrivate: (x, y, w, h, isPull) => {
                                               if (x > 1.0 || x < 0 || y > 1.0 || y < 0 || x + w > 1.0 || x + w < 0 || y + h > 1.0 || y + h < 0 || w < 0 || w > 1.0 || h < 0 || h > 1.0) {
                                                   return
                                               }
                                               thorughWaveEvent(x, y, w, h, isPull)
                                           }
                onRollBackPrivate: {
                    rollBack()
                }
            }
        }

        Rectangle {
            color: "transparent"
            height: 35
        }
    }

    Connections {
        target: mask
        function onKeyClickEvent(kl) {
            kl.forEach(k => {
                           switch (k) {
                               case Qt.Key_Left:
                               valueX2Minus()
                               break
                               case Qt.Key_Right:
                               valueX2Plus()
                               break
                               case Qt.Key_Up:
                               valueY2Minus()
                               break
                               case Qt.Key_Down:
                               valueY2Plus()
                               break
                               case Qt.Key_A:
                               valueX1Minus()
                               break
                               case Qt.Key_D:
                               valueX1Plus()
                               break
                               case Qt.Key_W:
                               valueY1Minus()
                               break
                               case Qt.Key_S:
                               valueY1Plus()
                               break
                           }
                       })
        }
    }

    function valueX1Plus() {
        if (mask.horizontalValue1 < 1.0) {
            mask.horizontalValue1 += (1 / intr.getLines())
            if (mask.horizontalValue1 > 1.0) {
                mask.horizontalValue1 = 1.0
            }
        }
    }

    function valueX1Minus() {
        if (mask.horizontalValue1 > 0) {
            mask.horizontalValue1 -= (1 / intr.getLines())
            if (mask.horizontalValue1 < 0) {
                mask.horizontalValue1 = 0
            }
        }
    }

    function valueX2Plus() {
        if (mask.horizontalValue2 < 1.0) {
            mask.horizontalValue2 += (1 / intr.getLines())
            if (mask.horizontalValue2 > 1.0) {
                mask.horizontalValue2 = 1.0
            }
        }
    }

    function valueX2Minus() {
        if (mask.horizontalValue2 > 0) {
            mask.horizontalValue2 -= (1 / intr.getLines())
            if (mask.horizontalValue2 < 0) {
                mask.horizontalValue2 = 0
            }
        }
    }

    function valueY1Plus() {
        if (mask.verticalValue1 < 1.0) {
            mask.verticalValue1 += (1 / intr.getLines())
            if (mask.verticalValue1 > 1.0) {
                mask.verticalValue1 = 1.0
            }
        }
    }

    function valueY1Minus() {
        if (mask.verticalValue1 > 0) {
            mask.verticalValue1 -= (1 / intr.getLines())
            if (mask.verticalValue1 < 0) {
                mask.verticalValue1 = 0
            }
        }
    }

    function valueY2Plus() {
        if (mask.verticalValue2 < 1.0) {
            mask.verticalValue2 += (1 / intr.getLines())
            if (mask.verticalValue2 > 1.0) {
                mask.verticalValue2 = 1.0
            }
        }
    }

    function valueY2Minus() {
        if (mask.verticalValue2 > 0) {
            mask.verticalValue2 -= (1 / intr.getLines())
            if (mask.verticalValue2 < 0) {
                mask.verticalValue2 = 0
            }
        }
    }
}
