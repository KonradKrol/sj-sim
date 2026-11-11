import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQml.Models 2.15
import QtQuick.Window 2.15

Menu {
    enter: PopupFade {}
    exit: PopupFade { leaving: true }
    id: menu
    objectName: "actionMenu"
    property var entries: []
    signal selected(var entry)
    readonly property real hostHeight: contentItem && contentItem.Window.window ? contentItem.Window.window.height : 600
    readonly property real hostWidth: contentItem && contentItem.Window.window ? contentItem.Window.window.width : 600
    width: Math.min(Math.max(120,hostWidth-16),Math.max(220,Math.min(420,parent ? parent.width : 320)))
    height: Math.min(implicitHeight,Math.max(80,hostHeight-16))
    margins: 8
    background: Rectangle { color: Theme.surface; border.color: Theme.outline; border.width: Math.max(1,Theme.controlBorder); radius: Theme.radius }
    Instantiator {
        model: menu.entries
        delegate: MenuItem {
            objectName: "menu-entry-" + index
            text: modelData.text || ""
            enabled: modelData.enabled !== false && !modelData.separator
            implicitHeight: modelData.separator ? 9 : Math.max(40,contentItem.implicitHeight + topPadding + bottomPadding)
            contentItem: Row {
                spacing: Theme.dp(9)
                Text { visible: !!modelData.icon; width: visible ? Theme.dp(18) : 0; text: modelData.icon || ""; color: modelData.danger ? Theme.danger : Theme.muted; font.pointSize: Theme.body; verticalAlignment: Text.AlignVCenter }
                Text { text: modelData.text || ""; width: Math.max(0,parent.width-(modelData.icon ? Theme.dp(27) : 0)); color: modelData.danger ? Theme.danger : Theme.ink; font.pointSize: Theme.body; elide: Text.ElideRight; verticalAlignment: Text.AlignVCenter }
            }
            background: Rectangle { color: modelData.separator ? Theme.divider : parent.highlighted ? Theme.selection : Theme.surface }
            onTriggered: menu.selected(modelData)
        }
        onObjectAdded: menu.insertItem(index,object)
        onObjectRemoved: menu.removeItem(object)
    }
}
