import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtCharts 2.15
import QtQuick.Layouts 1.15
import Qt.labs.folderlistmodel 2.15
import Qt.labs.platform 1.1
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
    signal splitViewResizingChanged(bool resizing)

    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        ToolBar {
            id: tool_bar
            Layout.fillWidth: true
            spacing: 0
            background: Rectangle {
                color: "#e1e1e1"
            }

            RowLayout {

                CIconButton {
                    img_src: "qrc:/img/folder.png"
                    btn_txt: qsTr("打开")
                    height: parent.height
                    onClicked: file_dialog.open()
                }

                CIconButton {
                    img_src: "qrc:/img/ava_clear.png"
                    btn_txt: qsTr("清除")
                    height: parent.height
                    onClicked: actionMainType("Unknow")
                }

                CIconButton {
                    implicitWidth: 60
                    img_src: "qrc:/img/param.png"
                    btn_txt: qsTr("工艺参数")
                    height: parent.height
                    onClicked: btnParamClicked()
                }

                CIconButton {
                    implicitWidth: 80
                    img_src: "qrc:/img/USB_51.png"
                    btn_txt: qsTr("通讯")
                    height: parent.height
                }

                CIconButton {
                    img_src: "qrc:/img/config-language.ico"
                    btn_txt: qsTr("帮助")
                    height: parent.height
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

        SplitView {
            id: split_view
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.leftMargin: 1
            Layout.rightMargin: 1
            handle: Rectangle {
                id: handleDelegate
                implicitWidth: 6
                color: SplitHandle.pressed ? "#c6e2ff" : (SplitHandle.hovered ? Qt.lighter("#c6e2ff", 1.1) : "#c6e2ff")

                containmentMask: Item {
                    x: (handleDelegate.width - width) / 2
                    width: 20
                    height: split_view.height
                }
            }

            Rectangle {
                id: rect_list
                SplitView.preferredWidth: 240
                SplitView.minimumWidth: 240
                SplitView.maximumWidth: 240
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

                    highlightMoveDuration: 0
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
                            cursorShape: Qt.PointingHandCursor
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                if (index === listView.currentIndex) {
                                    return
                                }
                                listView.currentIndex = index
                                color = "transparent"
                                console.log(category, "listview index changed, index:", index, "filepath:", folder_list.get(index, "filePath"))
                                let filePath = folder_list.get(index, "filePath")
                                let uiType = getMainUITypeIndex(filePath)
                                console.log(category, "uiType:", uiType)
                                if (uiType === mainUIType) {
                                    listviewIndexChanged(index, folder_list)
                                } else {
                                    actionMainType(uiType, filePath)
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
                    source: ""
                    Component.onCompleted: {
                        console.log(category, `loader.main width:${width} height:${height}`)
                    }
                }
            }

            Rectangle {
                SplitView.minimumWidth: 300
                SplitView.maximumWidth: 300
                SplitView.fillHeight: true
                color: "transparent"
                radius: 5
                border.color: "#3f6feb"
                border.width: 2

                Loader {
                    id: loader_ctrl
                    clip: true
                    anchors.fill: parent
                    anchors.margins: 10
                    anchors.centerIn: parent

                    source: ""
                    onLoaded: {
                        console.log(`loader height:${loader_ctrl.item.height} width: ${loader_ctrl.item.width}`)
                    }
                }
            }
            onResizingChanged: {
                splitViewResizingChanged(resizing)
            }
        }

        Rectangle {
            Layout.preferredHeight: 20
            color: "#b0e2ff"
            Text {
                x: 5
                height: parent.height
                text: qsTr("友联科技, 欢迎使用")
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    background: Rectangle {
        color: "#e0eeee"
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

    function getMainUITypeIndex(fileName) {
        let suffix = fileName.substring(fileName.lastIndexOf(".")).toLowerCase()
        console.log(category, `open file suffix:${suffix}, get ui type: ${MAINUI_MAP[suffix]}`)
        var ret = MAINUI_MAP[suffix]
        if (ret === undefined) {
            ret = "Unknow"
            showFailed("打开出错")
            console.warn(category, `open file:${fileName} error, unknow suffix:${suffix}`)
        }
        return ret
    }

    function actionMainType(type, filePath) {
        console.log(category, "load ui type:", type)
        mainUIType = type
        if (type === "Unknow") {
            listView.visible = false
            loader_ui.source = ""
            loader_ctrl.source = ""
        } else {
            loader_ctrl.source = "src/" + type + "/ControlUI.qml"
            loader_ui.source = "src/" + type + "/MainUI.qml"
            loader_ui.item.controlTarget = loader_ctrl.item
            loader_ui.item.mainTarget = wnd_main
            console.log(category, "AScan filePath:", filePath)
            openFile(filePath)
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

    Component.onCompleted: {

        // showMaximized()
    }
}
