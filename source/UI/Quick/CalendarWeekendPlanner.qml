import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: weekend
    property var backend
    property string start: backend.suggestedDate
    readonly property string rulesKey: JSON.stringify(backend.rules)
    property bool ready: false
    property bool updating: false
    property bool sharedSunday: false
    property string feedback: ""
    property bool added: false
    function invalidate(){if(!ready || !visible)return;added=false;previewTimer.restart();backend.discardWeekend()}
    onStartChanged: invalidate()
    onVisibleChanged: {if(visible)invalidate();else previewTimer.stop()}
    Component.onCompleted: {ready=true;invalidate()}
    Timer {
        id: previewTimer
        interval: 0
        onTriggered: {
            if(!weekend.visible || date.error || hill.currentIndex<0 || rules.currentIndex<0)return
            weekend.updating=true
            weekend.backend.previewWeekend(weekend.start,hill.currentIndex,rules.currentIndex,first.currentIndex===1,weekend.sharedSunday)
            weekend.updating=false
        }
    }
    Connections {
        target: weekend.backend
        property bool hadPreview: false
        function onChanged(){var has=weekend.backend.weekendPreview.length>0;var cleared=hadPreview && !has;hadPreview=has;if(cleared && !weekend.updating)weekend.invalidate()}
    }
    spacing: 8
    Label { text: qsTr("Dodaj kwalifikacje i dwa nowe konkursy — sobotni i niedzielny."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    CalendarDateField { id: date; name: "weekend-start"; label: qsTr("Weekend od"); backend: weekend.backend; value: weekend.start; Layout.fillWidth: true; onCommitted: function(iso){weekend.start=iso;weekend.invalidate()}; onErrorChanged: weekend.invalidate() }
    Label { text: qsTr("Skocznia"); color: Theme.muted; font.pointSize: Theme.caption }
    SChoice { id: hill; objectName: "weekend-hill"; Layout.fillWidth: true; model: weekend.backend.hills; optionImages: weekend.backend.hillFlags; Accessible.name: qsTr("Skocznia weekendu"); onActivated: weekend.invalidate() }
    RowLayout {
        Layout.fillWidth: true
        Label { text: qsTr("Zasady konkursów"); color: Theme.muted; font.pointSize: Theme.caption }
        HelpDot { explanation: qsTr("Oba konkursy otrzymają te zasady. Kwalifikacje i serie pomocnicze mają jedną serię. Klasyfikacje przypiszesz potem w Powiązaniach.") }
        Item { Layout.fillWidth: true }
    }
    SChoice { id: rules; objectName: "weekend-rules"; Layout.fillWidth: true; model: JSON.parse(weekend.rulesKey).slice(1); Accessible.name: qsTr("Zasady konkursów weekendu"); onActivated: weekend.invalidate() }
    Label { text: qsTr("Pierwsze kwalifikacje"); color: Theme.muted; font.pointSize: Theme.caption }
    SChoice { id: first; objectName: "weekend-first-qualification"; Layout.fillWidth: true; model: [qsTr("Piątek · dwa treningi przed kwalifikacjami"),qsTr("Sobota · przed pierwszym konkursem")]; Accessible.name: qsTr("Dzień pierwszych kwalifikacji"); onActivated: weekend.invalidate() }
    Label { text: qsTr("Niedziela"); color: Theme.muted; font.pointSize: Theme.caption }
    SChoice { id: sunday; objectName: "weekend-sunday"; Layout.fillWidth: true; model: [qsTr("Osobne kwalifikacje"),first.currentIndex===0 ? qsTr("Bierz kwalifikacje z piątku + rozegraj serię próbną") : qsTr("Bierz kwalifikacje z soboty + rozegraj serię próbną")]; Accessible.name: qsTr("Kwalifikacje do niedzielnego konkursu"); onModelChanged: Qt.callLater(function(){sunday.currentIndex=weekend.sharedSunday?1:0}); onActivated: {weekend.sharedSunday=currentIndex===1;weekend.invalidate()}
        implicitHeight: Math.max(Theme.controlHeight,contentItem.implicitHeight+topPadding+bottomPadding)
        contentItem: Text { text: sunday.displayText; font: sunday.font; color: Theme.ink; wrapMode: Text.WordWrap; verticalAlignment: Text.AlignVCenter }
        delegate: ItemDelegate {
            width: sunday.popup.width
            implicitHeight: Math.max(Theme.controlHeight,contentItem.implicitHeight+topPadding+bottomPadding)
            highlighted: sunday.highlightedIndex===index
            contentItem: Text { text: modelData; font: sunday.font; color: Theme.ink; wrapMode: Text.WordWrap; verticalAlignment: Text.AlignVCenter }
            background: Rectangle { color: parent.highlighted ? Theme.selection : Theme.surface }
        }
    }
    Label { visible: weekend.sharedSunday; text: qsTr("Oba konkursy korzystają z pierwszych kwalifikacji."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    Label { visible: !date.error && !previewTimer.running && !weekend.backend.weekendPreview.length || weekend.added; text: weekend.added ? weekend.feedback : weekend.backend.message; color: weekend.added ? Theme.accent : Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    ColumnLayout {
        visible: weekend.backend.weekendPreview.length>0; Layout.fillWidth: true; spacing: 10
        Label { text: qsTr("Plan weekendu"); color: Theme.muted; font.pointSize: Theme.caption }
        Repeater {
            model: weekend.backend.weekendPreview
            ColumnLayout {
                required property var modelData
                Layout.fillWidth: true; spacing: 3
                Label { text: [qsTr("Piątek"),qsTr("Sobota"),qsTr("Niedziela")][parent.modelData.day]+" · "+parent.modelData.date; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: parent.modelData.events; color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { visible: !!parent.modelData.qualification; text: parent.modelData.qualification; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Rectangle { Layout.fillWidth: true; implicitHeight: 1; color: Theme.outline }
            }
        }
        SButton { objectName: "weekend-apply"; text: qsTr("Dodaj weekend do kalendarza"); primary: true; Layout.fillWidth: true; enabled: !previewTimer.running && !date.error; onClicked: {var friday=weekend.backend.normalizeDate(weekend.backend.weekendPreview[0].date);if(weekend.backend.applyWeekend()){weekend.start=weekend.backend.offsetDate(friday,7);weekend.feedback=qsTr("Dodano weekend do kalendarza.");weekend.added=true}} }
        Label { text: qsTr("Następny weekend możesz dodać od razu — data przesunie się o tydzień."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    }
}
