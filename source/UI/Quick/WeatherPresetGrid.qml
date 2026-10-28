import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

GridLayout {
    id: presets
    property var backend
    readonly property var values: backend.values
    readonly property var groups: [
        {name:qsTr("Pod narty"),symbol:"↖",items:[{id:5,name:qsTr("Spokojny"),power:1},{id:1,name:qsTr("Mocny"),power:3},{id:8,name:qsTr("Zmienny"),power:2}]},
        {name:qsTr("W plecy"),symbol:"↘",items:[{id:0,name:qsTr("Stabilny"),power:1},{id:6,name:qsTr("Mocny"),power:3},{id:9,name:qsTr("Zmienny"),power:2}]},
        {name:qsTr("Boczny"),symbol:"↗",items:[{id:7,name:qsTr("Spokojny"),power:1},{id:3,name:qsTr("Zmienny"),power:2}]},
        {name:qsTr("Pozostałe"),symbol:"↔",items:[{id:4,name:qsTr("Cisza"),power:0},{id:10,name:qsTr("Różny na zeskoku"),power:2},{id:2,name:qsTr("Loteria"),power:3}]}
    ]
    columns: width>=Theme.fontSize(780)?4:width>=Theme.fontSize(450)?2:1
    columnSpacing: Theme.dp(8); rowSpacing: Theme.dp(8)
    Repeater {
        model: presets.groups
        Control {
            id: group
            required property var modelData
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.preferredWidth: 0
            padding: Theme.dp(7)
            implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
            background: Rectangle { color: Theme.raised; radius: Theme.dp(5) }
            contentItem: ColumnLayout {
                spacing: Theme.dp(5)
                RowLayout {
                    Layout.fillWidth: true; spacing: Theme.dp(5)
                    Label { text: group.modelData.name; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.bold: true; color: Theme.ink }
                    Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: Theme.outline; opacity: .4 }
                }
                Repeater {
                    model: group.modelData.items
                    WeatherPresetChoice {
                        required property var modelData
                        objectName: "weatherPreset-"+modelData.id
                        Layout.fillWidth: true; Layout.minimumWidth: 0
                        title: modelData.name; symbol: modelData.id===4?"—":group.modelData.symbol
                        power: modelData.power
                        accessibleTitle: presets.backend.presets[modelData.id]
                        explanation: accessibleTitle
                        checked: presets.values.preset===modelData.id && !presets.values.modified
                        onClicked: {presets.forceActiveFocus();presets.backend.applyPreset(modelData.id)}
                    }
                }
                Item { Layout.fillHeight: true; implicitHeight: 0 }
            }
        }
    }
}
