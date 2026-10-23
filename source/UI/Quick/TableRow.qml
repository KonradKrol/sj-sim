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
    Accessible.name: (rowCells || []).map(function(cell) { return cell.text }).join(", ") + (statusText ? ", " + statusText : "")
    Accessible.selected: rowSelected
    height: Theme.compact ? Math.max(24,Theme.bodyMetrics.height+4) : Math.max(tableView.compact ? 32 : 40, Theme.body * (tableView.compact ? 2.3 : 2.7))
    color: rowSelected ? Theme.selection : mouse.containsMouse ? Theme.raised : tintColor()
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
                width: row.tableView.columnWidth(index)
                height: row.height
                Row {
                    anchors.fill: parent
                    anchors.leftMargin: (Theme.compact ? 6 : 12) + (cell.columnIndex === 0 ? row.rowDepth * 16 + (row.rowBranch ? 24 : 0) : 0)
                    anchors.rightMargin: Theme.compact ? 4 : 10
                    spacing: Theme.compact ? 4 : 8
                    Image { visible: !!cell.fields.icon; source: cell.fields.icon || ""; sourceSize.width: 32; sourceSize.height: 24; width: visible ? 30 : 0; height: 24; anchors.verticalCenter: parent.verticalCenter; fillMode: Image.PreserveAspectFit }
                    Text { visible: !cell.fields.checkable; text: cell.fields.text || ""; color: Theme.ink; font.pointSize: Theme.body; font.family: /^[-+−]?\d+([.,]\d+)?\s*(m|pkt|%|m\/s)?$/.test(text) ? Theme.mono : Theme.bodyFamily; width: parent.width - (cell.fields.icon ? 38 : 0); height: parent.height; horizontalAlignment: cell.columnIndex > 0 && font.family === Theme.mono ? Text.AlignRight : Text.AlignLeft; verticalAlignment: Text.AlignVCenter; elide: Text.ElideRight }
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
            row.tableView.table.select(row.rowIndex,column,mouse.modifiers,false)
            var cell = row.rowCells[column]
            if (cell && cell.checkable) row.tableView.table.edit(row.rowIndex,column,!cell.checked)
        }
        ToolTip.visible: containsMouse
        ToolTip.delay: 600
        ToolTip.text: {
            var column = row.tableView.columnAt(mouseX)
            var data = row.rowCells && row.rowCells[column]
            return (data ? data.tip || data.text || "" : "") + (row.statusText ? " · " + row.statusText : "")
        }
        onDoubleClicked: {
            var column = row.tableView.columnAt(mouse.x)
            var cell = row.rowCells[column]
            if (cell && cell.editable) {
                row.editCell(column)
            } else row.tableView.table.select(row.rowIndex,column,mouse.modifiers,true)
        }
    }
    SButton {
        visible: row.rowBranch
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
