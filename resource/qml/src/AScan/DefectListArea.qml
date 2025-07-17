import QtQuick 2.15
import "../../components"
import QtQuick.Layouts 1.15
import Union.Model 1.0

CArea {
    id: b_c_scan_overwrite_gate_mode
    text: qsTr("B扫缺陷")
    Layout.preferredHeight: layout.height + bottomPadding + topPadding

    property alias viewItem: model.viewItem

    signal locateDefect(int idx)
    signal deleteDefect(int idx)
    signal clearDefect

    GridLayout {
        id: layout
        anchors.centerIn: parent
        columns: 2
        DefectListModel {
            id: model
        }

        Connections {
            target: aScanInteractor.scanViewHandler

            function onDefectListChanged() {
                model.updateModel()
                combobox.currentIndex = model.defectCount - 1
            }
        }

        CComboBox {
            Layout.preferredWidth: 120
            Layout.alignment: Qt.AlignHCenter
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
            Layout.preferredWidth: 72
            text: qsTr("水平长度")
        }

        Text {
            Layout.preferredWidth: 72
            text: model.horLen
        }

        Text {
            Layout.preferredWidth: 72
            text: qsTr("垂直长度")
        }

        Text {
            Layout.preferredWidth: 72
            text: model.verLen
        }

        Text {
            Layout.preferredWidth: 72
            text: qsTr("最高波")
        }

        Text {
            Layout.preferredWidth: 72
            text: model.ampValue + "%"
        }

        Text {
            Layout.preferredWidth: 72
            text: qsTr("缺陷位置")
        }

        Text {
            Layout.preferredWidth: 72
            text: `(${model.ampX}, ${model.ampY}}`
        }

        CButton {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 120
            Layout.columnSpan: 2
            text: qsTr("定位最高波")
            onClicked: {
                locateDefect(combobox.currentIndex)
            }
        }

        CButton {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 120
            Layout.columnSpan: 2
            Layout.topMargin: 20
            text: qsTr("删除当前缺陷")
            onClicked: {
                deleteDefect(combobox.currentIndex)
            }
        }

        CButton {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 120
            Layout.columnSpan: 2
            text: qsTr("删除所有缺陷")
            onClicked: {
                clearDefect()
            }
        }
    }
}
