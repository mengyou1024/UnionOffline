import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

Rectangle {
    id: root
    property int maskRadius: 3
    property string key: ""
    property alias value: _value.text
    width: _key.width + _value.width
    height: Math.max(_key.height, _value.height)
    clip: true
    border.width: 1
    border.color: "black"
    Row {
        clip: true
        spacing: 0
        Label {
            id: _key
            padding: 5
            leftPadding: 10
            rightPadding: 10
            text: key + ":"
            font.pointSize: 12
            background: Rectangle {
                color: Qt.darker("#00e3e6", 1.2)
            }
        }
        Label {
            id: _value
            padding: 5
            leftPadding: 20
            rightPadding: 20
            font.pointSize: 12
            background: Rectangle {
                color: Qt.lighter("#00e3e6", 1.0)
            }
        }
    }
    layer.enabled: true
    layer.effect: OpacityMask {
        maskSource: Rectangle {
            id: mask
            width: root.width
            height: root.height
            radius: maskRadius
        }
    }
}
