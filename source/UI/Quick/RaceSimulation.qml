import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: simulation
    signal started()
    readonly property var source: ui.find("toolButton_autoSimulationActions")
    readonly property var actions: source ? source.values.actions || [] : []
    enabled: !!source && source.values.enabled
    spacing: 6
    Label { text: qsTr("Symulacja"); color: Theme.muted; font.pointSize: Theme.caption }
    RowLayout {
        Layout.fillWidth: true; spacing: 6
        Repeater {
            model: [qsTr("N skoków"),qsTr("Seria"),qsTr("Konkurs")]
            SButton {
                objectName: "raceSimulate-"+index
                readonly property int actionIndex: index===0 ? simulation.actions.length-1 : index===1 ? simulation.actions.length-2 : 0
                Layout.fillWidth: true; Layout.minimumWidth: 0
                text: modelData
                enabled: actionIndex>=0 && !!simulation.actions[actionIndex] && simulation.actions[actionIndex].enabled
                hint: index===0 ? qsTr("Wybierz liczbę kolejnych skoków") : index===1 ? qsTr("Pozostałe skoki aktualnej serii") : qsTr("Cały pozostały konkurs")
                onClicked: { ui.activate(simulation.source.values.id,actionIndex); simulation.started() }
            }
        }
    }
    SButton {
        objectName: "raceSimulateGroup"
        visible: simulation.actions.length===4
        text: qsTr("Grupa")
        hint: qsTr("Pozostałe skoki aktualnej grupy")
        onClicked: { ui.activate(simulation.source.values.id,1); simulation.started() }
    }
}
