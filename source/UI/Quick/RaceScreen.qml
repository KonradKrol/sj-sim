import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: race
    objectName: "raceWorkspace"
    contentWidth: availableWidth
    contentHeight: raceContent.height
    clip: true
    padding: width < 700 || height < 600 ? 8 : 12
    function openOptions(section) {optionContent.section=section;options.open()}
    readonly property bool narrow: width < 700
    readonly property var individual: ui.find("tableView_results")
    readonly property var team: ui.find("quickTeamResults")
    readonly property var koNode: ui.find("tableView_KOGroupResults")
    readonly property bool hasKo: !!koNode && koNode.values.available !== false
    readonly property var resultNode: individual && individual.values.available !== false ? individual : team
    Item {
        id: raceContent
        width: race.availableWidth
        height: header.height+8+tables.height
        Column {
        id: header
        width: parent.width
        spacing: race.height < 600 ? 4 : 8
        RaceStatus { width: parent.width; height: implicitHeight; onConditionsRequested: race.openOptions(1) }
        GridLayout {
            id: hero
            objectName: "raceHeaderCards"
            columns: race.narrow ? 1 : 2
            width: Math.min(parent.width,1052); height: implicitHeight
            columnSpacing: 12; rowSpacing: 8
            RaceAthlete { Layout.alignment: Qt.AlignTop; Layout.maximumHeight: implicitHeight; dense: race.height < 600; Layout.row: race.narrow ? 1 : 0; Layout.column: 0; Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: race.narrow ? 0 : 280; Layout.maximumWidth: race.narrow ? Infinity : 320 }
            RaceLastJump { Layout.alignment: Qt.AlignTop; Layout.maximumHeight: implicitHeight; dense: race.height < 600; Layout.row: 0; Layout.column: race.narrow ? 0 : 1; Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: race.narrow ? 0 : 680; Layout.maximumWidth: race.narrow ? Infinity : 720 }
        }
        RaceControls { width: Math.min(parent.width,1052); height: implicitHeight; onOptionsRequested: race.openOptions(section) }
        }
        Item {
            id: tables
            objectName: "raceTables"
            y: header.height+8
            width: parent.width
            height: Math.max(race.narrow ? (race.hasKo ? 500 : 400) : 170,race.availableHeight-y)
            readonly property real resultWidth: race.narrow ? width : (width-12)*.66
            readonly property real resultHeight: !race.narrow ? height : race.hasKo ? Math.max(180,height-308) : (height-8)/2
            Item {
                width: tables.resultWidth; height: tables.resultHeight
                ColumnLayout {
                anchors.fill: parent
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
            }
            Item {
                x: race.narrow ? 0 : tables.resultWidth+12
                y: race.narrow ? tables.resultHeight+8 : 0
                width: race.narrow ? tables.width : tables.width-x
                height: tables.height-y
                ColumnLayout {
                anchors.fill: parent
                Label { text: qsTr("Lista startowa"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.body; font.weight: Font.ExtraBold }
                DataTable {
                    objectName: "raceStartList"
                    readonly property var entry: ui.find("listView_startList")
                    Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 120
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
