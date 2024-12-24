import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtCharts 2.15
import QtQuick.Layouts 1.15
import Qt.labs.folderlistmodel 2.15
import Qt.labs.platform 1.1
import Qt.labs.settings 1.1
import Morose.Utils 1.0

ApplicationWindow {
    LoggingCategory {
        id: category
        name: "JsonTable"
    }

    property var tableData
    property int treeDepth: 0
    property int treeDepthBias: 0

    id: control
    flags: Qt.Dialog
    width: 640
    height: 480
    visible: true

    ScrollView {
        id: scroll_view
        anchors.fill: parent
        clip: true
        padding: 10
        GridLayout {
            clip: true
            width: scroll_view.width - scroll_view.padding * 2
            id: layout
            columnSpacing: -1
            rowSpacing: -1
            columns: 4
        }
    }

    Component {
        id: text_elm

        Rectangle {
            property int columnSpan: 1
            property int treeDepth
            property alias text: text.text
            property bool isValue: false

            function borderColor() {
                return "#7c95c4"
            }

            function backgroundColor() {
                switch (treeDepth + treeDepthBias) {
                case 0:
                    return "#afeeee"
                case 1:
                    return "#e0eeee"
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
                switch (treeDepth + treeDepthBias) {
                case 0:
                    return 24
                case 1:
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

    function createModel(key, value) {
        if (value instanceof Object) {
            return text_elm.createObject(control, {
                                             "columnSpan": 4,
                                             "text": key,
                                             "treeDepth": treeDepth
                                         })
        } else {
            return [text_elm.createObject(control, {
                                              "columnSpan": 1,
                                              "text": key,
                                              "treeDepth": treeDepth
                                          }), text_elm.createObject(control, {
                                                                        "columnSpan": 1,
                                                                        "text": value,
                                                                        "treeDepth": treeDepth,
                                                                        "isValue": true
                                                                    })]
        }
    }

    function createList(data) {
        let list = []
        let keys = Object.keys(data)

        let key_value_count = 0
        for (var i = 0; i < keys.length; i++) {
            let item = data[keys[i]]
            if (item instanceof Object) {
                list.push(createModel(keys[i], item))
                treeDepth++
                list = list.concat(createList(item))
                treeDepth--
            } else {
                key_value_count++
                list = list.concat(createModel(keys[i], String(item)))
            }
        }

        if (key_value_count % 2) {
            list = list.concat(createModel("", ""))
        }

        return list
    }

    function generateTable() {
        layout.children = []
        let textList = createList(tableData)
        for (var i = 0; i < textList.length; i++) {
            layout.children.push(textList[i])
        }
    }

    Component.onCompleted: {
        generateTable()
    }
}
