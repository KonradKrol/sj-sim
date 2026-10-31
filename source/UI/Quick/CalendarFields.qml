import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: editorFields
    property var backend
    signal editRules(int target)
    spacing: 12
    CalendarDateField {
        visible: editorFields.backend.datesEnabled
        Layout.fillWidth: true; backend: editorFields.backend; name: "event"
        label: qsTr("Data zawodów (także dla treningów i serii próbnej)")
        mixed: editorFields.backend.selection.date===-1
        value: typeof editorFields.backend.selection.date==="string" ? editorFields.backend.selection.date : ""
        onCommitted: function(iso){editorFields.backend.assignDate(iso)}
    }
    Repeater {
        model: [
            {key:"hill",label:qsTr("Skocznia")},
            {key:"type",label:qsTr("Wydarzenie")},
            {key:"kind",label:qsTr("Rodzaj konkursu")},
            {key:"rules",label:qsTr("Zasady konkursu")}
        ]
        ColumnLayout {
            property var field: modelData
            Layout.fillWidth: true; spacing: 4
            Label { text: parent.field.label; color: Theme.muted; font.pointSize: Theme.caption }
            SChoice {
                objectName: "calendar-field-" + parent.field.key
                Layout.fillWidth: true
                searchable: parent.field.key === "hill"
                model: parent.field.key === "hill" ? editorFields.backend.hills : parent.field.key === "rules" ? editorFields.backend.rules : parent.field.key === "type" ? [qsTr("Konkurs"),qsTr("Kwalifikacje")] : [qsTr("Indywidualny"),qsTr("Drużynowy")]
                optionImages: parent.field.key === "hill" ? editorFields.backend.hillFlags : []
                currentIndex: editorFields.backend.selection[parent.field.key] === undefined ? -1 : editorFields.backend.selection[parent.field.key]
                displayText: currentIndex < 0 ? qsTr("Różne wartości") : currentText
                Accessible.name: parent.field.label
                onActivated: function(index) { if(parent.field.key === "rules" && index === 0)editorFields.editRules(0);else {var patch={};patch[parent.field.key]=index;editorFields.backend.update(patch)} }
            }
        }
    }
    Label { text: qsTr("Treningi"); color: Theme.muted; font.pointSize: Theme.caption }
    NumberField { Layout.fillWidth: true; direct: false; label: qsTr("Liczba treningów"); fields: ({value:Math.max(0,editorFields.backend.selection.trainings || 0),mixed:editorFields.backend.selection.trainings === -1,minimum:0,maximum:30,step:1,decimals:0}); onCommitted: editorFields.backend.update({trainings:value}) }
    ColumnLayout {
        visible: editorFields.backend.selection.trainings !== 0
        Layout.fillWidth: true; spacing: 4
        Label { text: qsTr("Zasady treningów"); color: Theme.muted; font.pointSize: Theme.caption }
        SChoice { Layout.fillWidth: true; model: editorFields.backend.auxiliaryRules; currentIndex: editorFields.backend.selection.trainingRules || 0; Accessible.name: qsTr("Zasady treningów"); onActivated: currentIndex === 1 ? editorFields.editRules(1) : editorFields.backend.update({trainingRules:currentIndex}) }
    }
    SCheck { text: qsTr("Seria próbna"); Layout.fillWidth: true; tristate: true; checkState: editorFields.backend.selection.trial === -1 ? Qt.PartiallyChecked : editorFields.backend.selection.trial ? Qt.Checked : Qt.Unchecked; onClicked: editorFields.backend.update({trial:checked}) }
    ColumnLayout {
        visible: editorFields.backend.selection.trial !== 0
        Layout.fillWidth: true; spacing: 4
        Label { text: qsTr("Zasady serii próbnej"); color: Theme.muted; font.pointSize: Theme.caption }
        SChoice { Layout.fillWidth: true; model: editorFields.backend.auxiliaryRules; currentIndex: editorFields.backend.selection.trialRules || 0; Accessible.name: qsTr("Zasady serii próbnej"); onActivated: currentIndex === 1 ? editorFields.editRules(2) : editorFields.backend.update({trialRules:currentIndex}) }
    }
}
