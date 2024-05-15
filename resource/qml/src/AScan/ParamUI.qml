import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../components"

ApplicationWindow {
    LoggingCategory {
        id: category
        name: "AScan.Param"
    }
    id: root
    width: 640
    height: 480
    signal winClose(var x)
    title: qsTr("工艺参数")
    visible: true
    modality: Qt.WindowModal

    property var params

    background: Rectangle {
        color: "#e0eeee"
    }

    ScrollView {
        anchors.fill: parent
        anchors.topMargin: 10
        anchors.bottomMargin: 10
        ColumnLayout {
            anchors.fill: parent
            spacing: 0
            Repeater {
                model: Object.keys(params)
                delegate: ColumnLayout {
                    Layout.preferredWidth: root.width
                    Layout.alignment: Qt.AlignHCenter
                    Label {
                        Layout.alignment: Qt.AlignHCenter
                        padding: 5
                        text: modelData
                        font.pointSize: 16
                        background: Rectangle {
                            color: "transparent"
                        }
                    }

                    property var pModelData: modelData
                    Flow {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignHCenter
                        spacing: 20
                        // @disable-check M16
                        leftPadding: 20
                        // @disable-check M16
                        rightPadding: 20
                        Repeater {
                            model: Object.keys(params[pModelData])
                            CKeyValue {
                                key: modelData
                                value: params[pModelData][modelData]
                            }
                        }
                    }
                }
            }
        }
    }
}
