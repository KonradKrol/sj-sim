import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

FocusScope {
    id: tableRoot
    objectName: "dataTable-" + ownerId
    enabled: !table || !table.frozen
    property var table
    property var actions: []
    property var columnWidths: []
    property int ownerId: -1
    property string title: ""
    property bool followCurrent: false
    property real maximumHeight: 540
    property bool compact: false
    property bool searchOpen: !compact && width >= 320 && height >= 240
    property bool jumpColumns: false
    property bool raceResults: false
    property bool teamResults: false
    property int sortColumn: -1
    property bool descending: false
    readonly property var headers: table ? table.headers : []
    readonly property int columns: Math.max(1, headers.length)
    readonly property var displayColumns: {
        var out=[]
        if(teamResults && columns>=5) return [0,1,4,3]
        if(raceResults && !jumpColumns && columns>4) {
            out=[0,1]
            for(var c=3;c<columns;c+=2)out.push(c)
            out.push(2)
        } else for(var i=0;i<(compact && columns>4 && !jumpColumns ? 3 : columns);++i)out.push(i)
        return out
    }
    readonly property int visibleColumns: displayColumns.length
    readonly property bool ranked: headers.length > 1 && /^(poz|miejsce|rank|place|lp|nr|#)/i.test(headers[0])
    readonly property int nameColumn: ranked && columns === 4 ? 2 : 1
    readonly property real totalWidth: {
        var total=0
        for(var c=0;c<visibleColumns;++c)total+=columnWidth(c)
        return total
    }
    implicitHeight: Math.min(maximumHeight, Math.max(240,(Window.window ? Window.window.height : 960) - 360), Math.max(compact ? 120 : 240, (table ? table.count : 0) * (compact ? 32 : 40) + (columns>1 ? 104 : 66)))
    activeFocusOnTab: true
    Accessible.role: Accessible.Table
    Accessible.name: title.length ? title : qsTr("Tabela danych")
    function columnWidth(column) {
        if(column>=visibleColumns)return 0
        if(columnWidths.length>column)return columnWidths[column]
        if(teamResults) {
            if(column===0)return 72
            if(column===1)return Math.max(140,width-276)
            return column===2 ? 120 : 84
        }
        if(raceResults) {
            if(column===0)return 44
            if(column===1)return Math.max(140,width-44-(visibleColumns-2)*76)
            return 76
        }
        if (ranked) {
            if (column === 0) return 56
            if (column === nameColumn) return Math.max(170,width - 58 - (visibleColumns-2)*96)
            return 96
        }
        return Math.max(column === 1 ? 190 : 130,width / columns)
    }
    function columnAt(x) {
        var offset = 0
        for (var c = 0; c < visibleColumns; ++c) { offset += columnWidth(c); if (x < offset) return displayColumns[c] }
        return displayColumns[visibleColumns - 1]
    }
    function editCurrent() {
        if (!table || table.currentRow < 0) return
        rows.positionViewAtIndex(table.currentRow,ListView.Contain)
        Qt.callLater(function() {
            var row = rows.itemAtIndex(table.currentRow)
            if (row) row.editCell(table.currentColumn)
        })
    }
    Rectangle { anchors.fill: parent; color: Theme.surface; radius: Theme.radius; border.color: tableRoot.activeFocus ? Theme.accent : Theme.divider; border.width: tableRoot.activeFocus ? 2 : 1 }
    ColumnLayout {
        anchors.fill: parent; anchors.margins: 1
        spacing: 0
        RowLayout {
            Layout.fillWidth: true
            Layout.margins: Theme.compact ? 2 : tableRoot.compact ? 4 : 8
            Label { text: tableRoot.table && tableRoot.table.selectedCount>0 && tableRoot.height<300 ? qsTr("Zaznaczono: %1").arg(tableRoot.table.selectedCount) : qsTr("%1 pozycji").arg(tableRoot.table ? tableRoot.table.count : 0); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight }
            SButton {
                implicitHeight: Theme.compact ? Math.max(26,Theme.bodyMetrics.height+4) : tableRoot.compact ? 28 : 40;
                visible: tableRoot.width >= 380 && tableRoot.actions.length > 0 && !!tableRoot.actions[0].insert
                text: "+"; quiet: true; Accessible.name: qsTr("Dodaj"); hint: qsTr("Dodaj")
                enabled: tableRoot.actions.length > 0 && tableRoot.actions[0].enabled
                onClicked: ui.triggerAction(tableRoot.actions[0].id)
            }
            SButton {
                implicitHeight: Theme.compact ? Math.max(26,Theme.bodyMetrics.height+4) : tableRoot.compact ? 28 : 40; visible: !!tableRoot.table && tableRoot.table.multiple && tableRoot.table.selectedCount>0 && tableRoot.height<300; text: "×"; quiet: true; Accessible.name: qsTr("Wyczyść zaznaczenie"); onClicked: tableRoot.table.clearSelection() }
            SButton {
                implicitHeight: Theme.compact ? Math.max(26,Theme.bodyMetrics.height+4) : tableRoot.compact ? 28 : 40; visible: !tableRoot.searchOpen; text: tableRoot.compact ? "⌕" : qsTr("Szukaj"); quiet: true; Accessible.name: qsTr("Szukaj"); hint: qsTr("Szukaj"); checkable: true; onClicked: tableRoot.searchOpen = checked }
            SButton {
                implicitHeight: Theme.compact ? Math.max(26,Theme.bodyMetrics.height+4) : tableRoot.compact ? 28 : 40; visible: tableRoot.compact && tableRoot.columns>4 && !tableRoot.teamResults; text: tableRoot.raceResults ? qsTr("Punkty serii") : qsTr("Skoki"); quiet: true; font.pointSize: Theme.caption; checkable: true; onClicked: tableRoot.jumpColumns=checked }
            SButton {
                implicitHeight: Theme.compact ? Math.max(26,Theme.bodyMetrics.height+4) : tableRoot.compact ? 28 : 40; visible: !tableRoot.compact; text: tableRoot.width < 380 ? "↗" : qsTr("Otwórz"); quiet: true; Accessible.name: qsTr("Otwórz"); hint: qsTr("Otwórz"); onClicked: if (tableRoot.table) tableRoot.table.openCurrent() }
            SButton {
                implicitHeight: Theme.compact ? Math.max(26,Theme.bodyMetrics.height+4) : tableRoot.compact ? 28 : 40;
                objectName: "table-options-" + tableRoot.ownerId
                visible: tableRoot.actions.length > 0 || (tableRoot.table && tableRoot.table.multiple)
                text: "⋯"; quiet: true; Accessible.name: qsTr("Opcje listy")
                onClicked: selectionMenu.open()
                ActionMenu {
                    id: selectionMenu
                    entries: (tableRoot.table && tableRoot.table.multiple
                              ? [{text: qsTr("Zaznacz wszystkie"), selectAll: true}]
                              : []).concat(tableRoot.actions)
                    onSelected: {
                        if (entry.selectAll) tableRoot.table.selectAll()
                        else ui.triggerAction(entry.id)
                    }
                }
            }
        }
        RowLayout {
            visible: tableRoot.searchOpen
            Layout.fillWidth: true; Layout.leftMargin: 8; Layout.rightMargin: 8; Layout.bottomMargin: 8
            SField {
                id: search; objectName: "tableSearch-"+tableRoot.ownerId
                Layout.fillWidth: true; Layout.minimumWidth: 0
                placeholderText: qsTr("Szukaj w tabeli…"); text: tableRoot.table ? tableRoot.table.filter : ""
                onTextEdited: if(tableRoot.table)tableRoot.table.filter=text
                Accessible.name: qsTr("Filtruj wiersze")
                Keys.onEscapePressed: {if(tableRoot.table)tableRoot.table.filter="";tableRoot.forceActiveFocus()}
            }
            SButton { visible: !!tableRoot.table && tableRoot.table.filter.length>0; text: "×"; quiet: true; Accessible.name: qsTr("Wyczyść filtr"); onClicked: tableRoot.table.filter="" }
        }
        Flickable {
            id: horizontal
            Layout.fillWidth: true; Layout.fillHeight: true
            contentWidth: Math.max(width, tableRoot.totalWidth)
            contentHeight: height
            flickableDirection: Flickable.HorizontalFlick
            boundsBehavior: Flickable.StopAtBounds
            clip: true
            ScrollBar.horizontal: ScrollBar {}
            Column {
                width: horizontal.contentWidth
                height: horizontal.height
                Row {
                    id: header
                    visible: tableRoot.columns > 1
                    height: visible ? (Theme.compact ? Math.max(24,Theme.captionMetrics.height+4) : tableRoot.compact ? 28 : 38) : 0
                    Repeater {
                        model: tableRoot.displayColumns
                        AbstractButton {
                            visible: index < tableRoot.visibleColumns
                            hoverEnabled: true
                            Accessible.name: (tableRoot.headers[modelData] || "")
                            ToolTip.visible: hovered || activeFocus
                            ToolTip.text: (tableRoot.headers[modelData] || "")
                            width: tableRoot.columnWidth(index); height: header.height
                            onClicked: {
                                tableRoot.descending = tableRoot.sortColumn === modelData ? !tableRoot.descending : false
                                tableRoot.sortColumn = modelData
                                tableRoot.table.sort(modelData, tableRoot.descending)
                            }
                            background: Rectangle { color: Theme.raised; border.width: 2; border.color: parent.activeFocus ? Theme.accent : "transparent" }
                            contentItem: Text { text: (tableRoot.raceResults && !tableRoot.jumpColumns ? (modelData===0 ? qsTr("Poz.") : modelData===1 ? tableRoot.headers[1] : modelData===2 ? qsTr("Punkty") : qsTr("%1 · m").arg((modelData-1)/2)) : (tableRoot.headers[modelData] || "")) + (tableRoot.sortColumn === modelData ? (tableRoot.descending ? " ↓" : " ↑") : ""); color: Theme.muted; font.family: Theme.bodyFamily; font.pointSize: Theme.caption; font.weight: Font.DemiBold; leftPadding: 12; verticalAlignment: Text.AlignVCenter; elide: Text.ElideRight }
                        }
                    }
                }
                ListView {
                    id: rows
                    objectName: "tableRows-" + tableRoot.ownerId
                    width: parent.width
                    height: Math.max(0,parent.height - header.height - (horizontal.contentWidth>horizontal.width+1 ? 14 : 0))
                    model: tableRoot.table
                    clip: true
                    boundsBehavior: Flickable.StopAtBounds
                    ScrollBar.vertical: ScrollBar { parent: horizontal; anchors.top: parent.top; anchors.bottom: parent.bottom; anchors.right: parent.right }
                    delegate: TableRow {
                        width: rows.width
                        tableView: tableRoot
                        rowIndex: index
                        rowCells: model.cells || []
                        rowSelected: !!model.selected
                        rowCurrent: !!model.current
                        rowDepth: model.depth || 0
                        rowBranch: !!model.branch
                        rowExpanded: !!model.expanded
                        statusText: model.statusText || ""
                        tint: model.tint || ""
                    }
                    Column {
                        anchors.centerIn: parent; spacing: 8; visible: rows.count===0
                        readonly property bool filtered: !!tableRoot.table && tableRoot.table.filter.length>0
                        Label { text: parent.filtered ? qsTr("Brak wyników") : qsTr("Lista jest pusta"); color: Theme.muted; font.pointSize: Theme.body }
                        SButton { objectName: "clearTableFilter-"+tableRoot.ownerId; visible: parent.filtered; text: qsTr("Wyczyść filtr"); quiet: true; onClicked: tableRoot.table.filter="" }
                    }
                }
            }
        }
        RowLayout {
            visible: !!tableRoot.table && tableRoot.table.multiple && tableRoot.table.selectedCount>0 && tableRoot.height>=300
            Layout.fillWidth: true; Layout.margins: 8
            Label { objectName: "tableSelection-"+tableRoot.ownerId; text: qsTr("Zaznaczono: %1").arg(tableRoot.table ? tableRoot.table.selectedCount : 0); color: Theme.ink; font.pointSize: Theme.caption; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight }
            SButton { text: tableRoot.width<400 ? "×" : qsTr("Wyczyść zaznaczenie"); Accessible.name: qsTr("Wyczyść zaznaczenie"); quiet: true; onClicked: tableRoot.table.clearSelection() }
        }
    }
    Connections {
        target: tableRoot.table
        function onCursorChanged() {
            if (tableRoot.followCurrent || tableRoot.activeFocus) Qt.callLater(function() {
                if (tableRoot.table && tableRoot.table.currentRow >= 0) rows.positionViewAtIndex(tableRoot.table.currentRow,ListView.Contain)
                if (tableRoot.table) {
                    var offset = 0
                    var visual=tableRoot.displayColumns.indexOf(tableRoot.table.currentColumn)
                    if(visual<0)return
                    for (var c=0;c<visual;++c) offset += tableRoot.columnWidth(c)
                    if (offset < horizontal.contentX) horizontal.contentX = offset
                    else if (offset+tableRoot.columnWidth(visual) > horizontal.contentX+horizontal.width)
                        horizontal.contentX = offset+tableRoot.columnWidth(visual)-horizontal.width
                }
            })
        }
    }
    Shortcut { sequence: StandardKey.Find; enabled: tableRoot.visible && tableRoot.activeFocus; onActivated: {tableRoot.searchOpen=true;search.forceActiveFocus();search.selectAll()} }
    Keys.onPressed: {
        if (!table) return
        if (event.key === Qt.Key_Up && !(event.modifiers & Qt.ControlModifier)) { table.moveCurrent(-1,event.modifiers); event.accepted = true }
        else if (event.key === Qt.Key_Down && !(event.modifiers & Qt.ControlModifier)) { table.moveCurrent(1,event.modifiers); event.accepted = true }
        else if (event.key === Qt.Key_Left || event.key === Qt.Key_Right) { var visual=displayColumns.indexOf(table.currentColumn);var next=Math.max(0,Math.min(visibleColumns-1,visual+(event.key === Qt.Key_Left ? -1 : 1)));table.moveColumn(displayColumns[next]-table.currentColumn); event.accepted = true }
        else if (event.key === Qt.Key_F2) { editCurrent(); event.accepted = true }
        else if (event.key === Qt.Key_Space) { table.toggleCurrent(); event.accepted = true }
        else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) { if (!ui.shortcut(ownerId,"Return")) table.openCurrent(); event.accepted = true }
        else if (event.key === Qt.Key_Delete) { event.accepted = ui.shortcut(ownerId,"Del") }
        else if (event.key === Qt.Key_Insert) { event.accepted = ui.shortcut(ownerId,"Insert") }
        else if (event.key === Qt.Key_A && event.modifiers === Qt.ControlModifier) { table.selectAll(); event.accepted = true }
        else if (event.modifiers & Qt.ControlModifier) {
            var sequence = "Ctrl+" + (event.modifiers & Qt.ShiftModifier ? "Shift+" : "") + (event.modifiers & Qt.AltModifier ? "Alt+" : "")
            sequence += event.key === Qt.Key_Up ? "Up" : event.key === Qt.Key_Down ? "Down" : String.fromCharCode(event.key)
            ui.shortcut(ownerId, sequence); event.accepted = true
        }
    }
}
