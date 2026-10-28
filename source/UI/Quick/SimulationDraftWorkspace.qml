import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: workspace
    objectName: "simulationDraftWorkspace"
    property var node
    readonly property var backend: node.values.backend
    readonly property bool hill: !!node.values.hill
    spacing: Theme.dp(6)
    RowLayout {
        Layout.fillWidth: true
        Label { text: qsTr("Zmiany tylko dla tej symulacji"); color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        SButton { text: qsTr("Przywróć z paczki"); enabled: workspace.backend.valid; onClicked: {workspace.forceActiveFocus(); workspace.backend.reset()} }
    }
    Label { visible: text.length>0; text: workspace.backend.message; color: Theme.danger; font.pointSize: Theme.body; wrapMode: Text.WordWrap; Layout.fillWidth: true }
    EmptyState {
        visible: !workspace.backend.valid
        Layout.fillWidth: true; Layout.fillHeight: true
        title: workspace.hill ? qsTr("Nie wybrano skoczni") : qsTr("Nie wybrano zawodnika")
        description: qsTr("Wybierz paczkę i jej pozycję w sekcji Konfiguracja.")
    }
    Loader {
        active: workspace.backend.valid; visible: active
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
        sourceComponent: workspace.hill ? hillEditor : athleteEditor
    }
    Component { id: hillEditor; HillEditor { backend: workspace.backend } }
    Component {
        id: athleteEditor
        ScrollView {
            id: athleteScroll
            contentWidth: availableWidth; clip: true
            AthletePatches { width: athleteScroll.availableWidth; backend: workspace.backend }
        }
    }
}
