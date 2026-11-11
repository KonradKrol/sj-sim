import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

GridLayout {
    id: autosave
    property var node
    readonly property var backend: node ? node.values.backend : null
    readonly property var formNode: ui.find("automaticFormEnabled")
    readonly property var datesNode: ui.find("checkBox_calendarDates")
    columns: width>=Theme.dp(780) ? 3 : width>=Theme.dp(520) ? 2 : 1
    columnSpacing: Theme.dp(8); rowSpacing: Theme.dp(8)
        SeasonAutomationToggle {
            objectName: "seasonDates"
            Layout.fillWidth: true; Layout.minimumWidth: 0
            text: qsTr("Daty zawodów")
            checked: !!autosave.datesNode && !!autosave.datesNode.values.value
            enabled: !!autosave.datesNode && autosave.datesNode.values.enabled!==false
            detail: qsTr("Umożliwiają formę dnia i autoformę.")
            explanation: qsTr("Używaj dat wydarzeń w sezonie.")
            onClicked: if(autosave.datesNode)ui.activate(autosave.datesNode.values.id)
        }
        SeasonAutomationToggle {
            objectName: "seasonAutosave"
            Layout.fillWidth: true; Layout.minimumWidth: 0
            text: qsTr("Autozapis")
            checked: autosave.backend ? autosave.backend.autosaveEnabled : false
            detail: checked ? qsTr("Po konkursie od razu, inne zmiany po 15 s.") : qsTr("Zapisuj sezon ręcznie.")
            explanation: qsTr("Przełączenie zapisuje obecny stan sezonu.")
            onClicked: if (autosave.backend) autosave.backend.setAutosaveEnabled(checked)
        }
        SeasonAutomationToggle {
            objectName: "seasonAutomaticForm"
            Layout.fillWidth: true; Layout.minimumWidth: 0
            text: qsTr("Autoforma")
            checked: !!autosave.formNode && !!autosave.formNode.values.value
            enabled: !!autosave.formNode && autosave.formNode.values.enabled!==false
            detail: enabled ? qsTr("Dyspozycja zmienia się wraz z datami zawodów.") : qsTr("Włącz daty wydarzeń.")
            explanation: qsTr("Wyłączenie zachowuje formę. Ponowne włączenie nie nadrabia przerwy.")
            onClicked: if(autosave.formNode)ui.activate(autosave.formNode.values.id)
        }
}
