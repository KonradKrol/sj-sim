import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: library
    objectName: "hillLibrary"
    property var backend
    property string query: ""
    property int pack: -1
    signal activated()
    ListModel { id: entries }
    function sync() {
        if(pack!==backend.packIndex){pack=backend.packIndex;query=""}
        var next=backend.profiles.filter(function(p){return (p.label+" "+p.country).toLocaleLowerCase().indexOf(query.toLocaleLowerCase())>=0})
        var prefix=0,suffix=0
        while(prefix<entries.count && prefix<next.length && entries.get(prefix).profileId===next[prefix].id)++prefix
        while(suffix<entries.count-prefix && suffix<next.length-prefix && entries.get(entries.count-1-suffix).profileId===next[next.length-1-suffix].id)++suffix
        if(entries.count-prefix-suffix)entries.remove(prefix,entries.count-prefix-suffix)
        for(var i=prefix;i<next.length-suffix;++i)entries.insert(i,{profileId:next[i].id,profileIndex:next[i].index,label:next[i].label,country:next[i].country,flag:next[i].flag,hs:next[i].hs})
        var selectedRow=-1
        for(var r=0;r<next.length;++r){entries.set(r,{profileId:next[r].id,profileIndex:next[r].index,label:next[r].label,country:next[r].country,flag:next[r].flag,hs:next[r].hs});if(next[r].index===backend.selectedIndex)selectedRow=r}
        rows.currentIndex=selectedRow
    }
    onQueryChanged: sync()
    Component.onCompleted: sync()
    function choose(index) {if(index<0 || index>=entries.count)return;backend.select(entries.get(index).profileIndex);rows.positionViewAtIndex(index,ListView.Contain)}
    Connections { target: library.backend; function onUiChanged(){library.sync()} }
    spacing: Theme.dp(6)
    Label { text: qsTr("Skocznie · %1").arg(library.backend.profiles.length); color: Theme.muted; font.pointSize: Theme.caption }
    SField { Layout.fillWidth: true; text: library.query; placeholderText: qsTr("Szukaj skoczni lub kraju…"); Accessible.name: placeholderText; onTextEdited: library.query=text }
    ListView {
        id: rows
        objectName: "hillList"
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
        clip: true; boundsBehavior: Flickable.StopAtBounds; activeFocusOnTab: true
        model: entries
        ScrollBar.vertical: ScrollBar {}
        Accessible.role: Accessible.List; Accessible.name: qsTr("Skocznie")
        keyNavigationEnabled: false
        Keys.onUpPressed: library.choose(Math.max(0,currentIndex-1))
        Keys.onDownPressed: library.choose(Math.min(count-1,currentIndex+1))
        Keys.onPressed: {if(event.key===Qt.Key_Home)library.choose(0);else if(event.key===Qt.Key_End)library.choose(count-1);else event.accepted=false}
        Keys.onReturnPressed: if(currentItem)currentItem.clicked()
        delegate: AbstractButton {
            id: row
            required property string profileId
            required property int profileIndex
            required property string label
            required property string country
            required property string flag
            required property real hs
            required property int index
            objectName: "hillListRow-"+profileIndex
            width: rows.width; implicitHeight: Math.max(Theme.dp(42),contentItem.implicitHeight+Theme.dp(14)); padding: Theme.dp(7)
            checked: library.backend.selectedIndex===profileIndex; hoverEnabled: true
            Accessible.role: Accessible.ListItem; Accessible.selected: checked; Accessible.name: label+", "+country+", HS"+hs
            background: Rectangle { color: row.checked?Theme.selection:row.hovered?Theme.raised:Theme.surface; radius: Theme.dp(5); border.width: row.activeFocus?2:0; border.color: Theme.accent; Behavior on color { ColorAnimation { duration: Theme.motion } } }
            contentItem: RowLayout {
                spacing: Theme.dp(7)
                Image { objectName: "hillListFlag-"+row.profileIndex; source: row.flag; sourceSize.width: 48; sourceSize.height: 32; Layout.preferredWidth: Theme.dp(24); Layout.preferredHeight: Theme.dp(16); fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
                Label { text: row.label; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight; color: Theme.ink; font.pointSize: Theme.body }
                Label { objectName: "hillListHs-"+row.profileIndex; text: "HS"+row.hs; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption }
            }
            onClicked: {library.backend.select(profileIndex);library.activated()}
        }
        EmptyState { anchors.fill: parent; visible: rows.count===0; compact: true; title: library.query.length?qsTr("Brak wyników"):qsTr("Brak skoczni"); description: library.query.length?qsTr("Zmień nazwę lub kraj."):qsTr("Dodaj skocznię lub skopiuj ją z klasycznego modelu.") }
    }
}
