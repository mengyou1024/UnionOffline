import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtCharts 2.15
import Union.Interactor 1.0
import "./compnents"
import Union.TOFD_PE 1.0

Rectangle {
    LoggingCategory {
        id: category
        name: "TOFD_PE.main"
    }

    property alias controlTarget: cons.target
    property alias mainTarget: main_cons.target

    clip: true
    color: "transparent"
    id: control
    ColumnLayout {
        id: grab_view
        anchors.fill: parent
        anchors.margins: 5
        spacing: 10
        ScanView {
            id: sv_tofd
            visible: controlTarget === null ? true : controlTarget.tofdShow
            Layout.fillWidth: true
            Layout.fillHeight: true
            intr: tofd_pe_intr
            softGain: controlTarget === null ? 0 : controlTarget.tofdSoftGain

            vText1: `${tofd_pe_intr.getTofdDepth(vValue1)}mm`
            vText2: `${tofd_pe_intr.getTofdDepth(vValue2)}mm`
            vSpace: Math.round((tofd_pe_intr.getTofdDepth(vValue2) - tofd_pe_intr.getTofdDepth(vValue1)) * 10) / 10

            onAdjustDepth: depth => {
                               tofd_pe_intr.adjustDepth(depth)
                               vText1 = Qt.binding(() => {
                                                       return `${tofd_pe_intr.getTofdDepth(vValue1)}mm`
                                                   })
                               vText2 = Qt.binding(() => {
                                                       return `${tofd_pe_intr.getTofdDepth(vValue2)}mm`
                                                   })
                               vSpace = Qt.binding(() => {
                                                       return Math.round((tofd_pe_intr.getTofdDepth(vValue2) - tofd_pe_intr.getTofdDepth(vValue1)) * 10) / 10
                                                   })
                           }

            onThorughWaveEvent: (x, y, w, h, isPull) => {
                                    tofd_pe_intr.throughWaveEvent(x, y, w, h, isPull)
                                }
            onRollBack: {
                tofd_pe_intr.rollBack()
                vText1 = Qt.binding(() => {
                                        return `${tofd_pe_intr.getTofdDepth(vValue1)}mm`
                                    })
                vText2 = Qt.binding(() => {
                                        return `${tofd_pe_intr.getTofdDepth(vValue2)}mm`
                                    })
                vSpace = Qt.binding(() => {
                                        return Math.round((tofd_pe_intr.getTofdDepth(vValue2) - tofd_pe_intr.getTofdDepth(vValue1)) * 10) / 10
                                    })
            }

            onHValue1Changed: {
                if (controlTarget.verticalLineFollowing) {
                    sv_pe.hValue1 = hValue1
                }
            }

            onHValue2Changed: {
                if (controlTarget.verticalLineFollowing) {
                    sv_pe.hValue2 = hValue2
                }
            }

            onVValue1Changed: {
                if (controlTarget.horizontalLineFollowing) {
                    sv_pe.vValue1 = vValue1
                }
            }

            onVValue2Changed: {
                if (controlTarget.horizontalLineFollowing) {
                    sv_pe.vValue2 = vValue2
                }
            }
        }

        ScanView {
            readonly property real _vTextShowValue1: (ordinateRange.x + vValue1 * ordinateRange.y).toFixed(1)
            readonly property real _vTextShowValue2: (ordinateRange.x + vValue2 * ordinateRange.y).toFixed(1)
            property real _angle: 0
            focus: true
            id: sv_pe
            isPe: true
            visible: controlTarget === null ? true : controlTarget.peShow
            Layout.fillWidth: true
            Layout.fillHeight: true
            intr: tofd_pe_intr
            softGain: controlTarget === null ? 0 : controlTarget.peSoftGain
            vText1: `(↘${_vTextShowValue1}  →${(Math.cos(_angle) * _vTextShowValue1).toFixed(1)}  ↓${(Math.sin(_angle) * _vTextShowValue1).toFixed(1)})`
            vText2: `(↘${_vTextShowValue2}  →${(Math.cos(_angle) * _vTextShowValue2).toFixed(1)}  ↓${(Math.sin(_angle) * _vTextShowValue2).toFixed(1)})`
            showRightMenu: false
            Component.onCompleted: {
                console.log(category, "_angle:", _angle)
                console.log(category, "Math.sin(_angle):", Math.sin(_angle))
                console.log(category, "_vTextShowValue1:", _vTextShowValue1)
                console.log(category, "(Math.sin(_angle) * _vTextShowValue2).toFixed(1):", (Math.sin(_angle) * _vTextShowValue2).toFixed(1))
            }

            onHValue1Changed: {
                if (controlTarget.verticalLineFollowing) {
                    sv_tofd.hValue1 = hValue1
                }
            }

            onHValue2Changed: {
                if (controlTarget.verticalLineFollowing) {
                    sv_tofd.hValue2 = hValue2
                }
            }

            onVValue1Changed: {
                if (controlTarget.horizontalLineFollowing) {
                    sv_tofd.vValue1 = vValue1
                }
            }

            onVValue2Changed: {
                if (controlTarget.horizontalLineFollowing) {
                    sv_tofd.vValue2 = vValue2
                }
            }
        }
    }

    TofdPeIntr {
        id: tofd_pe_intr
        onUpdatePrivateData: {
            console.log(category, "slot onUpdatePrivateData")
            sv_tofd.update()
        }
    }

    Connections {
        id: cons
        ignoreUnknownSignals: true
        target: null

        function onReportExportClicked(fileName) {
            grab_view.grabToImage(function (result) {
                if (tofd_pe_intr.reportExport(fileName, result)) {
                    showSuccessful("导出成功")
                } else {
                    showFailed("导出失败")
                }
            })
        }

        function onTofdSpaceValueModified(val) {
            console.log(category, "onTofdSpaceValueModified, value:", val)
            tofd_pe_intr.tofdSpace = val
            sv_tofd.update()
        }

        function onPeSpaceValueModified(val) {
            console.log(category, "onPeSpaceValueModified, value:", val)
            tofd_pe_intr.peSpace = val
            sv_pe.update()
        }
    }

    Connections {
        id: main_cons
        ignoreUnknownSignals: true
        function onBtnParamClicked() {
            var comp = Qt.createComponent("qrc:/qml/components/common/JsonTable.qml")
            if (comp.status === Component.Ready) {
                var params_wnd = comp.createObject(parent, {
                                                       "tableData": {
                                                           "TOFD参数": tofd_pe_intr.getTofdParam(),
                                                           "PE参数": tofd_pe_intr.getPeParam()
                                                       },
                                                       "title": qsTr("工艺参数"),
                                                       "modality": Qt.WindowModal
                                                   })
                params_wnd.closing.connect(() => {
                                               comp.destroy()
                                               params_wnd.destroy()
                                           })
            } else if (comp.status === Component.Error) {
                console.error(category, comp.errorString())
            }
        }

        function onListviewIndexChanged(index, list_view) {
            const filePath = list_view.get(index, "filePath")
            console.log(category, "listview index changed, index:", index, "filepath:", filePath)
            if (tofd_pe_intr.openFile(filePath)) {
                showSuccessful("打开成功")
                initAfterOpenFile()
            } else {
                showFailed("打开失败")
            }
        }

        function onOpenFile(file) {
            console.log(category, "open file:", file)
            if (tofd_pe_intr.openFile(file)) {
                showSuccessful("打开成功")
                initAfterOpenFile()
            } else {
                showFailed("打开失败")
            }
        }

        function onSplitViewResizingChanged(resizing) {
            console.log(category, "onSplitViewResizingChanged:", resizing)
        }
    }

    function initAfterOpenFile() {

        controlTarget.line1XLeft = Qt.binding(()=>{
            return sv_tofd.hText1
        })

        controlTarget.line1XRight = Qt.binding(()=>{
            return sv_tofd.hText2
        })

        controlTarget.line1ZLeft = Qt.binding(()=>{
            return (controlTarget === null ? false : controlTarget.showTime) ? (sv_tofd.rVValue1 * 2 / 5.9).toFixed(1) + "μs" : sv_tofd.vText1
        })

        controlTarget.line1ZRight = Qt.binding(()=>{
            return (controlTarget === null ? false : controlTarget.showTime) ? (sv_tofd.rVValue2 * 2 / 5.9).toFixed(1) + "μs" : sv_tofd.vText2
        })

        controlTarget.line1XSpace = Qt.binding(()=>{
            return sv_tofd.hSpace + " mm"
        })

        controlTarget.line1ZSpace = Qt.binding(()=>{
            return (controlTarget === null ? false : controlTarget.showTime) ? (sv_tofd.vSpace * 2 / 5.9).toFixed(1) + "μs" : sv_tofd.vSpace + " mm"
        })

        controlTarget.depth = Qt.binding(()=>{
            return (controlTarget === null ? false : controlTarget.showTime) ? sv_tofd.vSpace + " mm" : ""
        })

        controlTarget.line2XLeft = Qt.binding(()=>{
            return sv_pe.hText1
        })

        controlTarget.line2XRight = Qt.binding(()=>{
            return sv_pe.hText2
        })

        controlTarget.line2ZLeft = Qt.binding(()=>{
            return `${(Math.sin(sv_pe._angle) * sv_pe._vTextShowValue1).toFixed(1)} mm`
        })

        controlTarget.line2ZRight = Qt.binding(()=>{
            return `${(Math.sin(sv_pe._angle) * sv_pe._vTextShowValue2).toFixed(1)} mm`
        })

        controlTarget.line2XSpace = Qt.binding(()=>{
            return sv_pe.hSpace + " mm"
        })

        controlTarget.line2ZSpace = Qt.binding(()=>{
            return `${((Math.sin(sv_pe._angle) * (sv_pe._vTextShowValue2 - sv_pe._vTextShowValue1))).toFixed(1)} mm`
        })

        tofd_pe_intr.peSpace = 0
        tofd_pe_intr.tofdSpace = 0
        controlTarget.reInit()
        controlTarget.hasPe = tofd_pe_intr.hasPe()

        console.log(category, "tofd_pe_intr.getSubAngle():", tofd_pe_intr.getSubAngle())
        console.log(category, "reInitMask")
        console.log(category, `delay:${tofd_pe_intr.getDelay()}, range:${tofd_pe_intr.getRange()}`)

        sv_pe.ordinateRange = Qt.point(0, tofd_pe_intr.getSubRange())
        sv_pe.abscissaRange = Qt.point(0, tofd_pe_intr.getMaxLines())

        sv_tofd.ordinateRange = Qt.point(tofd_pe_intr.getDelay(), tofd_pe_intr.getRange())
        sv_tofd.abscissaRange = Qt.point(0, tofd_pe_intr.getMaxLines())

        sv_pe._angle = tofd_pe_intr.getSubAngle()
        sv_pe.reInitMask()
        sv_tofd.reInitMask()
        sv_pe.update()
        sv_tofd.update()
    }
}
