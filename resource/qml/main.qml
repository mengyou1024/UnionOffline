import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtCharts
import QtQuick.Layouts
import Qt.labs.folderlistmodel
import Qt.labs.platform
import "./src/AScan"
import "./components"

ApplicationWindow {
    LoggingCategory {
        id: category
        name: "main"
    }

    minimumWidth: 1636
    minimumHeight: 864
    visible: true
    id: wnd_main
    title: qsTr("通用离线分析软件")

    property string mainUIType: "Unknow"

    signal btnParamClicked
    signal listviewIndexChanged(var index, var list_view)
    signal openFile(string file)

    ToolBar {
        id: tool_bar
        width: parent.width
        spacing: 0
        background: Rectangle {
            color: "#e1e1e1"
        }

        RowLayout {
            ToolButton {
                width: 55
                height: 41
                icon.height: 41
                icon.width: 55
                icon.source: "qrc:/img/folder.png"
                onClicked: {
                    file_dialog.open()
                }
                text: qsTr("打开")
                display: AbstractButton.TextUnderIcon
            }
            ToolButton {
                width: 55
                height: 41
                icon.height: 41
                icon.width: 55
                icon.source: "qrc:/img/ava_clear.png"
                text: qsTr("清除")
                display: AbstractButton.TextUnderIcon
                onClicked: {
                    actionMainType("Unknow")
                }
            }
            ToolButton {
                width: 55
                height: 41
                icon.height: 41
                icon.width: 55
                icon.source: "qrc:/img/param.png"
                text: qsTr("工艺参数")
                display: AbstractButton.TextUnderIcon
                onClicked: {
                    btnParamClicked()
                }
            }
            ToolButton {
                width: 55
                height: 41
                icon.height: 41
                icon.width: 55
                icon.source: "qrc:/img/USB_51.png"
                text: qsTr("通讯")
                display: AbstractButton.TextUnderIcon
            }
            ToolButton {
                width: 55
                height: 41
                icon.height: 41
                icon.width: 55
                icon.source: "qrc:/img/config-language.ico"
                text: qsTr("帮助")
                display: AbstractButton.TextUnderIcon

                onClicked: {
                    var comp = Qt.createComponent("About.qml")
                    if (comp.status === Component.Ready) {
                        var about_wind = comp.createObject(parent)
                        about_wind.winClose.connect(param => {
                                                        console.info(category, "receive signal: winClose, parameter:", param)
                                                    })
                        about_wind.closing.connect(() => {
                                                       comp.destroy()
                                                       about_wind.destroy()
                                                   })
                    } else if (comp.status === Component.Error) {
                        console.error(category, comp.errorString())
                    }
                }
            }
        }
    }

    background: Rectangle {
        color: "#e0eeee"
    }

    SplitView {
        id: split_view

        anchors.top: tool_bar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.leftMargin: 1
        anchors.rightMargin: 1

        handle: Rectangle {
            id: handleDelegate
            implicitWidth: 6
            color: SplitHandle.pressed ? "#c6e2ff" : (SplitHandle.hovered ? Qt.lighter("#c6e2ff", 1.1) : "#c6e2ff")

            // @disable-check M16
            containmentMask: Item {
                x: (handleDelegate.width - width) / 2
                width: 20
                height: split_view.height
            }
        }

        Rectangle {
            id: rect_list
            SplitView.preferredWidth: 240
            color: "#e0eeee"
            radius: 5
            clip: true
            border.color: "#3f6feb"
            border.width: 2
            ListView {
                id: listView
                anchors.fill: parent
                anchors.margins: 2
                visible: false
                property color highlightColor: "#00e3e6"
                model: FolderListModel {
                    id: folder_list
                    showDirs: false
                    nameFilters: FOLDERLISTMODEL_NAMEFILTER
                    caseSensitive: false
                    folder: file_dialog.folder
                }

                highlight: Rectangle {
                    border.color: "transparent"
                    border.width: 2
                    radius: 5
                    color: listView.highlightColor
                }

                ScrollBar.vertical: ScrollBar {}

                highlightMoveDuration: 150
                highlightMoveVelocity: -1
                highlightResizeDuration: 0
                highlightResizeVelocity: -1

                delegate: Rectangle {
                    color: "transparent"
                    width: ListView.view.width
                    height: 35
                    radius: 5
                    property string fN: t.text
                    Text {
                        id: t
                        text: fileName
                        anchors.centerIn: parent
                        font.pixelSize: 16
                    }
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            if (index === listView.currentIndex) {
                                return
                            }
                            listView.currentIndex = index
                            color = "transparent"
                            console.log(category, "listview index changed, index:", index, "filepath:", folder_list.get(index, "filePath"))
                            let uiType = getMainUITypeIndex(folder_list.get(index, "filePath"))
                            if (uiType === mainUIType) {
                                listviewIndexChanged(index, folder_list)
                            } else {
                                actionMainType(uiType)
                            }
                        }
                        onEntered: {
                            if (listView.currentIndex === index) {
                                return
                            }
                            color = Qt.lighter(listView.highlightColor, 1.1)
                        }
                        onExited: {
                            color = "transparent"
                        }
                    }
                }
            }
        }

        Rectangle {
            color: "transparent"
            radius: 5
            border.color: "#3f6feb"
            border.width: 2
            SplitView.fillWidth: true
            Loader {
                id: loader_ui
                clip: true
                anchors.margins: 2
                anchors.fill: parent
            }
        }

        Rectangle {
            SplitView.preferredWidth: 240
            color: "transparent"
            radius: 5
            border.color: "#3f6feb"
            border.width: 2
            Loader {
                id: loader_ctrl
                clip: true
                anchors.fill: parent
                anchors.margins: 2
            }
        }
    }

    Rectangle {
        height: 20
        width: parent.width
        color: "#b0e2ff"
        anchors.top: split_view.bottom
        Text {
            height: parent.height
            text: qsTr("友联科技, 欢迎使用")
            verticalAlignment: Text.AlignVCenter
        }
    }

    FileDialog {
        id: file_dialog
        title: qsTr("选择一个文件")
        nameFilters: FILEDIALOG_NAMEFILTER
        onAccepted: {
            listView.currentIndex = folder_list.indexOf(file_dialog.file)
            listView.visible = true
            let filePath = String(file_dialog.file).substring(8)
            actionMainType(getMainUITypeIndex(filePath), filePath)
        }
    }

    Component.onCompleted: {
        showMaximized()
    }

    function getMainUITypeIndex(fileName) {
        let suffix = fileName.substring(fileName.lastIndexOf(".")).toLowerCase()
        console.log(category, `open file suffix:${suffix}, get ui type: ${MAINUI_MAP[suffix]}`)
        var ret = MAINUI_MAP[suffix]
        if (ret === undefined) {
            ret = "Unknow"
            console.warn(category, "unknow suffix:", suffix)
        }
        return ret
    }

    function actionMainType(type, filePath) {
        console.log(category, "load ui type:", type)
        mainUIType = type
        if (type === "AScan") {
            loader_ctrl.source = "src/AScan/ControlUI.qml"
            loader_ui.source = "src/AScan/MainUI.qml"
            loader_ui.item.controlTarget = loader_ctrl.item
            loader_ui.item.mainTarget = wnd_main
            openFile(filePath)
        } else if (type === "Unknow") {
            listView.visible = false
            loader_ui.source = ""
            loader_ctrl.source = ""
        } else {
            console.warn(category, "unknow ui:", type)
        }
    }

    CToast {
        id: toast
    }

    function showSuccessful(text) {
        toast.showSuccessful(text)
    }

    function showFailed(text) {
        toast.showFailed(text)
    }
}
