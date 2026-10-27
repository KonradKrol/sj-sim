import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Column {
    id: wind
    property var node
    readonly property int selectedSector: fields.selectedSector || 0
    readonly property var fields: node ? node.values : ({})
    readonly property var sectors: node ? node.children : []
    readonly property int selected: Math.max(0,Math.min(selectedSector,sectors.length-1))
    spacing: 16
    RowLayout {
        width: parent.width
        visible: ui.profile==="CompetitionConfigWindow" && ui.context.suggestedGate>=0
        readonly property int difference: ui.context.suggestedGate-ui.context.startingGate
        Label { text: qsTr("Sugerowana belka: %1").arg(ui.context.suggestedGate)+(parent.difference ? " ("+(parent.difference>0?"+":"")+parent.difference+")" : ""); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        SButton { objectName: "applySuggestedGate"; text: qsTr("Zastosuj"); quiet: true; font.pointSize: Theme.caption; implicitHeight: 32; enabled: ui.context.startingGate!==ui.context.suggestedGate; onClicked: {var gate=ui.find("spinBox_startGate");if(gate)ui.edit(gate.values.id,ui.context.suggestedGate)} }
    }
    Flow {
        enabled: wind.sectors.length > 0
        width: parent.width; spacing: 8
        SButton { text: qsTr("Losuj"); primary: true; enabled: (automaticDirection.checked || direction.acceptableInput) && (automaticVariation.checked || variation.acceptableInput); onClicked: ui.randomizeWind(wind.fields.id,automaticDirection.checked ? -1 : Number.fromLocaleString(Qt.locale(Theme.localeName),direction.text),automaticVariation.checked ? -1 : Number.fromLocaleString(Qt.locale(Theme.localeName),variation.text)) }
        SButton { text: qsTr("Przedni"); onClicked: ui.setWind(wind.fields.id,0,{strength:2,direction:0,strengthVariation:0,directionVariation:0},true) }
        SButton { text: qsTr("Tylny"); onClicked: ui.setWind(wind.fields.id,0,{strength:2,direction:180,strengthVariation:0,directionVariation:0},true) }
        SButton { text: qsTr("Cisza"); onClicked: ui.setWind(wind.fields.id,0,{strength:0,direction:0,strengthVariation:0,directionVariation:0},true) }
        SButton { id: options; text: qsTr("Opcje"); quiet: true; checkable: true }
        SButton { text: qsTr("Cofnij"); quiet: true; enabled: !!wind.fields.canUndo; onClicked: ui.undoWind(wind.fields.id) }
    }
    GridLayout {
        visible: options.checked
        width: parent.width
        columns: width >= 600 ? 4 : 2
        columnSpacing: 12; rowSpacing: 8
        SCheck { id: automaticDirection; text: qsTr("Losowy kierunek"); checked: true; Layout.fillWidth: true }
        SField { id: direction; text: "0"; enabled: !automaticDirection.checked; validator: DoubleValidator { bottom: 0; top: 360; decimals: 1; locale: Qt.locale(Theme.localeName).name }
            Accessible.name: qsTr("Kierunek · °"); Layout.fillWidth: true; Layout.preferredWidth: 100 }
        SCheck { id: automaticVariation; text: qsTr("Losowa zmienność"); checked: true; Layout.fillWidth: true }
        SField { id: variation; text: "5"; enabled: !automaticVariation.checked; validator: DoubleValidator { bottom: 0; top: 10; decimals: 1; locale: Qt.locale(Theme.localeName).name }
            Accessible.name: qsTr("Zmienność · 0–10"); Layout.fillWidth: true; Layout.preferredWidth: 100 }
    }
    WindSectorStrip { width: parent.width; sectors: wind.sectors; currentIndex: wind.selected; onSelected: ui.selectWindSector(wind.fields.id,index) }
    Pane {
        width: parent.width
        padding: 16
        background: Surface {}
        contentItem: WindFields { sector: wind.sectors.length ? wind.sectors[wind.selected] : null; editorId: wind.fields.id || -1; sectorIndex: wind.selected }
    }
    Loader { width: parent.width; active: !!wind.fields.submit; source: Qt.resolvedUrl("Node.qml"); onLoaded: item.node=Qt.binding(function(){return wind.fields.submit}) }
}
