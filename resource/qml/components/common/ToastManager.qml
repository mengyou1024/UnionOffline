import QtQuick 2.15

Column {
    property var toastComponent: toastComponent = Qt.createComponent("Toast.qml")

    function show(text, duration) {
        var toast = toastComponent.createObject(toastManager)
        toast.showImage = false
        toast.show(text, duration)
    }

    function showSuccessful(text) {
        var toast = toastComponent.createObject(toastManager)
        toast.show(text, 1500)
    }

    function showFailed(text) {
        var toast = toastComponent.createObject(toastManager)
        toast.msgType = "failed"
        toast.show(text, 1500)
    }

    id: toastManager

    z: Infinity
    spacing: 5
    anchors.centerIn: parent
}
