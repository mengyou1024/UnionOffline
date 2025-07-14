import QtQuick 2.15
import "../../components"
import QtQuick.Layouts 1.15
import Union.Model 1.0

CArea {
    id: b_c_scan_overwrite_gate_mode
    text: qsTr("B扫缺陷")
    Layout.preferredHeight: layout.height + bottomPadding + topPadding

    property alias viewItem: model.viewItem

    GridLayout {
        id: layout
        anchors.centerIn: parent
        columns: 2
        DefectListModel {
            id: model
        }

        Connections {
            target: aScanInteractor.scanViewHandler

            function onDefectItemPushed() {
                model.updateModel()
                combobox.currentIndex = model.defectCount - 1
            }
        }

        CComboBox {
            id: combobox
            Layout.columnSpan: 2
            model: Array.from({
                                  "length": model.defectCount
                              }, (_, i) => `缺陷${i + 1}`)

            onCurrentIndexChanged: {
                model.currentIndex = currentIndex
            }
        }

        Text {
            text: qsTr("水平长度")
        }

        Text {
            text: model.horLen
        }

        Text {
            text: qsTr("垂直长度")
        }

        Text {
            text: model.verLen
        }

        Text {
            text: qsTr("最高波")
        }

        Text {
            text: model.ampValue + "%"
        }

        Text {
            text: qsTr("缺陷位置")
        }

        Text {
            text: `(${model.ampX}, ${model.ampY}}`
        }
    }
}
