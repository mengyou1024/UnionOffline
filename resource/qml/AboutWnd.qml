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
        name: "UI.About"
    }

    title: qsTr("关于软件")
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
        anchors.fill: parent
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
        }
    }

    Component.onCompleted: {

        let generators = [{
                              "item": layout,
                              "values": [{
                                      "text": qsTr("软件版本"),
                                      "treeDepth": 0
                                  }, {
                                      "text": MOROSE_APP_VERSION,
                                      "treeDepth": 1,
                                      "isValue": true
                                  }, {
                                      "text": qsTr("编译日期"),
                                      "treeDepth": 0
                                  }, {
                                      "text": MOROSE_APP_BUILD_DATE,
                                      "treeDepth": 1,
                                      "isValue": true
                                  }, {
                                      "text": qsTr("编译时间"),
                                      "treeDepth": 0
                                  }, {
                                      "text": MOROSE_APP_BUILD_TIME,
                                      "treeDepth": 1,
                                      "isValue": true
                                  }, {
                                      "text": qsTr("流水线编译"),
                                      "treeDepth": 0
                                  }, {
                                      "text": MOROSE_APP_BUILD_BY_WORKFLOW === 1 ? qsTr("是") : qsTr("否"),
                                      "treeDepth": 1,
                                      "isValue": true
                                  }, {
                                      "text": "HASH",
                                      "treeDepth": 0
                                  }, {
                                      "text": `${MOROSE_APP_COMMIT_HASH}`,
                                      "treeDepth": 1,
                                      "isValue": true
                                  }]
                          }]

        for (const idx in generators) {
            for (const value_idx in generators[idx]["values"]) {
                generators[idx]["item"].children.push(text_elm.createObject(generators[idx]["item"], generators[idx]["values"][value_idx]))
            }
        }
    }
}
