import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

AbstractButton {
    id: choice
    property string title
    property string symbol
    property string detail
    property string explanation
    property bool helpRequested: false
    property bool hintSuppressed: false
    hoverEnabled: true
    onHoveredChanged: if(!hovered){helpRequested=false;hintSuppressed=false}
    onPressed: {hintSuppressed=true;helpRequested=false}
    onActiveFocusChanged: if(!activeFocus)helpRequested=false
    Keys.onPressed: {
        if(event.key===Qt.Key_F1){helpRequested=true;event.accepted=true}
        else event.accepted=false
    }
    padding: Theme.dp(10)
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    Accessible.name: title
    Accessible.description: explanation
    Accessible.role: Accessible.RadioButton
    Accessible.checked: checked
    background: Surface {
        sweep: Theme.dp(18)
        fillColor: choice.checked ? Theme.stadium : choice.hovered ? Theme.selection : Theme.surface
        edgeColor: choice.activeFocus ? Theme.accent : choice.checked ? Theme.stadium : Theme.divider
        strokeWidth: choice.activeFocus ? 2 : 1
    }
    contentItem: ColumnLayout {
        spacing: Theme.dp(3)
        RowLayout {
            Layout.fillWidth: true
            Label { text: choice.symbol; color: choice.checked ? Theme.signal : Theme.accent; font.pointSize: Theme.heading; font.bold: true; Accessible.ignored: true }
            Item { Layout.fillWidth: true }
            Label { text: choice.checked ? "●" : "○"; color: choice.checked ? Theme.signal : Theme.muted; font.pointSize: Theme.caption; Accessible.ignored: true }
        }
        Label { text: choice.title; color: choice.checked ? Theme.snow : Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.bold: true; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        Label { text: choice.detail; color: choice.checked ? Theme.headerMuted : Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
    }
    ToolTip.visible: !down && (helpRequested || hovered && !hintSuppressed)
    ToolTip.delay: helpRequested ? 0 : 700
    ToolTip.text: explanation
}
