import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
RowLayout {
    id: controls
    property var backend
    property bool wide: false
    readonly property var history: backend ? backend.find("resultsHistory") : null
    readonly property var fields: history ? history.values : ({})
    readonly property var round: fields.roundField ? fields.roundField.values : ({})
    readonly property bool hasChoices: !!fields.modeField || (round.options || []).length>1
    spacing: 6
    Loader { active: controls.wide && controls.hasChoices; visible: active; sourceComponent: Component { RaceViewChoices { backend: controls.backend } } }
    SButton {
        objectName: "raceViewMenu"
        visible: !controls.wide && controls.hasChoices
        text: qsTr("Wyniki ▾")
        Accessible.description: (controls.round.options || [])[controls.round.value] || ""
        quiet: true; font.pointSize: Theme.caption; padding: 6
        implicitHeight: Theme.controlHeight
        onClicked: {var at=mapToGlobal(0,height);controls.backend.openResultViewOptions(-1,at.x,at.y,width)}
    }
    SButton {
        objectName: "returnToCompetition"
        visible: !!controls.fields.paused
        text: qsTr("Wróć do konkursu"); primary: true
        font.pointSize: Theme.caption; padding: 6; implicitHeight: Theme.controlHeight
        onClicked: {if(controls.fields.individual && controls.fields.modeField)controls.backend.edit(controls.fields.modeField.values.id,0);controls.backend.edit(controls.round.id,0)}
    }
}
