import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: rec

    property alias img_src: icon.source
    property alias btn_txt: text.text

    property color clr_enter: Qt.lighter("#e1e1e1", 1.1)
    property color clr_exit: "transparent"
    property color clr_click: "#aba9b2"
    property color clr_release: "#ffffff"

    //自定义点击信号
    signal clicked

    implicitWidth: Math.max(text.implicitWidth, icon.width, 48)
    implicitHeight: text.implicitHeight + icon.height
    radius: 3
    color: "transparent"

    ColumnLayout {
        anchors.fill: parent
        spacing: -5
        Image {
            id: icon
            fillMode: Image.PreserveAspectFit
            Layout.alignment: Qt.AlignHCenter
            clip: true
        }
        Text {
            id: text
            text: qsTr("text")
            Layout.alignment: Qt.AlignHCenter
            leftPadding: 5
            rightPadding: 5
            bottomPadding: 2
            topPadding: 1
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true

        //接受左键和右键输入
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            parent.clicked()
        }

        onPressed: {
            color = clr_click
        }

        onReleased: {
            color = clr_enter
        }

        onEntered: {
            color = clr_enter
            cursorShape = Qt.PointingHandCursor
        }

        onExited: {
            color = clr_exit
            cursorShape = Qt.ArrowCursor
        }
    }
}
