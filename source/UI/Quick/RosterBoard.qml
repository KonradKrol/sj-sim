import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: board
    objectName: "rosterBoard"
    property var backend
    property var selectedLists: []
    property bool byCountry: true
    property bool exclusive: true
    property string query: ""
    property string picked: ""
    property real edgeScroll: 0
    signal finished()
    readonly property var snapshot: backend.rosterBoard
    readonly property var athletes: snapshot.athletes || []
    readonly property var members: snapshot.members || ({})
    readonly property var countries: {
        var result = []
        for (var i=0; i<athletes.length; ++i)
            if (result.indexOf(athletes[i].countryCode)<0) result.push(athletes[i].countryCode)
        return result.sort()
    }
    readonly property var sections: {
        var result=[]
        if (byCountry) for (var c=0;c<countries.length;++c) result.push({title:countries[c],country:countries[c],lists:selectedLists})
        else for (var l=0;l<selectedLists.length;++l) result.push({title:listName(selectedLists[l]),country:"",lists:[selectedLists[l]]})
        return result
    }
    function listName(id) {
        var lists=backend.rosterEntries
        for(var i=0;i<lists.length;++i)if(lists[i].id===id)return lists[i].name
        return ""
    }
    function rows(list,country) {
        var ids=members[list] || [], result=[]
        for(var i=0;i<ids.length;++i)for(var j=0;j<athletes.length;++j)
            if(athletes[j].athleteId===ids[i] && (!country || athletes[j].countryCode===country))result.push(athletes[j])
        if (!country) result.sort(function(a,b){return a.countryCode.localeCompare(b.countryCode)})
        return result
    }
    function assign(id,list) { backend.editRosterBoard(id,list,selectedLists,exclusive) }
    function leave() { if(backend.rosterBoardDirty)leaveDialog.open();else finished() }
    Component.onCompleted: {
        backend.beginRosterBoard()
        var ids=[];for(var i=0;i<backend.rosterEntries.length;++i)ids.push(backend.rosterEntries[i].id)
        selectedLists=ids
    }
    ColumnLayout {
        anchors.fill: parent; spacing: Theme.dp(8)
        RowLayout {
            Layout.fillWidth: true
            SButton { text: qsTr("Wróć"); quiet: true; onClicked: board.leave() }
            Label { text: qsTr("Rozdziel zawodników"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight }
            Label { text: qsTr("Niezastosowane zmiany"); visible: board.backend.rosterBoardDirty && board.width>Theme.dp(650); color: Theme.warning; font.pointSize: Theme.caption }
            SButton { objectName: "rosterBoardApply"; text: qsTr("Zastosuj"); primary: true; enabled: board.backend.rosterBoardDirty; onClicked: board.backend.applyRosterBoard() }
        }
        ScrollView {
            Layout.fillWidth: true; Layout.preferredHeight: chips.implicitHeight+Theme.dp(8)
            contentWidth: chips.implicitWidth; contentHeight: chips.implicitHeight
            clip: true
            Row {
                id: chips; spacing: Theme.dp(6)
                Repeater {
                    model: board.backend.rosterEntries
                    delegate: SButton {
                        required property var modelData
                        text: modelData.name; checkable: true
                        checked: board.selectedLists.indexOf(modelData.id)>=0
                        primary: checked
                        onClicked: {
                            var ids=board.selectedLists.slice(), at=ids.indexOf(modelData.id)
                            if(at<0)ids.push(modelData.id);else ids.splice(at,1)
                            board.selectedLists=ids
                        }
                    }
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            SChoice { model: [qsTr("Kraje → listy"),qsTr("Listy → kraje")]; currentIndex: board.byCountry ? 0 : 1; onActivated: board.byCountry=index===0; Layout.preferredWidth: Theme.dp(190) }
            SCheck { text: qsTr("Jeden skoczek na jednej wybranej liście"); checked: board.exclusive; onClicked: board.exclusive=checked; Layout.fillWidth: true }
        }
        RowLayout {
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0; spacing: Theme.dp(12)
            ColumnLayout {
                Layout.preferredWidth: Theme.dp(260); Layout.maximumWidth: Theme.dp(300); Layout.minimumWidth: Theme.dp(150); Layout.fillHeight: true
                Label { text: qsTr("Wszyscy skoczkowie"); color: Theme.ink; font.bold: true; font.pointSize: Theme.body }
                SField { Layout.fillWidth: true; placeholderText: qsTr("Nazwisko lub kraj"); onTextChanged: board.query=text.toLocaleLowerCase() }
                ListView {
                    id: library
                    Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
                    clip: true; spacing: Theme.dp(3); boundsBehavior: Flickable.StopAtBounds
                    model: board.athletes.filter(function(a){return (a.athleteName+" "+a.countryCode).toLocaleLowerCase().indexOf(board.query)>=0})
                    ScrollBar.vertical: ScrollBar {}
                    delegate: RosterBoardAthlete {
                        required property var modelData
                        width: library.width; athlete: modelData; selected: board.picked===modelData.athleteId
                        onClicked: board.picked=board.picked===modelData.athleteId ? "" : modelData.athleteId
                    }
                }
                Label { text: qsTr("Przeciągnij na listę lub wybierz skoczka i kliknij + na liście."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            }
            ScrollView {
                id: destinations
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
                clip: true; contentWidth: availableWidth
                Column {
                    width: destinations.availableWidth; spacing: Theme.dp(14)
                    Repeater {
                        model: board.sections
                        delegate: Column {
                            id: section
                            required property var modelData
                            width: parent.width; spacing: Theme.dp(6)
                            Label { text: section.modelData.title; color: Theme.ink; font.bold: true; font.pointSize: Theme.body }
                            Flow {
                                id: lanes
                                width: section.width; spacing: Theme.dp(6)
                            Repeater {
                                model: section.modelData.lists
                                delegate: RosterBoardLane {
                                    required property var modelData
                                    width: board.byCountry ? (lanes.width-Math.max(0,Math.min(section.modelData.lists.length,Math.floor(lanes.width/Theme.dp(230)))-1)*lanes.spacing)/Math.max(1,Math.min(section.modelData.lists.length,Math.floor(lanes.width/Theme.dp(230)))) : lanes.width
                                    objectName: "rosterBoardLane-"+modelData+"-"+section.modelData.country
                                    country: section.modelData.country
                                    title: board.byCountry ? board.listName(modelData) : ""
                                    athletes: board.rows(modelData,section.modelData.country)
                                    grouped: !board.byCountry
                                    canAdd: board.picked.length>0
                                    onDragPosition: function(y) {
                                        var point=mapToItem(destinations.contentItem,0,y)
                                        board.edgeScroll=point.y<Theme.dp(45) ? -Theme.dp(18) : point.y>destinations.height-Theme.dp(45) ? Theme.dp(18) : 0
                                    }
                                    onDragLeft: board.edgeScroll=0
                                    onAddClicked: board.assign(board.picked,modelData)
                                    onReceived: function(id){board.assign(id,modelData)}
                                    onRemoved: function(id){board.backend.editRosterBoard(id,"",[modelData],false)}
                                }
                            }
                            }
                        }
                    }
                    Label { visible: board.selectedLists.length===0; text: qsTr("Wybierz listy u góry. Ukryte listy pozostaną bez zmian."); width: parent.width; wrapMode: Text.WordWrap; color: Theme.muted }
                }
            }
        }
        Label { text: qsTr("Pusta lista oznacza wszystkich skoczków. Zmiany dotyczą wyłącznie wybranych list."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Label { text: board.backend.rosterNotice; visible: text.length>0; color: Theme.warning; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    }
    Timer {
        interval: 35; repeat: true; running: board.visible && board.edgeScroll!==0
        onTriggered: {
            var flick=destinations.contentItem
            flick.contentY=Math.max(0,Math.min(Math.max(0,flick.contentHeight-flick.height),flick.contentY+board.edgeScroll))
        }
    }
    Dialog {
        id: leaveDialog
        parent: Overlay.overlay; anchors.centerIn: parent; modal: true
        width: Math.min(Theme.dp(440),parent.width-Theme.dp(24))
        title: qsTr("Niezastosowane zmiany")
        contentItem: Label { text: qsTr("Odrzucić zmiany we wspólnym edytorze list?"); color: Theme.ink; wrapMode: Text.WordWrap }
        footer: DialogButtonBox {
            SButton { text: qsTr("Wróć do edycji"); DialogButtonBox.buttonRole: DialogButtonBox.RejectRole }
            SButton { text: qsTr("Odrzuć zmiany"); DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole; onClicked: { board.backend.discardRosterBoard();leaveDialog.close();board.finished() } }
            onRejected: leaveDialog.close()
        }
    }
}
