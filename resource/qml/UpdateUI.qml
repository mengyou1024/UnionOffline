import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "./components"
import Morose.Utils 1.0

ApplicationWindow {
    minimumWidth: 640
    maximumWidth: 640
    minimumHeight: 480
    maximumHeight: 480

    LoggingCategory {
        id: tag
        name: "About"
    }

    title: qsTr("软件更新")
    visible: true
    flags: Qt.Dialog
    modality: Qt.WindowModal
    background: Rectangle {
        color: "#e0eeee"
    }

    property string remoteVersion
    property string updateMessage

    Component {
        id: text_elm
        Rectangle {
            property int columnSpan: 1
            property int treeDepth
            property alias text: text.text
            property bool isValue: false
            property alias textFormat: text.textFormat
            property alias textAlignment: text.horizontalAlignment

            function borderColor() {
                return "#7c95c4"
            }

            function backgroundColor() {
                switch (treeDepth) {
                case 0:
                    return "#afeeee"
                default:
                    return "white"
                }
            }

            function fontColor() {
                if (isValue) {
                    return "#666666"
                } else {
                    return "black"
                }
            }

            function fontPixelSize() {
                switch (treeDepth) {
                case 0:
                    return 18
                default:
                    return 14
                }
            }

            width: text.width
            height: text.height
            border.color: borderColor()
            Layout.columnSpan: columnSpan
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: backgroundColor()
            Text {
                id: text
                padding: 10
                color: fontColor()
                anchors.centerIn: parent
                font.pixelSize: fontPixelSize()
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    ScrollView {
        id: scroll_view
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: footer_layout.top
        anchors.margins: 10
        anchors.rightMargin: 0
        rightPadding: 10
        clip: true
        ColumnLayout {
            width: scroll_view.width - scroll_view.rightPadding
            GridLayout {
                id: layout
                clip: true
                columnSpacing: -1
                rowSpacing: -1
                columns: 2
            }

            GridLayout {
                id: layout_message
                clip: true
                columnSpacing: -1
                rowSpacing: -1
                columns: 1
            }
        }
    }

    ColumnLayout {
        id: footer_layout
        anchors.bottom: parent.bottom
        width: parent.width
        RowLayout {
            Layout.leftMargin: 30
            Layout.bottomMargin: 20
            Layout.rightMargin: 30
            Layout.alignment: Qt.AlignRight
            spacing: 15

            ProgressBar {
                id: progressBar
                Layout.fillWidth: true

                from: 0
                value: 0
                to: 1

                background: Rectangle {
                    id: background
                    implicitWidth: 100
                    implicitHeight: 32
                    border.color: "#7c95c4"
                    border.width: 1
                }

                contentItem: Item {
                    Rectangle {
                        anchors.margins: 1
                        width: progressBar.visualPosition * progressBar.width
                        height: progressBar.height
                        color: "#00e3e6"
                        border.color: "transparent"
                        border.width: 1
                    }
                }

                Binding {
                    target: progressBar
                    property: "value"
                    value: AppUpdater.downloadProgress
                }
            }

            CButton {
                id: btn
                state: "newVersionFound"
                states: [
                    State {
                        name: "newVersionFound"
                        PropertyChanges {
                            target: btn
                            text: qsTr("下载")
                        }
                    },
                    State {
                        name: "downloading"
                        PropertyChanges {
                            target: btn
                            enabled: false
                            backgroundColor: Qt.darker("#00e3e6", 3)
                            text: qsTr("下载中")
                        }
                    },
                    State {
                        name: "downloadCompleted"
                        PropertyChanges {
                            target: btn
                            text: qsTr("升级")
                        }
                    }
                ]

                onClicked: {
                    if (state === "newVersionFound") {
                        AppUpdater.doDownloadFile()
                        state = "downloading"
                    } else if (state === "downloadCompleted") {
                        console.log("开始升级")
                        AppUpdater.doUpgrade()
                    }
                }
                Connections {
                    target: AppUpdater
                    function onDownloadSuccess() {
                        console.log("升级包下载完成!")
                        btn.state = "downloadCompleted"
                    }
                }
            }

            CButton {
                text: qsTr("取消")
                onClicked: {
                    close()
                }
            }
        }
    }

    Component.onCompleted: {

        let generators = [{
                              "item": layout,
                              "values": [{
                                      "text": qsTr("当前版本"),
                                      "treeDepth": 0
                                  }, {
                                      "text": `${MOROSE_APP_VERSION}`,
                                      "treeDepth": 1,
                                      "isValue": true
                                  }, {
                                      "text": qsTr("新版本"),
                                      "treeDepth": 0
                                  }, {
                                      "text": `${remoteVersion}`,
                                      "treeDepth": 1,
                                      "isValue": true
                                  }]
                          }, {
                              "item": layout_message,
                              "values": [{
                                      "text": qsTr("更新信息"),
                                      "treeDepth": 0
                                  }, {
                                      "text": updateMessage || qsTr("无"),
                                      "treeDepth": 1,
                                      "isValue": true,
                                      "textFormat": Text.MarkdownText,
                                      "textAlignment": Qt.AlignLeft
                                  }]
                          }]

        for (const idx in generators) {
            for (const value_idx in generators[idx]["values"]) {
                generators[idx]["item"].children.push(text_elm.createObject(generators[idx]["item"], generators[idx]["values"][value_idx]))
            }
        }
    }
}
