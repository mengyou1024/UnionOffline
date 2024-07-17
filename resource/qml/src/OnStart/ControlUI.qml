import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import "../../components"
import "./components"
import Morose.Utils 1.0

Rectangle {
    LoggingCategory {
        id: category
        name: "OnStart.ctrl"
    }
    color: "transparent"
    property alias description: fileReader.string
    property string imageSource: ""

    FileReader {
        id: fileReader
        fileName: ""
    }

    ListView {
        id: _listView
        anchors.fill: parent
        anchors.margins: 5
        model: FileExists.listDir(":/img/instruments")
        property color highlightColor: "#00e3e6"

        highlight: Rectangle {
            border.color: _listView.highlightColor
            border.width: 2
            radius: 5
            color: "transparent"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        ScrollBar.vertical: ScrollBar {}

        highlightMoveDuration: 0
        highlightMoveVelocity: -1
        highlightResizeDuration: 0
        highlightResizeVelocity: -1

        delegate: ImageCard {
            property color normalColor: "#e0eeee"
            anchors.horizontalCenter: parent.horizontalCenter
            imgSource: `qrc:/img/instruments/${modelData}/cover.png`
            text: modelData
            width: parent.width
            border.width: 2
            border.color: "transparent"
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    if (index !== _listView.currentIndex) {
                        _listView.currentIndex = index
                        fileReader.fileName = `:/img/instruments/${modelData}/description.md`
                        imageSource = `qrc:/img/instruments/${modelData}/image.png`
                    }
                }

                onEntered: {
                    if (_listView.currentIndex !== index) {
                        parent.border.color = Qt.lighter(_listView.highlightColor, 1.1)
                    }
                }
                onExited: {
                    parent.border.color = "transparent"
                }
            }

            Component.onCompleted: {
                if (index === 0) {
                    fileReader.fileName = `:/img/instruments/${modelData}/description.md`
                    imageSource = `qrc:/img/instruments/${modelData}/image.png`
                }
            }
        }
    }
}
