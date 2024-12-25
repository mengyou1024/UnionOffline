import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.settings 1.0
import "./components"
import Union.Utils 1.0

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
            clip: true
            width: scroll_view.width - scroll_view.padding * 2
            CArea {
                areaText: qsTr("软件更新")
                fontSize: 12
                Layout.fillWidth: true
                Layout.preferredHeight: layout_upgrade.height + heightFix
                Settings {
                    id: update_setting
                    category: "Upgrade"
                    fileName: "setting.ini"
                }

                ColumnLayout {
                    id: layout_upgrade
                    CheckBox {
                        Layout.alignment: Qt.AlignVCenter
                        Layout.leftMargin: 10
                        text: qsTr("软件启动时检查更新")
                        font.pointSize: 12
                        onClicked: {
                            if (checkState === Qt.Checked) {
                                update_setting.setValue("checkUpgrade", "true")
                            } else if (checkState === Qt.Unchecked) {
                                update_setting.setValue("checkUpgrade", "false")
                            }
                            update_setting.sync()
                        }

                        Component.onCompleted: {
                            let check_upgrade = update_setting.value("checkUpgrade", "true")
                            if (check_upgrade === "true") {
                                checkState = Qt.Checked
                            } else {
                                checkState = Qt.Unchecked
                            }
                        }
                    }
                }
            }
            CArea {
                // tag: disable multi-language
                visible: false
                areaText: qsTr("语言")
                fontSize: 12
                Layout.fillWidth: true
                Layout.preferredHeight: layout_language.height + heightFix
                ColumnLayout {
                    id: layout_language
                    CComboBox {
                        Layout.alignment: Qt.AlignVCenter
                        Layout.leftMargin: 10
                        implicitWidth: 140
                        implicitHeight: 32
                        model: Translator.languageNameList()
                        onActivated: index => {
                                         Translator.languageSelected = index
                                     }

                        Component.onCompleted: {
                            currentIndex = Translator.languageSelected
                        }
                    }
                }
            }
        }
    }
}
