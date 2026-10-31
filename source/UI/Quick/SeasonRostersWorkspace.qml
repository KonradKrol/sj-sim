import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: workspace
    objectName: "seasonRostersWorkspace"
    property var node
    readonly property var backend: node ? node.values.backend : null
    readonly property bool narrow: width < Theme.dp(1050)
    readonly property bool hasSelection: !!backend && backend.editingRosterId.length>0
    ColumnLayout {
        anchors.fill: parent; spacing: Theme.dp(12)
        RowLayout {
            Layout.fillWidth: true
            Label { text: qsTr("Listy zawodników"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight }
            SButton { visible: workspace.backend.canRestoreRoster; text: qsTr("Cofnij usunięcie"); quiet: true; onClicked: workspace.backend.restoreRoster() }
            SButton { objectName: "rosterCreate"; text: qsTr("Nowa lista"); primary: true; onClicked: {workspace.forceActiveFocus();workspace.backend.createRoster()} }
        }
        RowLayout {
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
            spacing: Theme.dp(14)
            ColumnLayout {
                visible: !workspace.narrow && workspace.backend.rosterEntries.length>0
                Layout.preferredWidth: Theme.dp(230); Layout.maximumWidth: Theme.dp(300); Layout.fillHeight: true; Layout.minimumHeight: 0
                Label { text: qsTr("Wybierz listę do edycji"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                ListView {
                    objectName: "rosterCatalog"
                    Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
                    model: workspace.backend.rosterEntries; spacing: Theme.dp(7); clip: true
                    ScrollBar.vertical: ScrollBar {}
                    delegate: RosterCard {
                        required property var modelData
                        width: ListView.view.width
                        title: modelData.name; detail: modelData.count ? qsTr("Skoczkowie: %1").arg(modelData.count) : qsTr("Wszyscy skoczkowie")
                        checked: modelData.editing; activeRoster: modelData.active
                        objectName: "rosterCard-"+modelData.id
                        onClicked: {workspace.forceActiveFocus();workspace.backend.selectRoster(modelData.id)}
                    }
                }
            }
            ColumnLayout {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0; spacing: Theme.dp(10)
                ListView {
                    visible: workspace.narrow && workspace.backend.rosterEntries.length>0
                    Layout.fillWidth: true; Layout.preferredHeight: Theme.dp(68); Layout.minimumHeight: 0
                    model: workspace.backend.rosterEntries; orientation: ListView.Horizontal; spacing: Theme.dp(7); clip: true
                    ScrollBar.horizontal: ScrollBar {}
                    delegate: RosterCard {
                        required property var modelData
                        width: Theme.dp(190); height: ListView.view.height; condensed: true
                        title: modelData.name; detail: modelData.count ? qsTr("Skoczkowie: %1").arg(modelData.count) : qsTr("Wszyscy skoczkowie")
                        checked: modelData.editing; activeRoster: modelData.active
                        objectName: "compactRosterCard-"+modelData.id
                        onClicked: {workspace.forceActiveFocus();workspace.backend.selectRoster(modelData.id)}
                    }
                }
                GridLayout {
                    visible: workspace.hasSelection
                    Layout.fillWidth: true; columns: width<Theme.dp(650) ? 2 : 3; columnSpacing: Theme.dp(8); rowSpacing: Theme.dp(6)
                    SField {
                        id: nameInput
                        objectName: "rosterName"
                        property string editingId: ""
                        Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.columnSpan: parent.columns===2 ? 2 : 1
                        text: workspace.backend.editingRosterName
                        placeholderText: qsTr("Nazwa listy"); Accessible.name: placeholderText; maximumLength: 160
                        onActiveFocusChanged: if(activeFocus)editingId=workspace.backend.editingRosterId
                        onEditingFinished: if(editingId.length && !workspace.backend.renameRoster(editingId,text))text=Qt.binding(function(){return workspace.backend.editingRosterName})
                    }
                    SButton {
                        objectName: "rosterSetActive"
                        text: workspace.backend.activeRosterId===workspace.backend.editingRosterId ? qsTr("Aktywna lista") : qsTr("Użyj w konkursach")
                        primary: workspace.backend.activeRosterId!==workspace.backend.editingRosterId
                        enabled: workspace.backend.activeRosterId!==workspace.backend.editingRosterId
                        Layout.fillWidth: parent.columns===2
                        onClicked: {workspace.forceActiveFocus();workspace.backend.selectActiveRoster(workspace.backend.editingRosterId)}
                    }
                    SButton { objectName: "rosterDelete"; text: qsTr("Usuń listę"); quiet: true; foreground: Theme.danger; Layout.fillWidth: parent.columns===2; onClicked: {workspace.forceActiveFocus();workspace.backend.removeRoster(workspace.backend.editingRosterId)} }
                }
                Loader {
                    visible: active; active: workspace.hasSelection
                    Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
                    sourceComponent: Component { SavedRosterWorkspace { backend: workspace.backend.rosterEditor; inlineEditor: true } }
                }
                EmptyState {
                    visible: !workspace.hasSelection
                    Layout.fillWidth: true; Layout.fillHeight: true
                    title: qsTr("Utwórz listę zawodników")
                    description: qsTr("Dodaj listę, nadaj jej nazwę i wybierz skoczków. Później możesz jej używać przed konkursami lub przypisać ją do kalendarza.")
                    actionText: qsTr("Nowa lista"); onTriggered: workspace.backend.createRoster()
                }
            }
        }
        Label { visible: !!workspace.backend.rosterNotice; text: workspace.backend.rosterNotice; color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    }
}
