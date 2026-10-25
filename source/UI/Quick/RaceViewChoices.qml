import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
GridLayout {
    id: choices
    signal selectionChanged()
    property var backend
    property bool vertical: false
    readonly property var history: backend ? backend.find("resultsHistory") : null
    readonly property var fields: history ? history.values : ({})
    readonly property var round: fields.roundField ? fields.roundField.values : ({})
    readonly property var mode: fields.modeField ? fields.modeField.values : ({})
    columns: vertical ? 1 : 3
    columnSpacing: 6; rowSpacing: 6
    RaceViewSelector {
        objectName: "historyRoundSelector"
        backend: choices.backend; field: 0
        visible: (choices.round.options || []).length>1
        model: choices.round.options || []; currentIndex: choices.round.value || 0
        enabled: choices.round.enabled !== false
        Layout.preferredWidth: 170; Layout.fillWidth: true; Layout.minimumWidth: 0
        Accessible.name: qsTr("Wyniki po serii")
        onActivated: function(index){choices.backend.edit(choices.round.id,index);choices.selectionChanged()}
    }
    RaceViewSelector {
        objectName: "teamResultsMode"
        backend: choices.backend; field: 2
        visible: !!choices.fields.modeField && !choices.fields.active
        model: [qsTr("Tylko wyniki bieżącej drużyny"),qsTr("Tylko suma drużyny"),qsTr("Pokaż pełne wyniki")]
        currentIndex: appearance.teamResultsMode
        Layout.preferredWidth: 230; Layout.fillWidth: true; Layout.minimumWidth: 0
        Accessible.name: qsTr("Widok wyników drużynowych")
        onActivated: function(index){appearance.teamResultsMode=index;choices.selectionChanged()}
    }
    RaceViewSelector {
        objectName: "historyStandingsMode"
        backend: choices.backend; field: 1
        visible: !!choices.fields.modeField
        model: choices.mode.options || []; currentIndex: choices.mode.value || 0
        enabled: choices.mode.enabled !== false
        Layout.preferredWidth: 190; Layout.fillWidth: true; Layout.minimumWidth: 0
        Accessible.name: qsTr("Klasyfikacja drużynowa lub indywidualna")
        onActivated: function(index){choices.backend.edit(choices.mode.id,index);choices.selectionChanged()}
    }
}
