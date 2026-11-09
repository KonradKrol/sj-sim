import QtQuick 2.15

SButton {
    id: button
    objectName: "raceContestOptions"
    text: qsTr("Opcje konkursu")+" ▾"
    dense: true
    readonly property var simulation: ui.find("toolButton_autoSimulationActions")
    readonly property var cancellation: ui.find("toolButton_cancelActions")
    readonly property var simulationActions: simulation ? simulation.values.actions || [] : []
    function entry(text,source,index,danger) {
        var actions=source ? source.values.actions || [] : []
        return {text:text,sourceId:source ? source.values.id : -1,actionIndex:index,danger:!!danger,enabled:!!source && source.values.enabled!==false && !!actions[index] && actions[index].enabled!==false}
    }
    onClicked: menu.open()
    ActionMenu {
        id: menu
        entries: {
            var rows=[button.entry(qsTr("Symuluj konkurs"),button.simulation,0)]
            if(button.simulationActions.length===4)rows.push(button.entry(qsTr("Symuluj grupę"),button.simulation,1))
            if(Number(ui.context.roundCount)>1)rows.push(button.entry(qsTr("Symuluj serię"),button.simulation,button.simulationActions.length-2))
            rows.push(button.entry(qsTr("Symuluj N skoków"),button.simulation,button.simulationActions.length-1))
            rows.push(button.entry(qsTr("Odwołaj rundę"),button.cancellation,1,true))
            rows.push(button.entry(qsTr("Odwołaj konkurs"),button.cancellation,0,true))
            return rows
        }
        onSelected: function(entry){ui.activate(entry.sourceId,entry.actionIndex)}
    }
}
