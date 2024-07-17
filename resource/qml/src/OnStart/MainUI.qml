import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    LoggingCategory {
        id: category
        name: "OnStart.main"
    }

    property alias controlTarget: cons.target
    property alias mainTarget: main_cons.target

    clip: true
    color: "transparent"
    id: control

    ScrollView {
        anchors.fill: parent
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical: ScrollBar {
            id: _scrollbar
        }

        ColumnLayout {
            width: control.width
            spacing: 30
            Image {
                source: controlTarget !== null ? controlTarget.imageSource : ""
                Layout.topMargin: 30
                Layout.alignment: Qt.AlignHCenter
            }

            Text {
                id: _text_area

                Layout.fillWidth: true
                Layout.margins: 50
                Layout.alignment: Qt.AlignHCenter

                textFormat: TextEdit.MarkdownText
                text: controlTarget !== null ? controlTarget.description : ""
                onTextChanged: {
                    _scrollbar.position = 0
                }

                font.pixelSize: 16
                wrapMode: Text.WordWrap
                lineHeight: 1.5
            }
        }
    }

    Connections {
        id: main_cons
        ignoreUnknownSignals: true
    }

    Connections {
        id: cons
        ignoreUnknownSignals: true
        target: null
    }
}
