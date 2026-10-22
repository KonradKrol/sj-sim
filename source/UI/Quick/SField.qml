import QtQuick 2.15
import QtQuick.Controls 2.15

TextField {
    id: field
    implicitHeight: Math.max(Theme.controlHeight, contentHeight + (Theme.compact ? 10 : 18))
    color: Theme.ink
    placeholderTextColor: Theme.muted
    selectedTextColor: Theme.ink
    selectionColor: Theme.selection
    font.pointSize: Theme.body
    selectByMouse: true
    leftPadding: Theme.compact ? 9 : 12
    rightPadding: Theme.compact ? 9 : 12
    background: Surface {
        fillColor: Theme.raised
        corner: 6; sweep: 12
        strokeWidth: field.activeFocus ? 2 : 0
        edgeColor: Theme.accent
    }
}
