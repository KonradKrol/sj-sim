import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: library
    objectName: "athleteLibrary"
    property var backend
    property string query: ""
    property string pack: ""
    property string selectedId: ""
    signal activated()
    ChangeFeedback { id: packFeedback; target: library; trigger: library.backend.packDirectory; duration: Theme.motion*2 }
    padding: 1
    background: Rectangle { color: Theme.surface; radius: Theme.radius; border.color: rows.activeFocus ? Theme.accent : Theme.divider }
    function sync(reset) {
        var newPack=backend.packDirectory
        if(pack!==newPack){pack=newPack;query="";reset=true}
        var source=backend.profiles, next=[], needle=query.trim().toLocaleLowerCase()
        for(var i=0;i<source.length;++i){
            var p=source[i]
            if(needle.length && (p.label+" "+p.country).toLocaleLowerCase().indexOf(needle)<0)continue
            next.push({profileId:p.id,profileIndex:p.index,label:p.label,country:p.country,flag:p.flag,overall:p.overall})
        }
        var prefix=0,suffix=0
        while(prefix<entries.count && prefix<next.length && entries.get(prefix).profileId===next[prefix].profileId)++prefix
        while(suffix<entries.count-prefix && suffix<next.length-prefix && entries.get(entries.count-1-suffix).profileId===next[next.length-1-suffix].profileId)++suffix
        var removeCount=entries.count-prefix-suffix
        if(removeCount)entries.remove(prefix,removeCount)
        for(var n=prefix;n<next.length-suffix;++n)entries.insert(n,next[n])
        var current=-1
        for(var r=0;r<next.length;++r){
            var old=entries.get(r), value=next[r]
            if(old.profileIndex!==value.profileIndex || old.label!==value.label || old.country!==value.country || old.flag!==value.flag || old.overall!==value.overall)entries.set(r,value)
            if(value.profileIndex===backend.selectedIndex)current=r
        }
        rows.currentIndex=current
        var id=backend.profile.id || "", reveal=selectedId!==id
        selectedId=id
        if(reset)rows.positionViewAtBeginning()
        else if(reveal && current>=0)Qt.callLater(function(){if(rows.currentIndex>=0)rows.positionViewAtIndex(rows.currentIndex,ListView.Contain)})
    }
    function choose(index,openEditor) {
        if(index<0 || index>=entries.count)return
        var target=entries.get(index).profileIndex
        rows.forceActiveFocus()
        backend.select(target)
        if(openEditor)activated()
    }
    onQueryChanged: sync(true)
    Component.onCompleted: sync(true)
    Connections { target: library.backend; function onUiChanged(){library.sync(false)} }
    ListModel { id: entries }
    contentItem: ColumnLayout {
        spacing: 0
        RowLayout {
            Layout.fillWidth: true; Layout.margins: Theme.dp(8)
            Label { text: qsTr("Skoczkowie · %1").arg(library.backend.profiles.length); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true }
            SButton { objectName: "athleteRemove"; text: qsTr("Usuń"); quiet: true; foreground: Theme.danger; enabled: library.backend.selectedIndex>=0; Accessible.name: qsTr("Usuń wybranego skoczka"); hint: qsTr("Delete · możesz cofnąć usunięcie"); onClicked: {rows.forceActiveFocus();library.backend.remove()} }
        }
        SField {
            objectName: "athleteListSearch"
            Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.leftMargin: Theme.dp(8); Layout.rightMargin: Theme.dp(8); Layout.bottomMargin: Theme.dp(8)
            text: library.query; placeholderText: qsTr("Szukaj skoczka lub kraju…"); Accessible.name: placeholderText
            onTextEdited: library.query=text
            Keys.onEscapePressed: library.query=""
        }
        ListView {
            id: rows
            opacity: 0.65+packFeedback.progress*0.35
            objectName: "athleteList"
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
            model: entries; clip: true; activeFocusOnTab: true
            boundsBehavior: Flickable.StopAtBounds
            keyNavigationEnabled: false
            Accessible.role: Accessible.List; Accessible.name: qsTr("Skoczkowie")
            ScrollBar.vertical: ScrollBar {}
            Keys.onUpPressed: library.choose(currentIndex<0 ? count-1 : Math.max(0,currentIndex-1))
            Keys.onDownPressed: library.choose(Math.min(count-1,currentIndex+1))
            Keys.onPressed: {
                if(event.key===Qt.Key_Home)library.choose(0)
                else if(event.key===Qt.Key_End)library.choose(count-1)
                else event.accepted=false
            }
            Keys.onDeletePressed: {if(library.backend.selectedIndex>=0)library.backend.remove();event.accepted=true}
            Keys.onReturnPressed: if(currentIndex>=0)library.activated()
            Keys.onEnterPressed: if(currentIndex>=0)library.activated()
            delegate: AbstractButton {
                id: row
                required property int index
                required property string profileId
                required property int profileIndex
                required property string label
                required property string country
                required property string flag
                required property int overall
                objectName: "athleteListRow-"+profileIndex
                width: rows.width
                implicitHeight: Theme.compact ? Math.max(24,Theme.bodyMetrics.height+4) : Math.max(40,Theme.body*2.7)
                padding: Theme.dp(9); rightPadding: Theme.dp(14)
                checked: library.backend.selectedIndex===profileIndex
                hoverEnabled: true; focusPolicy: Qt.NoFocus
                Accessible.role: Accessible.ListItem; Accessible.name: qsTr("%1, %2, OVR %3").arg(label).arg(country).arg(overall); Accessible.selected: checked
                background: Rectangle {
                    color: row.checked ? Theme.selection : row.hovered ? Theme.raised : Theme.surface
                    Behavior on color { ColorAnimation { duration: Theme.motion } }
                    Rectangle { width: 3; height: parent.height; visible: row.checked; color: Theme.accent }
                    Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: Theme.divider }
                }
                contentItem: RowLayout {
                    spacing: Theme.dp(8)
                    Image { source: row.flag; sourceSize.width: 32; sourceSize.height: 24; Layout.preferredWidth: Theme.dp(26); Layout.preferredHeight: Theme.dp(19); fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
                    Label { text: row.label; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight; color: Theme.ink; font.family: Theme.bodyFamily; font.pointSize: Theme.body }
                    Label { text: row.overall; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption }
                }
                onClicked: library.choose(index,true)
            }
            EmptyState { anchors.fill: parent; visible: rows.count===0; compact: true; title: library.query.length ? qsTr("Brak wyników") : qsTr("Brak skoczków"); description: library.query.length ? qsTr("Zmień nazwisko lub kraj.") : qsTr("Dodaj skoczka do paczki."); actionText: library.query.length ? qsTr("Wyczyść filtr") : ""; onTriggered: library.query="" }
        }
    }
}
