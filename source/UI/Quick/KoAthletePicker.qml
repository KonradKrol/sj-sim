import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Popup {
    id: picker
    objectName: "koAthletePicker"
    property var fields: ({})
    property string groupId: ""
    property int slot: -1
    signal chosen(string groupId, int slot, int athleteIndex)
    readonly property var matches: {
        var out=[]
        if (!visible) return out
        var names=fields.names || []
        var words=search.text.trim().toLocaleLowerCase().split(/\s+/)
        for (var i=0;i<names.length;++i) {
            var name=names[i].toLocaleLowerCase()
            if (words.every(function(word){return name.indexOf(word)>=0})) out.push(i)
        }
        return out
    }
    function openFor(row) {
        groupId=row.groupId;slot=row.slot
        parent=Overlay.overlay
        var point=row.mapToItem(parent,0,0)
        width=Math.min(Math.max(row.width,Theme.dp(280)),parent.width-16)
        x=Math.max(8,Math.min(point.x,parent.width-width-8))
        y=point.y+row.height+4
        if (y+implicitHeight>parent.height-8) y=Math.max(8,point.y-implicitHeight-4)
        open()
    }
    function choose(index) {
        if (index<0 || index>=matches.length) return
        var athleteIndex=matches[index]
        close()
        chosen(groupId,slot,athleteIndex)
    }
    padding: 6
    margins: 8
    implicitHeight: Math.min(Theme.dp(320),parent ? parent.height-16 : Theme.dp(320))
    onOpened: { search.clear(); results.currentIndex=0; search.forceActiveFocus() }
    contentItem: ColumnLayout {
        spacing: 6
        SField {
            id: search
            objectName: "koAthleteSearch"
            Layout.fillWidth: true
            placeholderText: qsTr("Szukaj zawodnika…")
            Accessible.name: qsTr("Szukaj zawodnika")
            onTextChanged: results.currentIndex=picker.matches.length ? 0 : -1
            Keys.onDownPressed: results.currentIndex=Math.min(picker.matches.length-1,results.currentIndex+1)
            Keys.onUpPressed: results.currentIndex=Math.max(0,results.currentIndex-1)
            Keys.onReturnPressed: picker.choose(results.currentIndex)
            Keys.onEnterPressed: picker.choose(results.currentIndex)
            Keys.onEscapePressed: picker.close()
        }
        ListView {
            id: results
            objectName: "koAthleteMatches"
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
            clip: true; boundsBehavior: Flickable.StopAtBounds
            model: picker.matches
            ScrollBar.vertical: ScrollBar {}
            delegate: ItemDelegate {
                id: option
                width: results.width
                highlighted: results.currentIndex===index
                onClicked: picker.choose(index)
                contentItem: RowLayout {
                    Image { source: (picker.fields.flags || [])[modelData] || ""; sourceSize.width: 32; sourceSize.height: 24; Layout.preferredWidth: 24; Layout.preferredHeight: 18; fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
                    Label { text: (picker.fields.names || [])[modelData] || ""; color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; elide: Text.ElideRight }
                }
                background: Rectangle { color: option.highlighted ? Theme.selection : Theme.surface }
            }
        }
        Label { visible: !picker.matches.length; text: qsTr("Brak zawodników"); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true }
    }
    background: Rectangle { color: Theme.surface; border.color: Theme.outline; radius: Theme.radius }
}
