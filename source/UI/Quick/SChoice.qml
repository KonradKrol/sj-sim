import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

ComboBox {
    id: control
    property bool searchable: false
    readonly property var matches: {
        var entries = []
        var source = model
        var query = search.text.trim().toLocaleLowerCase()
        if (searchable) for (var i = 0; i < count; ++i) {
            var label = textAt(i)
            if (label.toLocaleLowerCase().indexOf(query) >= 0) entries.push({sourceIndex:i,label:label})
        }
        return entries
    }
    function chooseMatch() {
        if (choices.currentIndex < 0 || choices.currentIndex >= matches.length) return
        var index = matches[choices.currentIndex].sourceIndex
        currentIndex = index
        activated(index)
        popup.close()
        forceActiveFocus()
    }
    implicitHeight: 42
    implicitWidth: 180
    font.pointSize: Theme.body
    leftPadding: 12
    rightPadding: 32
    hoverEnabled: true
    contentItem: Text {
        text: control.displayText
        font: control.font
        color: Theme.ink
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
    indicator: Text {
        text: "⌄"; color: Theme.muted
        anchors.right: parent.right; anchors.rightMargin: 12; anchors.verticalCenter: parent.verticalCenter
        font.pointSize: Theme.body
    }
    background: Surface {
        fillColor: control.hovered ? Theme.selection : Theme.raised
        edgeColor: Theme.accent
        strokeWidth: control.activeFocus ? 2 : 0
        sweep: 12
    }
    delegate: ItemDelegate {
        width: control.popup.width
        text: control.textRole ? model[control.textRole] : modelData
        highlighted: control.highlightedIndex === index
        contentItem: Text { text: parent.text; font: control.font; color: Theme.ink; elide: Text.ElideRight; verticalAlignment: Text.AlignVCenter }
        background: Rectangle { color: parent.highlighted ? Theme.selection : Theme.surface }
    }
    Component {
        id: matchedChoice
        ItemDelegate {
            width: choices.width
            text: modelData.label
            highlighted: choices.currentIndex === index
            onClicked: { choices.currentIndex = index; control.chooseMatch() }
            contentItem: Text { text: parent.text; font: control.font; color: Theme.ink; elide: Text.ElideRight; verticalAlignment: Text.AlignVCenter }
            background: Rectangle { color: parent.highlighted ? Theme.selection : Theme.surface }
        }
    }
    popup: Popup {
        objectName: "choicePopup"
        readonly property real above: Math.max(0,control.mapToItem(Overlay.overlay,0,0).y-8)
        readonly property real below: Math.max(0,(control.Window.window ? control.Window.window.height : 600)-control.mapToItem(Overlay.overlay,0,control.height).y-8)
        readonly property real desired: Math.min(320,choices.contentHeight+(control.searchable ? search.implicitHeight+4+(control.matches.length ? 0 : emptyResult.implicitHeight+4) : 0)+8)
        y: below >= desired || below >= above ? control.height+4 : -height-4
        margins: 8
        width: control.width
        padding: 4
        implicitHeight: Math.min(desired,Math.max(above,below))
        onOpened: if (control.searchable) {
            search.clear()
            choices.currentIndex = control.currentIndex
            search.forceActiveFocus()
        }
        onClosed: if (control.searchable) search.clear()
        contentItem: ColumnLayout {
            spacing: 4
            SField {
                id: search
                objectName: "hillChoiceSearch"
                visible: control.searchable
                Layout.fillWidth: true
                placeholderText: qsTr("Szukaj skoczni…")
                Accessible.name: qsTr("Szukaj skoczni")
                onTextChanged: choices.currentIndex = control.matches.length ? 0 : -1
                Keys.onDownPressed: choices.currentIndex = Math.min(control.matches.length-1,choices.currentIndex+1)
                Keys.onUpPressed: choices.currentIndex = Math.max(0,choices.currentIndex-1)
                Keys.onReturnPressed: control.chooseMatch()
                Keys.onEnterPressed: control.chooseMatch()
                Keys.onEscapePressed: control.popup.close()
            }
            ListView {
                id: choices
                objectName: "choiceResults"
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
                implicitHeight: contentHeight
                clip: true
                model: control.popup.visible ? (control.searchable ? control.matches : control.delegateModel) : null
                currentIndex: control.searchable ? -1 : control.highlightedIndex
                delegate: control.searchable ? matchedChoice : null
                boundsBehavior: Flickable.StopAtBounds
                ScrollBar.vertical: ScrollBar {}
            }
            Label {
                id: emptyResult
                visible: control.searchable && control.matches.length === 0
                Layout.fillWidth: true
                text: qsTr("Brak skoczni")
                color: Theme.muted; font.pointSize: Theme.caption
                padding: 8
            }
        }
        background: Rectangle { color: Theme.surface; border.color: Theme.outline; radius: Theme.radius }
    }
}
