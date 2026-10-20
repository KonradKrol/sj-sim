import QtQuick 2.15
import QtQuick.Controls 2.15

ComboBox {
    id:choice
    property var theme
    implicitHeight:40
    leftPadding:12; rightPadding:32
    contentItem:Text { text:choice.displayText; color:theme.ink; font.pixelSize:14; verticalAlignment:Text.AlignVCenter; elide:Text.ElideRight }
    indicator:Text { x:choice.width-width-12; anchors.verticalCenter:parent.verticalCenter; text:"⌄"; color:theme.muted; font.pixelSize:18 }
    background:Rectangle { radius:7; color:theme.dark ? "#142031" : "#f7f9fc"; border.color:choice.activeFocus ? "#639aff" : theme.dark ? "#35465d" : "#dbe4ef" }
    delegate:ItemDelegate {
        width:choice.width; height:40
        contentItem:Text { text:modelData; color:theme.ink; font.pixelSize:14; verticalAlignment:Text.AlignVCenter; elide:Text.ElideRight }
        background:Rectangle { color:parent.highlighted ? (theme.dark ? "#304d72" : "#dceaff") : theme.panel }
        highlighted:choice.highlightedIndex===index
    }
    popup:Popup {
        y:choice.height+4; width:choice.width
        height:Math.min(320,Math.max(48,choices.contentHeight+12))
        padding:6
        contentItem:ListView { id:choices; clip:true; implicitHeight:contentHeight; model:choice.popup.visible ? choice.delegateModel : null; currentIndex:choice.highlightedIndex; ScrollBar.vertical:ScrollBar {} }
        background:Rectangle { color:theme.panel; radius:8; border.color:theme.dark ? "#35465d" : "#dbe4ef" }
    }
}
