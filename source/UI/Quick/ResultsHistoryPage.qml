import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
ColumnLayout {
    id: page
    readonly property var fields: ui.root.values
    readonly property var entry: fields.tableField || null
    spacing: Theme.dp(8)
    RowLayout { Layout.fillWidth: true; HistoryControls { history: ui.root; Layout.fillWidth: true; Layout.maximumWidth: 480 } Item { Layout.fillWidth: true } Node { node: page.fields.actionsField || null } }
    GridLayout {
        columns: width>=900 ? 2 : 1
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
        DataTable { objectName: "archiveHistoryResults"; table: page.entry ? page.entry.values.table : null; ownerId: page.entry ? page.entry.values.id : -1; plain: true; searchEnabled: false; raceResults: !page.fields.team; teamResults: !!page.fields.team; Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 120; Layout.minimumWidth: 0 }
        ScrollView { id: details; visible: !!ui.jumpSummary.name; Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0; Layout.maximumWidth: page.width>=900 ? 650 : Infinity; contentWidth: availableWidth; clip: true; RaceLastJump { width: details.availableWidth; dense: true } }
    }
}
