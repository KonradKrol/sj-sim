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
    readonly property var history: ui.find("resultsHistory")
    readonly property bool previewPaused: !!history && !!history.values.paused
    property real heroReserve: 0
    onPreviewPausedChanged: {if(previewPaused)heroReserve=hero.height;else{heroReserve=0;if(viewOptions)viewOptions.close()}}
    readonly property bool historical: !!history && !!history.values.active
    readonly property var shownResults: historical ? history.values.tableField : resultNode
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
        RaceStatus { enabled: !race.previewPaused; opacity: race.previewPaused ? .45 : 1; width: parent.width; height: implicitHeight; onConditionsRequested: race.openOptions(1) }
        GridLayout {
            id: hero
            objectName: "raceHeaderCards"
            columns: race.narrow ? 1 : 2
            width: Math.min(parent.width,1052); height: race.previewPaused ? Math.max(implicitHeight,race.heroReserve) : implicitHeight
            columnSpacing: 12; rowSpacing: 8
            RaceAthlete { enabled: !race.previewPaused; opacity: race.previewPaused ? .45 : 1; Layout.alignment: Qt.AlignTop; Layout.maximumHeight: implicitHeight; dense: race.height < 600; Layout.row: race.narrow ? 1 : 0; Layout.column: 0; Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: race.narrow ? 0 : 280; Layout.maximumWidth: race.narrow ? Infinity : 320 }
            RaceLastJump { Layout.alignment: Qt.AlignTop; Layout.maximumHeight: implicitHeight; dense: race.height < 600; Layout.row: 0; Layout.column: race.narrow ? 0 : 1; Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: race.narrow ? 0 : 680; Layout.maximumWidth: race.narrow ? Infinity : 720 }
        }
        RaceControls { opacity: race.previewPaused ? .45 : 1; width: Math.min(parent.width,1052); height: implicitHeight; onOptionsRequested: race.openOptions(section) }
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
                RowLayout {
                    Layout.fillWidth: true
                    Label { text: qsTr("Wyniki"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.body; font.weight: Font.ExtraBold; Layout.fillWidth: true }
                }
                DataTable {
                    id: results
                    objectName: "raceResults"
                    Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 140
                    table: race.shownResults ? race.shownResults.values.table : null
                    ownerId: race.shownResults ? race.shownResults.values.id : -1
                    actions: race.resultNode ? race.resultNode.values.tableActions || [] : []
                    plain: true; searchEnabled: false
                    function applyTeamMode(){if(!race.historical && teamResults && table)table.setTeamMode(appearance.teamResultsMode)}
                    onTableChanged: {clearDisabledSearch();applyTeamMode()}
                    onTeamResultsChanged: applyTeamMode()
                    Component.onCompleted: applyTeamMode()
                    Connections { target: appearance; function onChanged(){results.applyTeamMode()} }
                    compact: true; raceResults: race.historical ? !race.history.values.team : race.resultNode === race.individual
                    teamResults: race.historical ? !!race.history.values.team : race.resultNode === race.team
                    title: qsTr("Wyniki")
                }
                }
            }
            Item {
                enabled: !race.previewPaused; opacity: race.previewPaused ? .45 : 1
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
                    compact: true; followCurrent: true; searchEnabled: false; title: qsTr("Lista startowa")
                }
                Node {
                    objectName: "raceKoGroup"
                    readonly property var entry: ui.find("tableView_KOGroupResults")
                    node: entry && entry.values.available !== false ? entry : null
                    Layout.fillWidth: true; Layout.preferredHeight: visible ? 130 : 0
                    compactTable: true; plainTable: true; tableHeightLimit: 130; tableSearchEnabled: false
                }
                }
            }
        }
    }
    Connections { target: ui; function onResultViewOptionsRequested(field,x,y,width){var origin=viewOptions.parent.mapToGlobal(0,0);viewOptions.anchorX=x<0 ? -1 : x-origin.x;viewOptions.anchorY=y<0 ? 8 : Math.max(4,y-origin.y+4);viewOptions.anchorWidth=width;viewOptions.selector=field;viewOptions.open()} }
    Popup {
        id: viewOptions
        objectName: "raceViewOptions"
        property int selector: -1
        property real anchorX: -1
        property real anchorY: 8
        property real anchorWidth: 0
        readonly property var fields: race.history ? race.history.values : ({})
        readonly property var field: selector===0 ? fields.roundField : fields.modeField
        readonly property var options: selector===2 ? [qsTr("Tylko wyniki bieżącej drużyny"),qsTr("Tylko suma drużyny"),qsTr("Pokaż pełne wyniki")] : field ? field.values.options || [] : []
        readonly property int selected: selector===2 ? appearance.teamResultsMode : field ? field.values.value || 0 : 0
        readonly property int menuCount: ((fields.roundField && fields.roundField.values.options.length>1) ? 1 : 0)+(fields.modeField ? (fields.active ? 1 : 2) : 0)
        readonly property real optionHeight: Math.max(40,Theme.bodyMetrics.height*2+12)
        function choose(index){if(index<0 || index>=options.length)return;if(selector===2)appearance.teamResultsMode=index;else if(field)ui.edit(field.values.id,index);close()}
        function focusChoices(){directOptions.currentIndex=selected;directOptions.forceActiveFocus()}
        onOpened: if(selector>=0)focusChoices()
        onSelectorChanged: if(visible && selector>=0)Qt.callLater(focusChoices)
        parent: Overlay.overlay
        x: Math.max(8,Math.min(anchorX<0 ? parent.width-width-8 : anchorX,parent.width-width-8)); y: anchorY
        width: Math.min(selector<0 ? 330 : Math.max(280,anchorWidth),parent.width-16); height: Math.min(Math.max(40,parent.height-y-8),24+(selector<0 ? (Theme.controlHeight+6)*menuCount : optionHeight*options.length))
        modal: false; dim: false; focus: true; padding: 12
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        contentItem: ColumnLayout {
            RaceViewChoices { id: viewChoices; visible: viewOptions.selector<0; Layout.fillWidth: true; backend: ui; vertical: true; onSelectionChanged: viewOptions.close() }
            ListView {
                id: directOptions
                objectName: "raceResultViewOptions"
                visible: viewOptions.selector>=0
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
                model: viewOptions.selector>=0 ? viewOptions.options : []
                clip: true
                keyNavigationWraps: true
                Keys.onReturnPressed: viewOptions.choose(currentIndex)
                Keys.onEnterPressed: viewOptions.choose(currentIndex)
                Keys.onSpacePressed: viewOptions.choose(currentIndex)
                ScrollBar.vertical: ScrollBar {}
                delegate: ItemDelegate {
                    objectName: "raceResultViewOption-"+index
                    width: directOptions.width; height: viewOptions.optionHeight
                    highlighted: index===directOptions.currentIndex
                    contentItem: Label { text: modelData; color: Theme.ink; font.pointSize: Theme.body; wrapMode: Text.WordWrap }
                    background: Rectangle { color: parent.highlighted ? Theme.selection : parent.hovered ? Theme.raised : Theme.surface; radius: 4 }
                    onClicked: viewOptions.choose(index)
                }
            }
        }
        background: Rectangle { color: Theme.surface; border.color: Theme.outline; radius: Theme.radius }
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
