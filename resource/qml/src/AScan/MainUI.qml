import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtCharts 2.15
import Union.Interactor 1.0
import "../../components"
import Union.AScan 1.0
import Union.View 1.0

Rectangle {
    LoggingCategory {
        id: category
        name: "AScan.main"
    }

    clip: true
    color: "transparent"
    property alias controlTarget: cons.target
    property alias mainTarget: main_cons.target

    readonly property var gateTextTable: [{
            "amp": "A%",
            "→": "A→",
            "↓": "A↓",
            "↘": "A↘",
            "equi": qsTr("当量")
        }, {
            "amp": "B%",
            "→": "B→",
            "↓": "B↓",
            "↘": "B↘",
            "equi": qsTr("当量")
        }]

    readonly property var gateIndexTable: ["amp", "↘", "→", "↓", "equi"]

    property var gateEnable: [false, false]

    readonly property var gateTextColor: ["red", "#5b4bab"]

    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 20
            Layout.rightMargin: 20
            spacing: 15

            ColumnLayout {
                Layout.alignment: Qt.AlignVCenter
                Layout.leftMargin: 20
                Text {
                    text: qsTr("声程") + `(${interactor.distanceMode})`
                    font.pixelSize: 20
                }
            }

            GridLayout {
                id: grid_layout
                Layout.fillWidth: true
                columns: 10
                columnSpacing: -1
                rowSpacing: -1
                Component {
                    id: text_component
                    Rectangle {
                        property int minPreferredWidth: 60
                        property bool fillWidth: false
                        property alias backgroundColor: root_box.color
                        property alias text: text.text
                        property color textColor: "black"

                        id: root_box
                        Layout.alignment: Qt.AlignCenter
                        height: text.height
                        border.color: "#7c95c4"
                        Layout.fillWidth: fillWidth
                        Layout.preferredWidth: Math.max(minPreferredWidth, text.contentWidth + 10)
                        Text {
                            id: text

                            anchors.centerIn: parent

                            padding: 5
                            font.pointSize: 12
                            horizontalAlignment: Qt.AlignHCenter
                            verticalAlignment: Qt.AlignVCenter
                            color: textColor
                        }
                    }
                }

                Component.onCompleted: {
                    grid_layout.children = []
                    for (var i = 0; i < 10; i++) {
                        let gateIndex = parseInt(i / gateIndexTable.length)
                        let paramIndex = i % gateIndexTable.length
                        grid_layout.children.push(text_component.createObject(grid_layout, {
                                                                                  "backgroundColor": "#afeeee",
                                                                                  "text": Qt.binding(() => {
                                                                                                         return gateTextTable[gateIndex][gateIndexTable[paramIndex]]
                                                                                                     }),
                                                                                  "textColor": gateTextColor[gateIndex],
                                                                                  "visible": Qt.binding(() => {
                                                                                                            return gateEnable[gateIndex]
                                                                                                        })
                                                                              }))
                        let object_param = {
                            "backgroundColor": "white",
                            "minPreferredWidth": 96,
                            "text": Qt.binding(() => {
                                                   return interactor.gateValue[gateIndex][gateIndexTable[paramIndex]]
                                               }),
                            "textColor": gateTextColor[gateIndex],
                            "visible": Qt.binding(() => {
                                                      return gateEnable[gateIndex]
                                                  })
                        }
                        if (paramIndex === 4) {
                            object_param["minPreferredWidth"] = 200
                        }

                        grid_layout.children.push(text_component.createObject(grid_layout, object_param))
                    }
                }
            }

            CKeyValue {
                Layout.alignment: Qt.AlignVCenter
                visible: gateEnable[1]
                property real value_A: parseFloat(interactor.gateValue[0]["↓"])
                property real value_B: parseFloat(interactor.gateValue[1]["↓"])
                key: qsTr("B-A(↓)")
                value: Math.round((value_B - value_A) * 10.0) / 10.0
            }
        }

        RowLayout {

            ChartView {
                id: chart_view
                readonly property bool hiddenOnResizing: false
                Layout.fillWidth: true
                Layout.fillHeight: true
                antialiasing: false
                legend.visible: false
                dropShadowEnabled: true

                Text {
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    anchors.leftMargin: 15
                    anchors.bottomMargin: 10
                    font.pointSize: 20

                    text: "A"
                }

                GateDrawMask {
                    x: chart_view.plotArea.x
                    y: chart_view.plotArea.y
                    width: chart_view.plotArea.width
                    height: chart_view.plotArea.height
                    interactor: interactor
                }
            }

            Control {
                id: scan_view_box_extra

                Layout.fillWidth: true
                Layout.fillHeight: true

                visible: interactor.scanViewHandlerExtra

                GridLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    columns: 2
                    rows: 2
                    columnSpacing: 0
                    rowSpacing: 0

                    Item {
                        Layout.row: 0
                        Layout.column: 0
                        Layout.preferredWidth: 35
                        Layout.fillHeight: true
                        ScrollView {
                            id: axis_vertical_box_extra
                            anchors.fill: parent

                            clip: true

                            contentWidth: axis_vertical_extra.width
                            contentHeight: axis_vertical_extra.height

                            ScrollBar.vertical.visible: false
                            ScrollBar.vertical.interactive: false
                            ScrollBar.vertical.position: b_or_c_scan_view_box_extra.ScrollBar.vertical.position

                            Flickable {
                                boundsBehavior: Flickable.StopAtBounds
                            }

                            Binding {
                                target: b_or_c_scan_view_box_extra.ScrollBar.vertical
                                property: "position"
                                value: axis_vertical_box_extra.ScrollBar.vertical.position
                            }

                            AxisView {
                                id: axis_vertical_extra
                                isVertical: true
                                reverse: true
                                width: 35
                            }
                        }
                    }

                    Item {
                        Layout.row: 0
                        Layout.column: 1
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        ScrollView {
                            id: b_or_c_scan_view_box_extra
                            anchors.fill: parent
                            clip: true

                            ScrollBar.horizontal.visible: false
                            ScrollBar.horizontal.interactive: false
                            ScrollBar.vertical.visible: false
                            ScrollBar.vertical.interactive: false

                            Flickable {
                                boundsBehavior: Flickable.StopAtBounds
                            }
                        }
                    }

                    Rectangle {
                        Layout.row: 1
                        Layout.column: 0
                        Layout.preferredWidth: 35
                        Layout.preferredHeight: 35
                        color: "#afeeee"
                        Text {
                            font.pointSize: 20
                            text: "B"
                            anchors.centerIn: parent
                        }
                    }

                    Item {
                        Layout.preferredHeight: 35
                        Layout.fillWidth: true
                        Layout.row: 1
                        Layout.column: 1

                        ScrollView {
                            id: axis_horizontal_box_extra
                            anchors.fill: parent
                            contentWidth: axis_horizontal_extra.width
                            contentHeight: axis_horizontal_extra.height
                            clip: true

                            ScrollBar.horizontal.visible: false
                            ScrollBar.horizontal.interactive: false
                            ScrollBar.horizontal.position: b_or_c_scan_view_box_extra.ScrollBar.horizontal.position

                            Flickable {
                                boundsBehavior: Flickable.StopAtBounds
                            }

                            Binding {
                                target: b_or_c_scan_view_box_extra.ScrollBar.horizontal
                                property: "position"
                                value: axis_horizontal_box_extra.ScrollBar.horizontal.position
                            }

                            AxisView {
                                id: axis_horizontal_extra
                                height: 35
                            }
                        }
                    }
                }

                Connections {
                    target: interactor
                    enabled: interactor

                    function onUpdateBScanExtraHandler() {
                        if (interactor.scanViewHandlerExtra) {
                            b_or_c_scan_view_box_extra.contentChildren.push(interactor.scanViewHandlerExtra)
                            b_or_c_scan_view_box_extra.onChildrenChanged()

                            let img_width = interactor.scanViewHandlerExtra.width
                            let img_height = interactor.scanViewHandlerExtra.height

                            interactor.scanViewHandlerExtra.width = Qt.binding(() => {
                                                                                   let content_width = b_or_c_scan_view_box_extra.width
                                                                                   b_or_c_scan_view_box_extra.contentWidth = content_width
                                                                                   return content_width
                                                                               })

                            interactor.scanViewHandlerExtra.height = Qt.binding(() => {
                                                                                    let content_height = b_or_c_scan_view_box_extra.height
                                                                                    b_or_c_scan_view_box_extra.contentHeight
                                                                                    = content_height
                                                                                    return content_height
                                                                                })

                            axis_horizontal_extra.width = Qt.binding(() => {
                                                                         try {
                                                                             return interactor.scanViewHandlerExtra.width
                                                                         } catch (e) {
                                                                             return axis_horizontal_extra.width
                                                                         }
                                                                     })

                            axis_vertical_extra.height = Qt.binding(() => {
                                                                        try {
                                                                            return interactor.scanViewHandlerExtra.height
                                                                        } catch (e) {
                                                                            return axis_vertical_extra.height
                                                                        }
                                                                    })

                            axis_horizontal_extra.axisRange = Qt.binding(() => {
                                                                             try {
                                                                                 return interactor.scanViewHandlerExtra.horizontalAxisRange
                                                                             } catch (e) {
                                                                                 return axis_horizontal_extra.axisRange
                                                                             }
                                                                         })

                            axis_vertical_extra.axisRange = Qt.binding(() => {
                                                                           try {
                                                                               return interactor.scanViewHandlerExtra.verticalAxisRange
                                                                           } catch (e) {
                                                                               return axis_vertical_extra.axisRange
                                                                           }
                                                                       })
                        } else {
                            b_or_c_scan_view_box_extra.contentChildren = []
                        }
                    }
                }
            }
        }

        Control {
            id: scan_view_box

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.topMargin: -10

            visible: interactor ? interactor.scanViewHandler : false

            GridLayout {
                anchors.fill: parent
                anchors.margins: 10
                columns: 2
                rows: 2
                columnSpacing: 0
                rowSpacing: 0

                Item {
                    Layout.row: 0
                    Layout.column: 0
                    Layout.preferredWidth: 35
                    Layout.fillHeight: true
                    ScrollView {
                        id: axis_vertical_box
                        anchors.fill: parent

                        clip: true

                        contentWidth: axis_vertical.width
                        contentHeight: axis_vertical.height

                        ScrollBar.vertical.visible: false
                        ScrollBar.vertical.interactive: false
                        ScrollBar.vertical.position: b_or_c_scan_view_box.ScrollBar.vertical.position

                        Flickable {
                            boundsBehavior: Flickable.StopAtBounds
                        }

                        Binding {
                            target: b_or_c_scan_view_box.ScrollBar.vertical
                            property: "position"
                            value: axis_vertical_box.ScrollBar.vertical.position
                        }

                        AxisView {
                            id: axis_vertical
                            isVertical: true
                            reverse: true
                            width: 35
                        }
                    }
                }

                Item {
                    Layout.row: 0
                    Layout.column: 1
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    ScrollView {
                        id: b_or_c_scan_view_box
                        anchors.fill: parent
                        clip: true

                        ScrollBar.horizontal.visible: false
                        ScrollBar.horizontal.interactive: false
                        ScrollBar.vertical.visible: false
                        ScrollBar.vertical.interactive: false

                        Flickable {
                            boundsBehavior: Flickable.StopAtBounds
                        }
                    }
                }

                Rectangle {
                    Layout.row: 1
                    Layout.column: 0
                    Layout.preferredWidth: 35
                    Layout.preferredHeight: 35
                    color: "#afeeee"

                    Text {
                        font.pointSize: 20
                        function getText() {
                            if (interactor && interactor.showBScanView) {
                                return "B"
                            }
                            if (interactor && interactor.showCScanView) {
                                return "C"
                            }
                            return ""
                        }

                        text: getText()
                        anchors.centerIn: parent
                    }
                }

                Item {
                    Layout.preferredHeight: 35
                    Layout.fillWidth: true
                    Layout.row: 1
                    Layout.column: 1

                    ScrollView {
                        id: axis_horizontal_box
                        anchors.fill: parent
                        contentWidth: axis_horizontal.width
                        contentHeight: axis_horizontal.height
                        clip: true

                        ScrollBar.horizontal.visible: false
                        ScrollBar.horizontal.interactive: false
                        ScrollBar.horizontal.position: b_or_c_scan_view_box.ScrollBar.horizontal.position

                        Flickable {
                            boundsBehavior: Flickable.StopAtBounds
                        }

                        Binding {
                            target: b_or_c_scan_view_box.ScrollBar.horizontal
                            property: "position"
                            value: axis_horizontal_box.ScrollBar.horizontal.position
                        }

                        AxisView {
                            id: axis_horizontal
                            height: 35
                        }
                    }
                }
            }

            Connections {
                target: interactor.scanViewHandler
                enabled: interactor.scanViewHandler
                ignoreUnknownSignals: true

                function onDataCursorChanged() {
                    if (interactor.showBScanView) {
                        interactor.aScanCursor = interactor.scanViewHandler.dataCursor
                    }
                }

                function onDataCursorIntChanged() {
                    if (interactor.showCScanView) {
                        interactor.aScanCursor = interactor.scanViewHandler.dataCursorInt
                    }
                }
            }

            Connections {
                target: interactor
                enabled: interactor

                function onUpdateBOrCScanHandler() {
                    if (interactor.scanViewHandler) {
                        b_or_c_scan_view_box.contentChildren.push(interactor.scanViewHandler)
                        b_or_c_scan_view_box.onChildrenChanged()

                        interactor.scanViewHandler.width = Qt.binding(() => {
                                                                          let content_width = b_or_c_scan_view_box.width
                                                                          b_or_c_scan_view_box.contentWidth = content_width
                                                                          return content_width
                                                                      })

                        interactor.scanViewHandler.height = Qt.binding(() => {
                                                                           let content_height = b_or_c_scan_view_box.height
                                                                           b_or_c_scan_view_box.contentHeight = content_height
                                                                           return content_height
                                                                       })

                        axis_horizontal.width = Qt.binding(() => {
                                                               try {
                                                                   return interactor.scanViewHandler.width
                                                               } catch (e) {
                                                                   return axis_horizontal.width
                                                               }
                                                           })

                        axis_vertical.height = Qt.binding(() => {
                                                              try {
                                                                  return interactor.scanViewHandler.height
                                                              } catch (e) {
                                                                  return axis_vertical.height
                                                              }
                                                          })

                        axis_horizontal.axisRange = Qt.binding(() => {
                                                                   try {
                                                                       return interactor.scanViewHandler.horizontalAxisRange
                                                                   } catch (e) {
                                                                       return axis_horizontal.axisRange
                                                                   }
                                                               })

                        axis_vertical.axisRange = Qt.binding(() => {
                                                                 try {
                                                                     return interactor.scanViewHandler.verticalAxisRange
                                                                 } catch (e) {
                                                                     return axis_vertical.axisRange
                                                                 }
                                                             })
                    } else {
                        b_or_c_scan_view_box.contentChildren = []
                    }
                }
            }
        }
    }

    AScanInteractor {
        id: interactor
        softGain: controlTarget !== null ? controlTarget.softGain : 0
        replayValue: controlTarget !== null ? controlTarget.replayValue : 0
        chartView: chart_view
        onAScanCursorChanged: {
            if (interactor.hasCameraImage) {
                controlTarget.setImage(interactor.getCameraImage())
                if (pop_img.opened) {
                    img_popup.setImageAndZoomIn(interactor.getCameraImage(), 2)
                }
            }
            let _gateEnable = [interactor.isGateEnable(0), interactor.isGateEnable(1)]
            gateEnable = _gateEnable
        }

        onReplayValueChanged: {
            controlTarget.replayValue = replayValue
        }
    }

    Connections {
        id: cons
        ignoreUnknownSignals: true
        target: null

        function onReportExportClicked(fileName) {
            chart_view.grabToImage(function (result) {
                console.log(category, result)
                if (interactor.reportExportClicked(fileName, result)) {
                    showSuccessful(qsTr("保存成功"))
                } else {
                    showFailed(qsTr("保存失败"))
                }
            })
        }

        function onPerformanceClicked(fileName) {
            if (interactor.performanceClicked(fileName)) {
                showSuccessful(qsTr("保存成功"))
            } else {
                showFailed(qsTr("保存失败"))
            }
        }

        function onGainValueModified(val) {
            interactor.gainValueModified(val)
        }

        function onReplayStartClicked(isStart) {
            interactor.replayStartClicked(isStart)
        }

        function onReplaySpeedClicked(val) {
            interactor.replaySpeedClicked(val)
        }

        function onLastFrameClicked() {
            interactor.lastFrameClicked()
        }

        function onNextFrameClicked() {
            interactor.nextFrameClicked()
        }

        function onTimeSliderMoved(val) {
            interactor.timeSliderMoved(val)
        }

        function onSetFileNameIndex(idx) {
            interactor.setFileNameIndex(idx)
        }

        function onShowImage() {
            console.log("on click image")
            img_popup.setImageAndZoomIn(interactor.getCameraImage(), 2)
            pop_img.open()
        }
    }

    Connections {
        property var wind
        id: cursor_changed_connection
        target: interactor
        ignoreUnknownSignals: true
        enabled: false
        function onAScanCursorChanged() {
            if (!wind) {
                return
            }

            wind.tableData = interactor.getTechnologicalParameter()
            wind.generateTable()
        }
    }

    Connections {
        id: main_cons
        ignoreUnknownSignals: true
        function onBtnParamClicked() {
            // 判断是否已经打开了一个工艺参数窗口
            if (cursor_changed_connection.wind) {
                return
            }

            var comp = Qt.createComponent("qrc:/qml/components/common/JsonTable.qml")
            if (comp.status === Component.Ready) {
                // DONE: 获取参数
                const params = interactor.getTechnologicalParameter()
                console.log(category, params)
                var params_wnd = comp.createObject(parent, {
                                                       "tableData": params,
                                                       "treeDepthBias": 1,
                                                       "title": qsTr("工艺参数")
                                                   })

                cursor_changed_connection.wind = params_wnd
                cursor_changed_connection.enabled = true

                params_wnd.closing.connect(() => {
                                               cursor_changed_connection.wind = null
                                               cursor_changed_connection.enabled = false
                                               comp.destroy()
                                               params_wnd.destroy()
                                           })
            } else if (comp.status === Component.Error) {
                console.error(category, comp.errorString())
            }
        }

        function onListviewIndexChanged(index, list_view) {
            const filePath = list_view.get(index, "filePath")
            console.log(category, "listview index changed, index:", index, "filepath:", filePath)

            openFileAction(filePath)

            if (cursor_changed_connection.wind) {
                cursor_changed_connection.wind.tableData = interactor.getTechnologicalParameter()
                cursor_changed_connection.wind.generateTable()
            }
        }

        function onOpenFile(filePath) {
            openFileAction(filePath)
        }

        function onSplitViewResizingChanged(resizing) {
            console.log(category, "onSplitViewResizingChanged:", resizing)
            if (chart_view.hiddenOnResizing) {
                if (resizing) {
                    chart_view.visible = false
                } else {
                    chart_view.visible = true
                }
            }
        }
    }

    function openFileAction(filePath) {
        controlTarget.replayVisible = Qt.binding(() => {
                                                     return interactor.replayVisible
                                                 })
        controlTarget.date = Qt.binding(() => {
                                            return interactor.date
                                        })
        controlTarget.cursorMax = Qt.binding(() => {
                                                 return interactor.aScanCursorMax
                                             })
        controlTarget.imageVisible = Qt.binding(() => {
                                                    return interactor.hasCameraImage
                                                })
        controlTarget.showRailWeldDigram = Qt.binding(() => {
                                                          return interactor.showRailWeldDigramSpecial
                                                      })
        controlTarget.showCMP001Special = Qt.binding(() => {
                                                         return interactor.showCMP001Special
                                                     })
        controlTarget.replayTimerBaseInterval = Qt.binding(() => {
                                                               return interactor.replayTimerInterval
                                                           })
        controlTarget.reportEnable = Qt.binding(() => {
                                                    return interactor.reportEnabled
                                                })
        controlTarget.dateEnable = Qt.binding(() => {
                                                  return interactor.dateEnabled
                                              })
        controlTarget.railweldSpecial_ZeroPointInFoot = Qt.binding(() => {
                                                                       return interactor.railWeldSpecial_ZeroPointInFoot()
                                                                   })
        controlTarget.softGainEnable = Qt.binding(() => {
                                                      return interactor.softGainEnable
                                                  })
        controlTarget.aScanInteractor = interactor
        interactor.setDefaultValue()
        if (interactor.openFile(filePath)) {
            controlTarget.fileNameList = interactor.getFileNameList()
            if (interactor.hasCameraImage) {
                controlTarget.setImage(interactor.getCameraImage())
            }
            if (interactor.showRailWeldDigramSpecial) {
                controlTarget.setRailWeldDot(interactor.getRailWeldDot())
            }
            let _gateEnable = [interactor.isGateEnable(0), interactor.isGateEnable(1)]
            gateEnable = _gateEnable
            showSuccessful(qsTr("打开成功"))

            controlTarget.replayVisible = Qt.binding(() => {
                                                         return interactor.replayVisible
                                                     })
            controlTarget.date = Qt.binding(() => {
                                                return interactor.date
                                            })
            controlTarget.cursorMax = Qt.binding(() => {
                                                     return interactor.aScanCursorMax
                                                 })
            controlTarget.imageVisible = Qt.binding(() => {
                                                        return interactor.hasCameraImage
                                                    })
            controlTarget.showRailWeldDigram = Qt.binding(() => {
                                                              return interactor.showRailWeldDigramSpecial
                                                          })
            controlTarget.showCMP001Special = Qt.binding(() => {
                                                             return interactor.showCMP001Special
                                                         })
            controlTarget.replayTimerBaseInterval = Qt.binding(() => {
                                                                   return interactor.replayTimerInterval
                                                               })
            controlTarget.reportEnable = Qt.binding(() => {
                                                        return interactor.reportEnabled
                                                    })
            controlTarget.dateEnable = Qt.binding(() => {
                                                      return interactor.dateEnabled
                                                  })
            controlTarget.railweldSpecial_ZeroPointInFoot = Qt.binding(() => {
                                                                           return interactor.railWeldSpecial_ZeroPointInFoot()
                                                                       })
        } else {
            showFailed(qsTr("打开失败"))
        }
        controlTarget.init()
    }

    Popup {
        id: pop_img
        anchors.centerIn: parent
        width: 0
        height: 0
        modal: true
        background: Rectangle {
            color: "transparent"
        }

        CImage {
            id: img_popup
            anchors.centerIn: parent
        }
    }

    Component.onDestruction: {
        if (cursor_changed_connection.wind) {
            cursor_changed_connection.wind.close()
        }
    }
}
