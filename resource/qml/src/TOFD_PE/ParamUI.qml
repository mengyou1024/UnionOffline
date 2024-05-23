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

    property var tofd_params
    property var pe_params

    background: Rectangle {
        color: "#e0eeee"
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 5

        Label {
            id: _lb_tofd_param
            text: qsTr("TOFD参数")
            Layout.alignment: Qt.AlignHCenter
            font.pointSize: 16
            background: Rectangle {
                color: Qt.lighter("#3f6feb", 1.5)
                radius: 3
            }
        }

        ScrollView {
            clip: true
            Layout.fillWidth: true
            Layout.preferredHeight: (parent.height - _lb_tofd_param.height - _lb_pe_param.height - 25) / 2
            background: Rectangle {
                color: Qt.darker("#eef4f9", 1.1)
            }

            ColumnLayout {

                anchors.fill: parent
                spacing: 5
                Item {
                    Layout.preferredWidth: root.width
                    Layout.alignment: Qt.AlignHCenter
                    height: 20
                }

                Repeater {
                    model: Object.keys(tofd_params)
                    delegate: ColumnLayout {
                        Layout.preferredWidth: root.width
                        Layout.alignment: Qt.AlignHCenter
                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            padding: 5
                            text: modelData
                            font.pointSize: 12
                            background: Rectangle {
                                color: Qt.lighter("#3f6feb", 1.5)
                                radius: 3
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
                                model: Object.keys(tofd_params[pModelData])
                                CKeyValue {
                                    key: modelData
                                    value: tofd_params[pModelData][modelData]
                                }
                            }
                        }
                    }
                }

                Item {
                    Layout.preferredWidth: root.width
                    Layout.alignment: Qt.AlignHCenter
                    height: 20
                }
            }
        }

        Label {
            id: _lb_pe_param
            text: qsTr("PE参数")
            Layout.alignment: Qt.AlignHCenter
            font.pointSize: 16
            background: Rectangle {
                color: Qt.lighter("#3f6feb", 1.5)
                radius: 3
            }
        }

        ScrollView {
            clip: true
            Layout.fillWidth: true
            Layout.preferredHeight: (parent.height - _lb_tofd_param.height - _lb_pe_param.height - 25) / 2
            background: Rectangle {
                color: Qt.darker("#e0eeee", 1.1)
            }

            ColumnLayout {
                anchors.fill: parent
                spacing: 5

                Item {
                    Layout.preferredWidth: root.width
                    Layout.alignment: Qt.AlignHCenter
                    height: 20
                }

                Repeater {
                    model: Object.keys(pe_params)
                    delegate: ColumnLayout {
                        Layout.preferredWidth: root.width
                        Layout.alignment: Qt.AlignHCenter
                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            padding: 5
                            text: modelData
                            font.pointSize: 12
                            background: Rectangle {
                                color: Qt.lighter("#3f6feb", 1.5)
                                radius: 3
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
                                model: Object.keys(pe_params[pModelData])
                                CKeyValue {
                                    key: modelData
                                    value: pe_params[pModelData][modelData]
                                }
                            }
                        }
                    }
                }

                Item {
                    Layout.preferredWidth: root.width
                    Layout.alignment: Qt.AlignHCenter
                    height: 20
                }
            }
        }
    }
}
