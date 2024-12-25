import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtCharts 2.15
import QtQuick.Layouts 1.15
import Qt.labs.folderlistmodel 2.15
import Qt.labs.platform 1.1
import Qt.labs.settings 1.1
import Morose.Utils 1.0
import "./components"

ApplicationWindow {
    LoggingCategory {
        id: category
        name: "main"
    }

    minimumWidth: 1636
    minimumHeight: 870
    visible: true
    id: wnd_main
    title: `${MOROSE_APP_NAME_ZH_CN}`

    property string mainUIType: "Unknow"

    signal btnParamClicked
    signal listviewIndexChanged(var index, var list_view)
    signal openFile(string file)
    signal splitViewResizingChanged(bool resizing)
    property bool listview_unfold: true
    property bool controlui_unflod: true
    property string mainUi_name: ""
    property string fileName2Open: ""

    Conditional {
        id: listview_updated
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        ToolBar {
            id: tool_bar
            Layout.fillWidth: true
            spacing: 0
            background: Rectangle {
                color: "#ccd8d8"
            }

            RowLayout {

                CIconButton {
                    img_src: "qrc:/img/folder.png"
                    btn_txt: qsTr("打开")
                    onClicked: file_dialog.open()
                }

                CIconButton {
                    img_src: "qrc:/img/remove.png"
                    btn_txt: qsTr("清除")
                    onClicked: {
                        if (mainUIType === "Unknow") {
                            showToast(qsTr("当前未打开文件"))
                        }
                        actionMainType("Unknow")
                    }
                }

                CIconButton {
                    img_src: "qrc:/img/param.png"
                    btn_txt: qsTr("工艺参数")
                    onClicked: {
                        if (mainUIType === "Unknow") {
                            showToast(qsTr("当前未打开文件"))
                        }
                        btnParamClicked()
                    }
                }

                CIconButton {
                    // tag: disable communication
                    visible: false
                    img_src: "qrc:/img/usb.png"
                    btn_txt: qsTr("通讯")
                    onClicked: {
                        let comp = Qt.createComponent("CommunicateWnd.qml")
                        if (comp.status === Component.Ready) {
                            let com_wnd = comp.createObject(parent)
                            com_wnd.closing.connect(() => {
                                                        comp.destroy()
                                                        com_wnd.destroy()
                                                    })
                            com_wnd.saveTemporaryFile.connect(filePath => {
                                                                  console.log(category, "on saveTemporaryFile trigger file:", filePath)
                                                                  actionMainType(getMainUITypeIndex(filePath), filePath)
                                                                  listView.currentIndex = -1
                                                              })
                        } else if (comp.status === Component.Error) {
                            console.error(category, comp.errorString())
                        }
                    }
                }

                CIconButton {
                    img_src: "qrc:/img/setting.png"
                    btn_txt: qsTr("设置")
                    onClicked: {
                        let comp = Qt.createComponent("SettingWnd.qml")
                        if (comp.status === Component.Ready) {
                            let setting_wind = comp.createObject(parent)
                            setting_wind.closing.connect(() => {
                                                             comp.destroy()
                                                             setting_wind.destroy()
                                                         })
                        } else if (comp.status === Component.Error) {
                            console.error(category, comp.errorString())
                        }
                    }
                }

                CIconButton {
                    img_src: "qrc:/img/help.png"
                    btn_txt: qsTr("帮助/关于软件")
                    onClicked: {
                        let helpFile = "qrc:/qml/src/" + mainUIType + "/HelpWnd.qml"
                        console.log("helpFile:", helpFile)
                        if (!FileExists.isFileExists(helpFile.substring(3))) {
                            helpFile = "AboutWnd.qml"
                        }

                        let comp = Qt.createComponent(helpFile)
                        if (comp.status === Component.Ready) {
                            let about_wind = comp.createObject(parent)
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

        RowLayout {
            id: row_layout
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.leftMargin: 1
            Layout.rightMargin: 1
            spacing: 2

            Rectangle {
                visible: listview_unfold
                id: rect_list
                Layout.preferredWidth: 240
                Layout.fillHeight: true
                color: "#e0eeee"
                radius: 5
                clip: true
                border.color: "#7c95c4"
                border.width: 2
                ListView {
                    id: listView
                    anchors.fill: parent
                    anchors.margins: 2
                    clip: true
                    property color highlightColor: "#00e3e6"
                    model: FolderListModel {
                        id: folder_list
                        showDirs: false
                        nameFilters: FOLDERLISTMODEL_NAMEFILTER
                        caseSensitive: false
                        folder: file_dialog.folder

                        onFolderChanged: {
                            listview_updated.conditionRlease()
                        }
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
                            // @disable-check M16
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
                border.color: "#7c95c4"
                border.width: 2
                Layout.fillWidth: true
                Layout.fillHeight: true

                BusyIndicator {
                    anchors.centerIn: loader_ui
                    running: loader_ui.status === Loader.Loading
                }

                Loader {
                    id: loader_ui
                    clip: true
                    anchors.margins: 2
                    anchors.fill: parent
                    source: ""
                    asynchronous: true
                    Component.onCompleted: {
                        console.log(category, `loader.main width:${width} height:${height}`)
                    }
                    onLoaded: {
                        loader_ui.item.controlTarget = loader_ctrl.item
                        loader_ui.item.mainTarget = wnd_main
                        if (mainUIType !== "Unknow") {
                            console.log("main ui loaded")
                            console.log(category, "openFile, filePath:", fileName2Open)
                            openFile(fileName2Open)
                        }
                    }
                }

                Rectangle {
                    id: listview_unfold_ctrl
                    width: 32
                    height: 32
                    z: 1
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    radius: 16
                    clip: true
                    color: "#ccd8d8"
                    antialiasing: true
                    opacity: 0.2
                    Image {
                        anchors.centerIn: parent
                        source: "qrc:/img/arrow.png"
                        rotation: listview_unfold ? 90 : -90
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                listview_unfold = !listview_unfold
                            }
                            onEntered: {
                                listview_unfold_ctrl.opacity = 1
                                listview_unfold_ctrl.color = Qt.darker("#ccd8d8", 1.2)
                            }
                            onExited: {
                                listview_unfold_ctrl.opacity = 0.2
                                listview_unfold_ctrl.color = "#ccd8d8"
                            }
                            cursorShape: Qt.PointingHandCursor
                        }
                        Behavior on rotation {
                            NumberAnimation {
                                duration: 300
                            }
                        }
                    }
                }

                Rectangle {
                    id: controlui_unfold_ctrl
                    width: 32
                    height: 32
                    z: 1
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 5
                    radius: 16
                    clip: true
                    color: "#ccd8d8"
                    antialiasing: true
                    opacity: 0.2
                    Image {
                        anchors.centerIn: parent
                        source: "qrc:/img/arrow.png"
                        rotation: controlui_unflod ? -90 : 90
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                controlui_unflod = !controlui_unflod
                            }
                            onEntered: {
                                controlui_unfold_ctrl.opacity = 1
                                controlui_unfold_ctrl.color = Qt.darker("#ccd8d8", 1.2)
                            }
                            onExited: {
                                controlui_unfold_ctrl.opacity = 0.2
                                controlui_unfold_ctrl.color = "#ccd8d8"
                            }
                            cursorShape: Qt.PointingHandCursor
                        }
                        Behavior on rotation {
                            NumberAnimation {
                                duration: 300
                            }
                        }
                    }
                }
            }

            Rectangle {
                visible: controlui_unflod
                Layout.preferredWidth: loader_ctrl.implicitWidth
                Layout.fillHeight: true
                color: "transparent"
                radius: 5
                border.color: "#7c95c4"
                border.width: 2

                BusyIndicator {
                    anchors.centerIn: loader_ctrl
                    running: loader_ctrl.status === Loader.Loading
                }

                Loader {
                    id: loader_ctrl
                    clip: true
                    anchors.fill: parent
                    anchors.centerIn: parent
                    anchors.topMargin: 10
                    anchors.bottomMargin: 10
                    asynchronous: true

                    source: ""
                    onLoaded: {
                        loader_ui.source = mainUi_name
                    }
                }
            }
        }

        Rectangle {
            Layout.preferredHeight: 20
            Layout.fillWidth: true
            color: "#ccd8d8"
            RowLayout {
                spacing: 20
                Text {
                    Layout.fillHeight: true
                    Layout.leftMargin: 5
                    text: qsTr("友联科技, 欢迎使用")
                    verticalAlignment: Text.AlignVCenter
                }

                Text {
                    Layout.fillHeight: true
                    text: qsTr("版权所有© 南通友联数码技术开发有限公司")
                    verticalAlignment: Text.AlignVCenter
                }
            }

            ProgressBar {
                function isLoading() {
                    return loader_ctrl.status === Loader.Loading || loader_ui.status === Loader.Loading
                }
                height: parent.height
                width: 100
                anchors.right: parent.right
                anchors.rightMargin: 10
                value: getValue()

                function getValue() {
                    if (isLoading()) {
                        return (loader_ctrl.progress + loader_ui.progress) / 2
                    } else if (GlobalCppProgress.enable) {
                        return GlobalCppProgress.progress
                    } else {
                        return 0
                    }
                }
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
            listview_unfold = true
            listview_unfold_ctrl.visible = true
            let filePath = String(file_dialog.file).substring(8)
            actionMainType(getMainUITypeIndex(filePath), filePath)
        }
    }

    function getMainUITypeIndex(fileName) {
        let suffix = fileName.substring(fileName.lastIndexOf(".")).toLowerCase()
        console.log(category, `open file suffix:${suffix}, get ui type: ${MAINUI_MAP[suffix]}`)
        let ret = MAINUI_MAP[suffix]
        if (ret === undefined) {
            ret = "Unknow"
            showFailed("打开出错")
            console.warn(category, `open file:${fileName} error, unknow suffix:${suffix}`)
        }
        return ret
    }

    function actionMainType(type, filePath) {
        console.log(category, "load ui type:", type)
        let lastMainUIType = mainUIType
        mainUIType = type
        if (type === "Unknow") {
            listview_unfold = false
            listview_unfold_ctrl.visible = false
            loader_ctrl.source = "src/OnStart/ControlUI.qml"
            mainUi_name = "src/OnStart/MainUI.qml"
        } else {
            if (lastMainUIType !== mainUIType) {
                loader_ctrl.source = "src/" + type + "/ControlUI.qml"
                mainUi_name = "src/" + type + "/MainUI.qml"
                fileName2Open = filePath
            } else {
                let index = folder_list.indexOf("file:///" + filePath)
                if (index < 0) {
                    folder_list.folder = ""
                    folder_list.folder = Qt.binding(() => {
                                                        return file_dialog.folder
                                                    })
                    listview_updated.waitCondition()
                    index = folder_list.indexOf("file:///" + filePath)
                    listView.currentIndex = index
                }

                listviewIndexChanged(index, folder_list)
            }
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

    function showToast(text) {
        toast.show(text)
    }

    Connections {
        id: update_connections
        property var update_wnd
        target: AppUpdater

        function onNewVersionFound(ver, msg) {
            var comp = Qt.createComponent("qrc:/qml/UpdateWnd.qml")
            if (comp.status === Component.Ready) {
                update_connections.update_wnd = comp.createObject(wnd_main, {
                                                                      "remoteVersion": ver,
                                                                      "updateMessage": msg
                                                                  })

                update_connections.update_wnd.closing.connect(() => {
                                                                  comp.destroy()
                                                                  update_connections.update_wnd.destroy()
                                                                  update_connections.update_wnd = null
                                                              })
            } else if (comp.status === Component.Error) {
                console.error(category, comp.errorString())
            }
        }

        function onDownloadSuccess() {
            if (update_connections.update_wnd === null) {
                console.log("更新文件下载成功")
                AppUpdater.doUpgrade()
            }
        }
    }

    Component.onCompleted: {
        actionMainType("Unknow")
        showMaximized()
        AppUpdater.uiInitOK = true
    }
}
