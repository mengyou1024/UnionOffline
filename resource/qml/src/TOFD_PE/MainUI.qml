import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtCharts
import Union.Interactor
import "./compnents"
import Union.TOFD_PE

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
        }

        ScanView {
            id: sv_pe
            isPe: true
            visible: controlTarget === null ? true : controlTarget.peShow
            Layout.minimumWidth: control.width - 10
            Layout.maximumWidth: control.width - 10
            Layout.minimumHeight: (control.height - parent.spacing - 10) / 2
            Layout.maximumHeight: (control.height - parent.spacing - 10) / 2
            intr: tofd_pe_intr
            softGain: controlTarget === null ? 0 : controlTarget.peSoftGain
        }
    }

    TofdPeIntr {
        id: tofd_pe_intr
    }

    Connections {
        id: cons
        ignoreUnknownSignals: true
        target: null
    }

    Connections {
        id: main_cons
        ignoreUnknownSignals: true
        function onBtnParamClicked() {
            var comp = Qt.createComponent("qrc:/qml/src/AScan/ParamUI.qml")
            if (comp.status === Component.Ready) {
                // TODO: 获取参数
                // const params = interactor.getTechnologicalParameter()
                // console.log(category, params)
                var params = {
                    "参数1": {
                        "name": "张三",
                        "age": 18
                    }
                }
                var params_wnd = comp.createObject(parent, {
                                                       "params": params
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
                sv_pe.update()
                sv_tofd.update()
            } else {
                showFailed("打开失败")
            }
        }

        function onOpenFile(file) {
            console.log(category, "open file:", file)
            if (tofd_pe_intr.openFile(file)) {
                showSuccessful("打开成功")
            } else {
                showFailed("打开失败")
            }
        }

        function onSplitViewResizingChanged(resizing) {
            console.log(category, "onSplitViewResizingChanged:", resizing)
        }
    }
}
