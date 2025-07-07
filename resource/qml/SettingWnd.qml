import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.settings 1.0
import "./components"
import Union.Utils 1.0
import Morose.Utils 1.0

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
                visible: MOROSE_APP_ENABLE_UPGRADE === 1
                text: qsTr("软件更新")
                font.pointSize: 12
                Layout.fillWidth: true
                Layout.preferredWidth: layout_upgrade.width + horizontalPadding * 2
                Layout.preferredHeight: layout_upgrade.height + bottomPadding + topPadding

                ColumnLayout {
                    id: layout_upgrade
                    Settings {
                        id: update_setting
                        category: "Upgrade"
                        fileName: "setting.ini"
                    }

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
                text: qsTr("界面设置(需重启软件)")
                font.pointSize: 12
                Layout.fillWidth: true
                Layout.preferredWidth: layout_wnd_setting.width + horizontalPadding * 2
                Layout.preferredHeight: layout_wnd_setting.height + topPadding + bottomPadding
                ColumnLayout {
                    id: layout_wnd_setting

                    Settings {
                        id: wnd_setting
                        property bool enableHighDpi: true
                        property int highDpiRoundPolicy: 4
                        category: "Wnd"
                        fileName: "setting.ini"
                    }

                    GridLayout {
                        rows: 2
                        columns: 2
                        CheckBox {
                            Layout.alignment: Qt.AlignVCenter
                            Layout.leftMargin: 10
                            Layout.columnSpan: 2
                            text: qsTr("启用高DPI")
                            font.pointSize: 12

                            checkState: wnd_setting.enableHighDpi ? Qt.Checked : Qt.Unchecked

                            onCheckStateChanged: {
                                if (checkState == Qt.Checked) {
                                    wnd_setting.enableHighDpi = true
                                } else {
                                    wnd_setting.enableHighDpi = false
                                }
                            }
                        }

                        Text {
                            Layout.leftMargin: 20
                            text: qsTr("高DPI策略:")
                            font.pointSize: 12
                        }

                        CComboBox {
                            Layout.alignment: Qt.AlignVCenter
                            Layout.leftMargin: 10
                            implicitWidth: 210
                            implicitHeight: 32
                            model: [qsTr("四舍五入"), qsTr("向上取整"), qsTr("向下取整"), qsTr("高于0.75向上取整"), qsTr("不取整")]
                            onActivated: index => {
                                             wnd_setting.highDpiRoundPolicy = index + 1
                                         }

                            Component.onCompleted: {
                                currentIndex = wnd_setting.highDpiRoundPolicy - 1
                            }
                        }
                    }
                }
            }

            CArea {
                visible: MOROSE_ENABLE_MULTI_LANGUATE_FEATURE === 1
                text: qsTr("语言")
                font.pointSize: 12
                Layout.fillWidth: true
                Layout.preferredWidth: layout_language.width + horizontalPadding * 2
                Layout.preferredHeight: layout_language.height + topPadding + bottomPadding
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

            CArea {
                text: qsTr("T8/390N")
                font.pointSize: 12
                Layout.fillWidth: true
                Layout.preferredWidth: layout_mdat_setting.width + horizontalPadding * 2
                Layout.preferredHeight: layout_mdat_setting.height + topPadding + bottomPadding
                ColumnLayout {
                    id: layout_mdat_setting

                    CheckBox {
                        Layout.alignment: Qt.AlignVCenter
                        Layout.leftMargin: 10
                        text: qsTr("重新计算波门数据")
                        font.pointSize: 12

                        checkState: mdat_setting.calculateGateResult ? Qt.Checked : Qt.Unchecked

                        onCheckStateChanged: {
                            if (checkState == Qt.Checked) {
                                mdat_setting.calculateGateResult = true
                            } else {
                                mdat_setting.calculateGateResult = false
                            }
                        }
                    }

                    CheckBox {
                        Layout.alignment: Qt.AlignVCenter
                        Layout.leftMargin: 10
                        text: qsTr("显示C扫分层线")
                        font.pointSize: 12

                        checkState: mdat_setting.displayCScanLayerLine ? Qt.Checked : Qt.Unchecked

                        onCheckStateChanged: {
                            if (checkState == Qt.Checked) {
                                mdat_setting.displayCScanLayerLine = true
                            } else {
                                mdat_setting.displayCScanLayerLine = false
                            }
                        }
                    }

                    CheckBox {
                        Layout.alignment: Qt.AlignVCenter
                        Layout.leftMargin: 10
                        text: qsTr("B/C扫坐标轴取整")
                        font.pointSize: 12

                        checkState: mdat_setting.roundToInt ? Qt.Checked : Qt.Unchecked

                        onCheckStateChanged: {
                            if (checkState == Qt.Checked) {
                                mdat_setting.roundToInt = true
                            } else {
                                mdat_setting.roundToInt = false
                            }
                        }
                    }

                    CheckBox {
                        Layout.alignment: Qt.AlignVCenter
                        Layout.leftMargin: 10
                        text: qsTr("B扫图像平滑")
                        font.pointSize: 12

                        checkState: mdat_setting.bScanImageSmoothing ? Qt.Checked : Qt.Unchecked

                        onCheckStateChanged: {
                            if (checkState == Qt.Checked) {
                                mdat_setting.bScanImageSmoothing = true
                            } else {
                                mdat_setting.bScanImageSmoothing = false
                            }
                        }
                    }

                    CheckBox {
                        Layout.alignment: Qt.AlignVCenter
                        Layout.leftMargin: 10
                        text: qsTr("C扫图像平滑")
                        font.pointSize: 12

                        checkState: mdat_setting.cScanImageSmoothing ? Qt.Checked : Qt.Unchecked

                        onCheckStateChanged: {
                            if (checkState == Qt.Checked) {
                                mdat_setting.cScanImageSmoothing = true
                            } else {
                                mdat_setting.cScanImageSmoothing = false
                            }
                        }
                    }
                }
            }
        }
    }
}
