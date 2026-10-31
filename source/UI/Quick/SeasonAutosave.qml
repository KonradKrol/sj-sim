import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: autosave
    property var node
    readonly property var backend: node ? node.values.backend : null
    spacing: Theme.dp(2)
    RowLayout {
        Layout.fillWidth: true; spacing: Theme.dp(8)
        Switch {
            objectName: "seasonAutosave"
            text: qsTr("Automatyczny zapis")
            palette.windowText: Theme.ink
            palette.text: Theme.ink
            palette.buttonText: Theme.ink
            checked: autosave.backend ? autosave.backend.autosaveEnabled : false
            onClicked: if (autosave.backend) autosave.backend.setAutosaveEnabled(checked)
        }
        Label {
            text: autosave.backend && autosave.backend.autosaveEnabled ? qsTr("Włączony") : qsTr("Wyłączony")
            color: autosave.backend && autosave.backend.autosaveEnabled ? Theme.success : Theme.muted
            font.pointSize: Theme.caption; font.bold: true
            Layout.fillWidth: true; horizontalAlignment: Text.AlignRight
        }
    }
    Label {
        text: autosave.backend && autosave.backend.autosaveEnabled
              ? qsTr("Po konkursie zapis następuje od razu, a po innych zmianach po 15 sekundach. Zmiana tego ustawienia zapisuje obecny stan sezonu.")
              : qsTr("Automatyczny zapis jest wyłączony. Zapisz sezon ręcznie. Zmiana tego ustawienia zapisuje obecny stan sezonu.")
        color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap
    }
}
