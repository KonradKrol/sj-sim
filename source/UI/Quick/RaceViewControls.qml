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
    spacing: 6
    Loader { active: controls.wide; visible: active; sourceComponent: Component { RaceViewChoices { backend: controls.backend } } }
    SButton {
        objectName: "raceViewMenu"
        visible: !controls.wide
        text: controls.fields.paused ? (controls.round.options || [])[controls.round.value] + " ▾" : qsTr("Wyniki ▾")
        quiet: true; font.pointSize: Theme.caption; padding: 6
        implicitHeight: Theme.controlHeight
        onClicked: controls.backend.openResultViewOptions()
    }
    SButton {
        objectName: "returnToCompetition"
        visible: !!controls.fields.paused
        text: qsTr("Wróć do konkursu"); primary: true
        font.pointSize: Theme.caption; padding: 6; implicitHeight: Theme.controlHeight
        onClicked: controls.backend.edit(controls.round.id,0)
    }
}
