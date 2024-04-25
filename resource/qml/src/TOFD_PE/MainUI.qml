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
            Layout.minimumWidth: control.width - 10
            Layout.maximumWidth: control.width - 10
            Layout.minimumHeight: (control.height - parent.spacing - 10) / 2
            Layout.maximumHeight: (control.height - parent.spacing - 10) / 2
            intr: tofd_pe_intr
            softGain: controlTarget === null ? 0 : controlTarget.tofdSoftGain
            onThorughWaveEvent: (x, y, w, h, isPull) => {
                                    tofd_pe_intr.throughWaveEvent(x, y, w, h, isPull)
                                }
            onRollBack: {
                tofd_pe_intr.rollBack()
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
            Layout.minimumWidth: control.width - 10
            Layout.maximumWidth: control.width - 10
            Layout.minimumHeight: (control.height - parent.spacing - 10) / 2
            Layout.maximumHeight: (control.height - parent.spacing - 10) / 2
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
        }
    }

    TofdPeIntr {
        id: tofd_pe_intr
        onUpdatePrivateData: {
            console.log(category, "slot onUpdatePrivateData")
            sv_tofd.update()
        }
    }

    Binding {
        when: controlTarget !== null
        target: controlTarget
        property: "line1XLeft"
        value: sv_tofd.hText1
    }

    Binding {
        when: controlTarget !== null
        target: controlTarget
        property: "line1XRight"
        value: sv_tofd.hText2
    }

    Binding {
        when: controlTarget !== null
        target: controlTarget
        property: "line1ZLeft"
        value: (controlTarget === null ? false : controlTarget.showTime) ? (sv_tofd.rVValue1 * 2 / 5.9).toFixed(1) + "μs" : sv_tofd.vText1
    }

    Binding {
        when: controlTarget !== null
        target: controlTarget
        property: "line1ZRight"
        value: (controlTarget === null ? false : controlTarget.showTime) ? (sv_tofd.rVValue2 * 2 / 5.9).toFixed(1) + "μs" : sv_tofd.vText2
    }

    Binding {
        when: controlTarget !== null
        target: controlTarget
        property: "line1XSpace"
        value: sv_tofd.hSpace + " mm"
    }

    Binding {
        when: controlTarget !== null
        target: controlTarget
        property: "line1ZSpace"
        value: (controlTarget === null ? false : controlTarget.showTime) ? (sv_tofd.vSpace * 2 / 5.9).toFixed(1) + "μs" : sv_tofd.vSpace + " mm"
    }

    Binding {
        when: controlTarget !== null
        target: controlTarget
        property: "depth"
        value: (controlTarget === null ? false : controlTarget.showTime) ? sv_tofd.vSpace + " mm" : ""
    }

    Binding {
        when: controlTarget !== null
        target: controlTarget
        property: "line2XLeft"
        value: sv_pe.hText1
    }

    Binding {
        when: controlTarget !== null
        target: controlTarget
        property: "line2XRight"
        value: sv_pe.hText2
    }

    Binding {
        when: controlTarget !== null
        target: controlTarget
        property: "line2ZLeft"
        value: `${(Math.sin(sv_pe._angle) * sv_pe._vTextShowValue1).toFixed(1)} mm`
    }

    Binding {
        when: controlTarget !== null
        target: controlTarget
        property: "line2ZRight"
        value: `${(Math.sin(sv_pe._angle) * sv_pe._vTextShowValue2).toFixed(1)} mm`
    }

    Binding {
        when: controlTarget !== null
        target: controlTarget
        property: "line2XSpace"
        value: sv_pe.hSpace + " mm"
    }

    Binding {
        when: controlTarget !== null
        target: controlTarget
        property: "line2ZSpace"
        value: `${((Math.sin(sv_pe._angle) * (sv_pe._vTextShowValue2 - sv_pe._vTextShowValue1))).toFixed(1)} mm`
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
        }

        function onPeSpaceValueModified(val) {
            console.log(category, "onPeSpaceValueModified, value:", val)
            tofd_pe_intr.peSpace = val
        }
    }

    Connections {
        id: main_cons
        ignoreUnknownSignals: true
        function onBtnParamClicked() {
            var comp = Qt.createComponent("qrc:/qml/src/TOFD_PE/ParamUI.qml")
            if (comp.status === Component.Ready) {
                var params_wnd = comp.createObject(parent, {
                                                       "tofd_params": tofd_pe_intr.getTofdParam(),
                                                       "pe_params": tofd_pe_intr.getPeParam()
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
