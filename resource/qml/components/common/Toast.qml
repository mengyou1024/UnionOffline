import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: toast

    property real time: defaultTime
    readonly property real defaultTime: 3000
    readonly property real fadeTime: 300
    property string msgType: "succed"
    property alias showImage: imgHeader.visible
    property real margin: 10
    property bool selfDestroying: true

    implicitWidth: layout.implicitWidth
    implicitHeight: layout.implicitHeight
    radius: 4
    opacity: 0
    color: "#ccd8d8"
    border.color: "#7c95c4"
    border.width: 1

    anchors.horizontalCenter: parent.horizontalCenter

    function show(text, duration) {
        theText.text = text
        if (typeof duration !== "undefined") {
            if (duration >= 2 * fadeTime)
                time = duration
            else
                time = 2 * fadeTime
        } else
            time = defaultTime
        anim.start()
    }

    ColumnLayout {
        id: layout
        spacing: 0
        Image {
            id: imgHeader
            Layout.topMargin: 10
            Layout.alignment: Qt.AlignHCenter
            sourceSize.width: 50
            sourceSize.height: 50
            source: "qrc:/img/" + msgType + ".png"
        }

        Text {
            topPadding: 5
            bottomPadding: 5
            leftPadding: 10
            rightPadding: 10
            id: theText
            text: ""
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 14
            color: "#000000"
        }
    }

    SequentialAnimation on opacity {
        id: anim

        running: false

        NumberAnimation {
            to: 0.9
            duration: fadeTime
        }

        PauseAnimation {
            duration: time - 2 * fadeTime
        }

        NumberAnimation {
            to: 0
            duration: fadeTime
        }

        onRunningChanged: {
            if (!running && selfDestroying) {
                toast.destroy()
            }
        }
    }
}
