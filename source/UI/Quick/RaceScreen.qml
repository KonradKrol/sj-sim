import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: race
    contentWidth: availableWidth
    contentHeight: raceContent.height
    clip: true
    padding: width < 700 ? 8 : 12
    function openOptions(section) {optionContent.section=section;options.open()}
    readonly property bool narrow: width < 700
    readonly property var individual: ui.find("tableView_results")
    readonly property var team: ui.find("quickTeamResults")
    readonly property var resultNode: individual && individual.values.available !== false ? individual : team
    Item {
        id: raceContent
        width: race.availableWidth
        height: header.height+8+tables.height
        Column {
        id: header
        width: parent.width
        spacing: 8
        RaceStatus { width: parent.width; height: implicitHeight; onConditionsRequested: race.openOptions(1) }
        GridLayout {
            id: hero
            columns: race.narrow ? 1 : 2
            width: parent.width; height: implicitHeight
            columnSpacing: 12; rowSpacing: 8
            RaceAthlete { Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: 0 }
            RaceLastJump { Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: 0 }
        }
        RaceControls { width: parent.width; height: implicitHeight; onOptionsRequested: race.openOptions(0) }
        }
        GridLayout {
            id: tables
            y: header.height+8
            width: parent.width
            height: Math.max(race.narrow?360:170,race.availableHeight-y)
            columns: race.narrow ? 1 : 2
            columnSpacing: 12; rowSpacing: 8
            ColumnLayout {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0
                Layout.preferredWidth: 640
                Label { text: qsTr("Wyniki"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.body; font.weight: Font.ExtraBold }
                DataTable {
                    id: results
                    objectName: "raceResults"
                    Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 140
                    table: race.resultNode ? race.resultNode.values.table : null
                    ownerId: race.resultNode ? race.resultNode.values.id : -1
                    actions: race.resultNode ? race.resultNode.values.tableActions || [] : []
                    compact: true; raceResults: race.resultNode === race.individual
                    teamResults: race.resultNode === race.team
                    title: qsTr("Wyniki")
                }
            }
            ColumnLayout {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0
                Layout.preferredWidth: 330
                Label { text: qsTr("Lista startowa"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.body; font.weight: Font.ExtraBold }
                DataTable {
                    objectName: "raceStartList"
                    readonly property var entry: ui.find("listView_startList")
                    Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 140
                    table: entry ? entry.values.table : null; ownerId: entry ? entry.values.id : -1
                    compact: true; followCurrent: true; title: qsTr("Lista startowa")
                }
                Node {
                    objectName: "raceKoGroup"
                    readonly property var entry: ui.find("tableView_KOGroupResults")
                    node: entry && entry.values.available !== false ? entry : null
                    Layout.fillWidth: true; Layout.preferredHeight: visible ? 130 : 0
                    compactTable: true; tableHeightLimit: 130
                }
            }
        }
    }
    Popup {
        id: options
        objectName: "raceOptionsPanel"
        parent: Overlay.overlay
        x: Math.max(8,parent.width-width-8); y: 8
        width: Math.min(440,parent.width-16); height: Math.max(100,parent.height-16)
        modal: true; focus: true; padding: 12
        onOpened: optionContent.focusClose()
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        background: Rectangle { color: Theme.surface; border.color: Theme.outline; radius: Theme.radius }
        contentItem: RaceOptions { id: optionContent; onCloseRequested: options.close() }
    }
}
