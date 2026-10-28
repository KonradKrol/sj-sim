import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

AbstractButton {
    id: choice
    property string title
    property string accessibleTitle: title
    property string symbol
    property string detail
    property string explanation
    property int power: -1
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
    padding: Theme.dp(7)
    implicitHeight: Math.max(Theme.dp(detail.length?48:35),contentItem.implicitHeight+topPadding+bottomPadding)
    Accessible.name: accessibleTitle
    Accessible.description: explanation
    Accessible.role: Accessible.RadioButton
    Accessible.checked: checked
    background: Surface {
        sweep: Theme.dp(10)
        fillColor: choice.checked ? Theme.stadium : choice.hovered ? Theme.selection : Theme.surface
        edgeColor: choice.visualFocus ? Theme.accent : choice.checked ? Theme.signal : Theme.divider
        strokeWidth: choice.checked || choice.visualFocus ? 2 : 1
        Rectangle { visible: choice.checked; width: Theme.dp(3); height: parent.height-Theme.dp(12); y: Theme.dp(6); color: Theme.signal }
    }
    contentItem: RowLayout {
        spacing: Theme.dp(7)
        Label { text: choice.symbol; color: choice.checked ? Theme.signal : Theme.accent; font.pointSize: Theme.section; font.bold: true; Accessible.ignored: true; Layout.preferredWidth: Theme.dp(20); horizontalAlignment: Text.AlignHCenter }
        ColumnLayout {
            Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: Theme.dp(1)
            Label { text: choice.title; color: choice.checked ? Theme.snow : Theme.ink; font.family: Theme.bodyFamily; font.pointSize: Theme.body; font.weight: choice.checked ? Font.Bold : Font.Medium; wrapMode: Text.WordWrap; Layout.fillWidth: true }
            Label { visible: choice.detail.length>0; text: choice.detail; color: choice.checked ? Theme.headerMuted : Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        }
        Row {
            visible: choice.power>=0
            spacing: Theme.dp(2)
            Layout.alignment: Qt.AlignVCenter
            Repeater {
                model: 3
                Rectangle { required property int index; width: Theme.dp(2); height: Theme.dp(4+index*3); y: Theme.dp(10)-height; radius: 1; color: index<choice.power ? (choice.checked ? Theme.signal : Theme.accent) : (choice.checked ? Theme.track : Theme.divider) }
            }
        }
        Label { visible: choice.checked; text: "●"; color: Theme.signal; font.pointSize: Theme.caption; Accessible.ignored: true }
    }
    ToolTip.visible: !down && (helpRequested || hovered && !hintSuppressed)
    ToolTip.delay: helpRequested ? 0 : 700
    ToolTip.text: explanation
}
