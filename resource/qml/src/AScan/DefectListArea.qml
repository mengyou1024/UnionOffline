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
            target: aScanInteractor ? aScanInteractor.scanViewHandler : null
            enabled: aScanInteractor ? (aScanInteractor.scanViewHandler ? aScanInteractor.scanViewHandler : false) : false
            ignoreUnknownSignals: true

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
            text: qsTr("起始位置S1")
        }

        Text {
            text: model.s1
        }

        Text {
            text: qsTr("终止位置S2")
        }

        Text {
            text: model.s2
        }

        Text {
            text: qsTr("缺陷指示长度S2-S1")
        }

        Text {
            text: model.verLen
        }

        Text {
            text: qsTr("最大波幅位置S3")
        }

        Text {
            text: model.ampY
        }

        Text {
            text: qsTr("缺陷深度H")
        }

        Text {
            text: model.h
        }

        Text {
            text: qsTr("当量Amax")
        }

        Text {
            text: model.aMax
        }

        Text {
            text: qsTr("缺陷所在区域")
        }

        Text {
            text: model.area + qsTr("区")
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
