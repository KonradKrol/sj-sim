import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: row
    property var tableView
    property int rowIndex
    property var rowCells
    property bool rowSelected
    property bool rowCurrent
    property int rowDepth
    property bool rowBranch
    property bool rowExpanded
    property int advanceState: -1
    property int qualificationState: -1
    property string qualificationTarget: ""
    property bool luckyLoser: false
    property bool latestJump: false
    readonly property bool teamHeader: tableView.teamResults && rowDepth===0
    readonly property bool teamMember: tableView.teamResults && rowDepth>0
    readonly property bool sureAdvanced: advanceState===0 || qualificationState===0
    readonly property bool eliminated: advanceState===1 || qualificationState===1
    property string statusText: ""
    property string tint: ""
    function editCell(column) {
        var cell = row.rowCells[column]
        if (!cell || !cell.editable) return false
        editor.columnIndex = column
        editor.text = cell.value === undefined || cell.value === null ? cell.text : cell.value.toString()
        editor.visible = true
        editor.forceActiveFocus(); editor.selectAll()
        return true
    }
    function tintColor() {
        if (!tint || tint === "#ffffff" || tint === "#fefefe") return Theme.surface
        var color = Qt.colorEqual(tint,"#c8c8c8") ? Theme.raised : tint
        if (!Theme.dark) return color
        var r = parseInt(tint.substr(1,2),16), g = parseInt(tint.substr(3,2),16), b = parseInt(tint.substr(5,2),16)
        var high = Math.max(r,g,b), low = Math.min(r,g,b)
        if (high - low < 10) return Theme.raised
        return Qt.rgba(.09+(r-low)/(high-low)*.08,.14+(g-low)/(high-low)*.08,.17+(b-low)/(high-low)*.08,1)
    }
    Accessible.role: Accessible.ListItem
    Accessible.name: (rowCells || []).map(function(cell) { return cell.text }).join(", ") + (statusText ? ", " + statusText : "") + (latestJump ? ", " + qsTr("Ostatnio oddany skok") : "")
    Accessible.selected: rowSelected
    height: Theme.compact ? Math.max(24,Theme.bodyMetrics.height+4) : Math.max(tableView.compact ? 32 : 40, Theme.body * (tableView.compact ? 2.3 : 2.7))
    color: eliminated ? Theme.raised : sureAdvanced ? Theme.advancementSurface : rowSelected ? Theme.selection : mouse.containsMouse ? Theme.raised : teamHeader ? Theme.raised : teamMember ? Theme.surface : tintColor()
    border.width: latestJump ? 1 : 0
    border.color: Theme.accent
    Rectangle { objectName: "latestJumpIndicator-"+row.rowIndex; anchors.right: parent.right; anchors.top: parent.top; anchors.bottom: parent.bottom; width: Math.max(2,Theme.dp(4)); color: Theme.accent; visible: row.latestJump }
    Rectangle { anchors.left: parent.left; anchors.top: parent.top; anchors.bottom: parent.bottom; width: 3; color: Theme.accent; visible: row.rowCurrent }
    Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: Theme.divider }
    Row {
        Repeater {
            model: row.tableView.displayColumns
            Item {
                id: cell
                visible: index < row.tableView.visibleColumns
                property var fields: row.rowCells[row.tableView.teamResults && modelData===1 && row.rowDepth>0 ? 2 : modelData] || ({})
                property int columnIndex: modelData
                readonly property bool qualificationBadge: (row.qualificationState===0 || row.luckyLoser) && cell.columnIndex===1 && (!row.tableView.teamResults || row.rowDepth===0)
                width: row.tableView.columnWidth(index)
                height: row.height
                Row {
                    anchors.fill: parent
                    anchors.leftMargin: (Theme.compact ? 6 : 12) + (cell.columnIndex === 0 ? (row.rowBranch ? 24 : 0) : row.teamMember && cell.columnIndex===1 ? 20 : 0)
                    anchors.rightMargin: Theme.compact ? 4 : 10
                    spacing: Theme.compact ? 4 : 8
                    Image { visible: !!cell.fields.icon; source: cell.fields.icon || ""; sourceSize.width: 32; sourceSize.height: 24; width: visible ? 30 : 0; height: 24; anchors.verticalCenter: parent.verticalCenter; fillMode: Image.PreserveAspectFit }
                    Text { visible: !cell.fields.checkable; text: cell.fields.text || ""; color: row.eliminated ? Theme.muted : Theme.ink; font.pointSize: row.teamMember ? Theme.caption : Theme.body; font.weight: row.teamHeader ? Font.DemiBold : Font.Normal; font.family: /^[-+−]?\d+([.,]\d+)?\s*(m|pkt|%|m\/s)?$/.test(text) ? Theme.mono : Theme.bodyFamily; width: parent.width - (cell.fields.icon ? 38 : 0) - (cell.qualificationBadge ? 46 : 0); height: parent.height; horizontalAlignment: cell.columnIndex > 0 && font.family === Theme.mono ? Text.AlignRight : Text.AlignLeft; verticalAlignment: Text.AlignVCenter; elide: Text.ElideRight }
                    Label { objectName: cell.qualificationBadge ? "qualificationBadge-"+row.rowIndex : ""; visible: cell.qualificationBadge; text: row.luckyLoser ? (row.qualificationState===0 ? "LL Q" : "LL") : "Q"; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.body; font.weight: Font.DemiBold; width: 38; height: parent.height; verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; Accessible.name: row.luckyLoser ? qsTr("Aktualne miejsce lucky losera") : qsTr("Awans do konkursu"); ToolTip.visible: badgeHover.containsMouse; ToolTip.text: row.luckyLoser ? qsTr("Aktualne miejsce lucky losera") : row.qualificationTarget.length ? qsTr("Awans do konkursu · %1").arg(row.qualificationTarget) : qsTr("Awans do konkursu"); MouseArea { id: badgeHover; anchors.fill: parent; hoverEnabled: true; acceptedButtons: Qt.NoButton } }
                    Text { visible: !!cell.fields.checkable; text: cell.fields.checked ? "☑" : "☐"; color: Theme.accent; font.pointSize: Theme.body; height: parent.height; verticalAlignment: Text.AlignVCenter }
                }
            }
        }
    }
    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            row.tableView.forceActiveFocus()
            var column = row.tableView.columnAt(mouse.x)
            row.tableView.table.select(row.rowIndex,column,row.tableView.plain ? 0 : mouse.modifiers,false)
            var cell = row.rowCells[column]
            if (cell && cell.checkable) row.tableView.table.edit(row.rowIndex,column,!cell.checked)
        }
        ToolTip.visible: containsMouse
        ToolTip.delay: 600
        ToolTip.text: {
            var column = row.tableView.columnAt(mouseX)
            var data = row.rowCells && row.rowCells[column]
            return (data ? data.tip || data.text || "" : "") + (row.statusText ? " · " + row.statusText : "") + (row.latestJump ? " · " + qsTr("Ostatnio oddany skok") : "")
        }
        onDoubleClicked: {
            var column = row.tableView.columnAt(mouse.x)
            var cell = row.rowCells[column]
            if (cell && cell.editable) {
                row.editCell(column)
            } else row.tableView.table.select(row.rowIndex,column,row.tableView.plain ? 0 : mouse.modifiers,true)
        }
    }
    SButton {
        visible: row.rowBranch && !row.tableView.teamResults
        x: row.rowDepth * 16 + 3; y: 2
        width: 28; height: row.height - 4
        text: row.rowExpanded ? "−" : "+"
        quiet: true
        Accessible.name: row.rowExpanded ? qsTr("Zwiń grupę") : qsTr("Rozwiń grupę")
        onClicked: row.tableView.table.toggle(row.rowIndex)
    }
    SField {
        id: editor
        property int columnIndex: 0
        visible: false
        anchors.fill: parent
        onEditingFinished: {
            if (visible) { row.tableView.table.edit(row.rowIndex,columnIndex,text); visible = false; focus = false; row.tableView.forceActiveFocus() }
        }
        Keys.onEscapePressed: { visible = false; focus = false; row.tableView.forceActiveFocus() }
    }
}
