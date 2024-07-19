import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "./components"
import Morose.translator 1.0

ApplicationWindow {
    minimumWidth: 640
    maximumWidth: 640
    minimumHeight: 480
    maximumHeight: 480

    LoggingCategory {
        id: tag
        name: "About"
    }

    title: qsTr("关于")
    visible: true
    flags: Qt.Dialog
    modality: Qt.WindowModal
    background: Rectangle {
        color: "#e0eeee"
    }
    RowLayout {
        x: 50
        y: 150
        height: 120
        spacing: 10
        Image {
            Layout.preferredWidth: 120
            source: "qrc:/img/morose.ico"
            fillMode: Image.PreserveAspectFit
        }
        ColumnLayout {
            Layout.fillHeight: true
            spacing: 40
            Text {
                Layout.fillWidth: true
                verticalAlignment: Qt.AlignVCenter
                font.pixelSize: 18
                text: `${MOROSE_APP_NAME_ZH_CN} ${MOROSE_APP_VERSION}`
            }
            Text {
                Layout.fillWidth: true
                verticalAlignment: Qt.AlignVCenter
                font.pixelSize: 18
                text: `版权所有(C) 南通友联数码技术开发有限公司`
            }
        }
    }
}
