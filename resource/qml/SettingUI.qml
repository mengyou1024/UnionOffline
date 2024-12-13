import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.settings 1.0
import "./components"

ApplicationWindow {

    width: 640
    height: 480

    LoggingCategory {
        id: tag
        name: "UI.Setting"
    }

    title: qsTr("设置")
    visible: true
    flags: Qt.Dialog
    modality: Qt.WindowModal
    background: Rectangle {
        color: "#e0eeee"
    }

    ScrollView {
        id: scroll_view
        anchors.fill: parent
        clip: true
        padding: 10

        ColumnLayout {
            id: layout
            clip: true
            width: scroll_view.width - scroll_view.padding * 2
            CArea {
                areaText: qsTr("软件更新")
                fontSize: 12
                Layout.fillWidth: true
                Layout.preferredHeight: 80
                Settings {
                    id: update_setting
                    category: "Upgrade"
                    fileName: "setting.ini"
                }

                ColumnLayout {
                    anchors.fill: parent
                    CheckBox {
                        Layout.leftMargin: 20
                        Layout.topMargin: 15
                        text: qsTr("软件启动时检查更新")
                        font.pointSize: 12

                        onClicked: {
                            if (checkState === Qt.Checked) {
                                update_setting.setValue("checkUpgrade", true)
                            } else if (checkState === Qt.Unchecked) {
                                update_setting.setValue("checkUpgrade", false)
                            }
                            update_setting.sync()
                        }

                        Component.onCompleted: {
                            if (update_setting.value("checkUpgrade", true) === true) {
                                checkState = Qt.Checked
                            } else {
                                checkState = Qt.Unchecked
                            }
                        }
                    }
                }
            }
        }
    }
}
