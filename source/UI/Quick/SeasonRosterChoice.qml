import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: choice
    property var backend
    property string controlName: "seasonActiveRoster"
    readonly property var entries: backend ? [{id:"-1",name:qsTr("Wszyscy"),count:0}].concat(backend.rosterEntries) : []
    spacing: Theme.dp(5)
    RowLayout {
        Layout.fillWidth: true
        Label { text: qsTr("Aktywna lista zawodników"); color: Theme.ink; font.pointSize: Theme.body; font.bold: true; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight }
        SButton { text: qsTr("Zarządzaj"); quiet: true; implicitHeight: Theme.dp(28); font.pointSize: Theme.caption; padding: 2; onClicked: choice.backend.openRosterWorkspace() }
        SButton { objectName: "activeRosterCreate"; text: "+"; implicitHeight: Theme.dp(28); implicitWidth: Theme.dp(28); Accessible.name: qsTr("Nowa lista"); padding: 2; onClicked: choice.backend.createRoster() }
    }
    ListView {
        objectName: choice.controlName
        Layout.fillWidth: true; Layout.preferredHeight: Math.max(Theme.dp(56),Theme.bodyMetrics.height+Theme.captionMetrics.height+Theme.dp(24))
        orientation: ListView.Horizontal; model: choice.entries; spacing: Theme.dp(6); clip: true
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.horizontal: ScrollBar {}
        delegate: RosterCard {
            required property var modelData
            width: Theme.dp(170); height: ListView.view.height; condensed: true
            title: modelData.name
            detail: modelData.id==="-1" ? "" : !modelData.count ? qsTr("Wszyscy skoczkowie") : qsTr("Skoczkowie: %1").arg(modelData.count)
            checked: choice.backend.activeRosterId===modelData.id; activeRoster: checked
            objectName: "activeRosterCard-"+modelData.id
            onClicked: choice.backend.selectActiveRoster(modelData.id)
        }
    }
    Label { visible: !!choice.backend && !!choice.backend.rosterNotice; text: choice.backend ? choice.backend.rosterNotice : ""; color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
}
