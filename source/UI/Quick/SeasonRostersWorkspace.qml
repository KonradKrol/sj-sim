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
        anchors.fill: parent; spacing: Theme.dp(workspace.height < 500 ? 4 : 12)
        RowLayout {
            visible: !workspace.narrow
            Layout.fillWidth: true
            Label { text: qsTr("Listy zawodników"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight }
            SButton { visible: workspace.backend.canRestoreRoster; text: qsTr("Cofnij usunięcie"); quiet: true; onClicked: workspace.backend.restoreRoster() }
            SButton { objectName: workspace.narrow ? "" : "rosterCreate"; text: qsTr("Nowa lista"); primary: true; onClicked: {workspace.forceActiveFocus();workspace.backend.createRoster()} }
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
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0; spacing: Theme.dp(workspace.height < 500 ? 4 : 10)
                RowLayout {
                    visible: workspace.narrow
                    Layout.fillWidth: true; Layout.preferredHeight: Math.max(32,Theme.dp(48)); spacing: Theme.dp(6)
                    ListView {
                    Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
                    model: workspace.backend.rosterEntries; orientation: ListView.Horizontal; spacing: Theme.dp(7); clip: true
                    ScrollBar.horizontal: ScrollBar {}
                    delegate: RosterCard {
                        required property var modelData
                        width: Theme.dp(190); height: ListView.view.height; condensed: true; hideDetail: true
                        title: modelData.name; detail: modelData.count ? qsTr("Skoczkowie: %1").arg(modelData.count) : qsTr("Wszyscy skoczkowie")
                        checked: modelData.editing; activeRoster: modelData.active
                        objectName: "compactRosterCard-"+modelData.id
                        onClicked: {workspace.forceActiveFocus();workspace.backend.selectRoster(modelData.id)}
                    }
                }
                    SButton { objectName: "rosterCreate"; text: "+"; primary: true; dense: true; Layout.preferredWidth: 36; Layout.fillHeight: true; Accessible.name: qsTr("Nowa lista"); onClicked: {workspace.forceActiveFocus();workspace.backend.createRoster()} }
                    SButton { visible: workspace.backend.canRestoreRoster; text: "↶"; dense: true; quiet: true; Layout.preferredWidth: 36; Layout.fillHeight: true; Accessible.name: qsTr("Cofnij usunięcie"); onClicked: workspace.backend.restoreRoster() }
                }
                GridLayout {
                    visible: workspace.hasSelection
                    Layout.fillWidth: true; columns: 3; columnSpacing: Theme.dp(8); rowSpacing: Theme.dp(6)
                    SField {
                        id: nameInput
                        objectName: "rosterName"
                        property string editingId: ""
                        Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.columnSpan: 1
                        text: workspace.backend.editingRosterName
                        placeholderText: qsTr("Nazwa listy"); Accessible.name: placeholderText; maximumLength: 160
                        onActiveFocusChanged: if(activeFocus)editingId=workspace.backend.editingRosterId
                        onEditingFinished: if(editingId.length && !workspace.backend.renameRoster(editingId,text))text=Qt.binding(function(){return workspace.backend.editingRosterName})
                    }
                    SButton {
                        objectName: "rosterSetActive"
                        text: workspace.backend.activeRosterId===workspace.backend.editingRosterId ? (workspace.narrow ? qsTr("Aktywna") : qsTr("Aktywna lista")) : (workspace.narrow ? qsTr("Aktywuj") : qsTr("Użyj w konkursach"))
                        Accessible.name: workspace.backend.activeRosterId===workspace.backend.editingRosterId ? qsTr("Aktywna lista") : qsTr("Użyj w konkursach")
                        dense: workspace.narrow
                        primary: workspace.backend.activeRosterId!==workspace.backend.editingRosterId
                        enabled: workspace.backend.activeRosterId!==workspace.backend.editingRosterId
                        Layout.fillWidth: false
                        onClicked: {workspace.forceActiveFocus();workspace.backend.selectActiveRoster(workspace.backend.editingRosterId)}
                    }
                    SButton { objectName: "rosterDelete"; text: workspace.narrow ? qsTr("Usuń") : qsTr("Usuń listę"); Accessible.name: qsTr("Usuń listę"); dense: workspace.narrow; quiet: true; foreground: Theme.danger; onClicked: {workspace.forceActiveFocus();workspace.backend.removeRoster(workspace.backend.editingRosterId)} }
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
                    description: qsTr("Dodaj listę i wybierz skoczków. Możesz przypisać ją do kalendarza.")
                }
            }
        }
        Label { visible: !!workspace.backend.rosterNotice; text: workspace.backend.rosterNotice; color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    }
}
