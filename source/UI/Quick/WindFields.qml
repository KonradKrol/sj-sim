import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Column {
    id: editor
    property var sector
    property int editorId: -1
    property int sectorIndex: 0
    readonly property var fields: sector ? sector.values : ({})
    spacing: 12
    RowLayout {
        width: parent.width
        Label { text: qsTr("Sektor %1").arg(editor.sectorIndex+1); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; Layout.fillWidth: true }
        SCheck { id: together; text: qsTr("Wszystkie"); Accessible.description: qsTr("Zmiany dotyczą wszystkich sektorów") }
    }
    GridLayout {
        width: parent.width
        columns: width >= 800 ? 4 : width >= 420 ? 2 : 1
        columnSpacing: 16; rowSpacing: 12
        Repeater {
            model: [{key:"strength",label:qsTr("Siła · m/s")},{key:"direction",label:qsTr("Kierunek · °")},{key:"strengthVariation",label:qsTr("Zmienność · m/s")},{key:"directionVariation",label:qsTr("Zmienność · °")}]
            Column {
                id: field
                required property var modelData
                readonly property string key: field.modelData.key
                property var values: editor.fields[key]
                visible: !!values
                Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: 200
                spacing: 6
                Label { text: field.modelData.label; color: Theme.muted; font.pointSize: Theme.caption }
                NumberField {
                    width: parent.width
                    fields: field.values || ({value:0,minimum:0,maximum:0,step:1,decimals:1})
                    label: field.modelData.label
                    direct: false
                    onCommitted: { var data={};data[field.key]=value;ui.setWind(editor.editorId,editor.sectorIndex,data,together.checked) }
                }
            }
        }
    }
    Flow {
        width: parent.width; spacing: 8
        SButton {
            text: qsTr("Kopiuj do wszystkich")
            quiet: true
            onClicked: ui.copyWindSector(editor.editorId,editor.sectorIndex)
        }
        SButton { id: advanced; visible: !!editor.fields.details; text: qsTr("Cechy"); quiet: true; checkable: true }
    }
    Item {
        id: detailsWrap
        width: parent.width
        property real reveal: advanced.checked && !!editor.fields.details ? 1 : 0
        height: details.item ? details.item.implicitHeight*reveal : 0
        clip: true
        Behavior on reveal { NumberAnimation { duration: Theme.motion; easing.type: Easing.OutCubic } }
        Loader {
            id: details
            width: parent.width
            active: detailsWrap.reveal>0
            opacity: detailsWrap.reveal
            source: Qt.resolvedUrl("Node.qml")
            onLoaded: item.node=Qt.binding(function(){return editor.fields.details})
        }
    }
}
