import QtQuick 2.15
import QtQuick.Controls 2.15

CheckBox {
    id: control
    nextCheckState: function() { return checkState === Qt.Checked ? Qt.Unchecked : Qt.Checked }
    font.pointSize: Theme.body
    implicitHeight: Math.max(Theme.compact ? 26 : 40, label.implicitHeight + (Theme.compact ? 2 : 12))
    padding: Theme.compact ? 2 : 6
    indicator: Rectangle {
        width: Theme.compact ? 18 : 22; height: width; radius: Theme.checkRadius
        y: (control.height - height) / 2
        color: control.checkState !== Qt.Unchecked ? Theme.accent : Theme.raised
        border.color: control.activeFocus ? Theme.accent : Theme.outline
        border.width: control.activeFocus ? Theme.focusBorder : Math.max(1,Theme.controlBorder)
        Text { anchors.centerIn: parent; text: control.checkState === Qt.PartiallyChecked ? "−" : control.checked ? "✓" : ""; color: Theme.accentText; font.pointSize: Theme.body }
    }
    contentItem: Text {
        id: label
        text: control.text
        font: control.font
        color: Theme.ink
        leftPadding: Theme.compact ? 24 : 34
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
    }
}
