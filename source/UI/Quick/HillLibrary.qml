import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

ColumnLayout {
    id: library
    objectName: "hillLibrary"
    property bool season: false
    property var loadViewState: null
    property var saveViewState: null
    property bool stateReady: false
    property bool viewportRestorePending: false
    property real viewportRestoreY: 0
    function restoreViewport() {
        if(!viewportRestorePending || rows.width<=0 || rows.height<=0 || (entries.count>0 && rows.contentHeight<=0))return
        rows.forceLayout()
        var target=Math.max(rows.originY,Math.min(rows.originY+Math.max(0,rows.contentHeight-rows.height),viewportRestoreY))
        viewportRestorePending=false
        rows.contentY=target
    }
    function saveState() {
        if(!Window.window || !library.visible || rows.width<=0 || rows.height<=0 || !saveViewState || !stateReady || viewportRestorePending || (season && (!backend.available || backend.hill!==true)))return
        saveViewState({query:query,contentY:rows.contentY})
    }
    property var backend
    property string query: ""
    property int pack: -1
    signal activated()
    ListModel { id: entries }
    function sync() {
        if(season && (!backend.available || !backend.hill))return;
        if(pack!==backend.packIndex){pack=backend.packIndex;query=""}
        var next=backend.profiles.filter(function(p){return typeof p.hs === "number" && isFinite(p.hs) && (p.label+" "+p.country).toLocaleLowerCase().indexOf(query.toLocaleLowerCase())>=0})
        var prefix=0,suffix=0
        while(prefix<entries.count && prefix<next.length && entries.get(prefix).profileId===next[prefix].id)++prefix
        while(suffix<entries.count-prefix && suffix<next.length-prefix && entries.get(entries.count-1-suffix).profileId===next[next.length-1-suffix].id)++suffix
        if(entries.count-prefix-suffix)entries.remove(prefix,entries.count-prefix-suffix)
        for(var i=prefix;i<next.length-suffix;++i)entries.insert(i,{profileId:next[i].id,profileIndex:next[i].index,label:next[i].label,country:next[i].country,flag:next[i].flag,hs:next[i].hs})
        var selectedRow=-1
        for(var r=0;r<next.length;++r){var old=entries.get(r), value=next[r];if(old.profileIndex!==value.index || old.label!==value.label || old.country!==value.country || old.flag!==value.flag || old.hs!==value.hs)entries.set(r,{profileId:value.id,profileIndex:value.index,label:value.label,country:value.country,flag:value.flag,hs:value.hs});if(next[r].index===backend.selectedIndex)selectedRow=r}
        rows.currentIndex=selectedRow
    }
    onQueryChanged: {sync();saveState()}
    Component.onCompleted: Qt.callLater(initializeState)
    function initializeState() {
        var state=loadViewState ? loadViewState() : ({})
        sync()
        if(state.query!==undefined)query=state.query
        stateReady=true
        if(state.contentY!==undefined){viewportRestoreY=state.contentY;viewportRestorePending=true;Qt.callLater(restoreViewport)}
    }

    function choose(index) {if(index<0 || index>=entries.count)return;backend.select(entries.get(index).profileIndex);rows.forceActiveFocus();rows.positionViewAtIndex(index,ListView.Contain)}
    Connections { target: library.backend; function onUiChanged(){library.sync()} }
    spacing: Theme.dp(6)
    RowLayout {
        Layout.fillWidth: true
        Label { text: qsTr("Skocznie · %1").arg(library.backend.profiles.length); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true }
        SortCountryButton { visible: !library.season; enabled: library.backend.profiles.length>1; onConfirmed: library.backend.sortByCountry() }
    }
    SField { Layout.fillWidth: true; text: library.query; placeholderText: qsTr("Szukaj skoczni lub kraju…"); Accessible.name: placeholderText; onTextEdited: library.query=text }
    ListView {
        id: rows
        onContentYChanged: library.saveState()
        onHeightChanged: Qt.callLater(library.restoreViewport)
        onWidthChanged: Qt.callLater(library.restoreViewport)
        onContentHeightChanged: Qt.callLater(library.restoreViewport)
        onDraggingChanged: if(dragging)library.viewportRestorePending=false
        property int reorderTarget: -1
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
            width: rows.width; implicitHeight: Math.max(Theme.dp(42),contentItem.implicitHeight+Theme.dp(14)); padding: Theme.dp(7); rightPadding: library.season?Theme.dp(7):Theme.dp(50)
            checked: library.backend.selectedIndex===profileIndex; hoverEnabled: true
            Accessible.role: Accessible.ListItem; Accessible.selected: checked; Accessible.name: label+", "+country+", HS"+hs
            background: Rectangle { color: row.checked?Theme.selection:row.hovered?Theme.raised:Theme.surface; radius: Theme.dp(5); border.width: row.activeFocus?2:0; border.color: Theme.accent; Behavior on color { ColorAnimation { duration: Theme.motion } } }
            contentItem: RowLayout {
                spacing: Theme.dp(7)
                Image { objectName: "hillListFlag-"+row.profileIndex; source: row.flag; sourceSize.width: 48; sourceSize.height: 32; Layout.preferredWidth: Theme.dp(24); Layout.preferredHeight: Theme.dp(16); fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
                Label { text: row.label; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight; color: Theme.ink; font.pointSize: Theme.body }
                Label { objectName: "hillListHs-"+row.profileIndex; text: "HS"+row.hs; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption }
            }
            ReorderHandle {
                visible: !library.season
                objectName: "hillDragHandle-"+row.profileIndex
                anchors.right: parent.right; anchors.rightMargin: Theme.dp(16); anchors.verticalCenter: parent.verticalCenter
                view: rows; rowIndex: row.index
                onMoved: function(from,to){library.backend.move(entries.get(from).profileIndex,entries.get(to).profileIndex)}
            }
            Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: 2; color: Theme.accent; visible: rows.reorderTarget===row.index }
            onClicked: {library.backend.select(profileIndex);rows.forceActiveFocus();library.activated()}
        }
        EmptyState { anchors.fill: parent; visible: rows.count===0; compact: true; title: library.query.length?qsTr("Brak wyników"):qsTr("Brak skoczni"); description: library.query.length?qsTr("Zmień nazwę lub kraj."):qsTr("Dodaj skocznię lub skopiuj ją z klasycznego modelu.") }
    }
}
