import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: weekend
    ViewArrival { target: weekend; expressive: false }
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
            if(!weekend.visible || weekend.backend.datesEnabled && date.error || hill.currentIndex<0 || rules.currentIndex<0)return
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
    Label { text: weekend.backend.datesEnabled ? qsTr("Dodaj kwalifikacje i dwa nowe konkursy — sobotni i niedzielny.") : qsTr("Dodaj dwa konkursy z kwalifikacjami. Ich kolejność ustalisz w kalendarzu."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    CalendarDateField { visible: weekend.backend.datesEnabled; id: date; name: "weekend-start"; label: qsTr("Weekend od"); backend: weekend.backend; value: weekend.start; Layout.fillWidth: true; onCommitted: function(iso){weekend.start=iso;weekend.invalidate()}; onErrorChanged: weekend.invalidate() }
    Label { text: qsTr("Skocznia"); color: Theme.muted; font.pointSize: Theme.caption }
    SChoice { id: hill; objectName: "weekend-hill"; searchable: true; Layout.fillWidth: true; model: weekend.backend.hills; optionImages: weekend.backend.hillFlags; Accessible.name: qsTr("Skocznia"); onActivated: weekend.invalidate() }
    RowLayout {
        Layout.fillWidth: true
        Label { text: qsTr("Zasady konkursów"); color: Theme.muted; font.pointSize: Theme.caption }
        HelpDot { explanation: qsTr("Oba konkursy otrzymają te zasady. Kwalifikacje i serie pomocnicze mają jedną serię. Klasyfikacje przypiszesz potem w Powiązaniach.") }
        Item { Layout.fillWidth: true }
    }
    SChoice { id: rules; currentIndex: weekend.backend.ruleDefaults.length ? weekend.backend.ruleDefaults[0].presetIndex : 0; objectName: "weekend-rules"; Layout.fillWidth: true; model: JSON.parse(weekend.rulesKey).slice(1); Accessible.name: qsTr("Zasady konkursów weekendu"); onActivated: weekend.invalidate() }
    RowLayout { Layout.fillWidth: true; Label { text: weekend.backend.datesEnabled ? qsTr("Pierwsze kwalifikacje") : qsTr("Przygotowanie pierwszego konkursu"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true } HelpDot { explanation: qsTr("Wybierz, jak przygotować pierwszy konkurs. Podgląd poniżej pokaże treningi i kwalifikacje.") } }
    SChoice { id: first; objectName: "weekend-first-qualification"; Layout.fillWidth: true; model: weekend.backend.datesEnabled ? [qsTr("Piątek · dwa treningi przed kwalifikacjami"),qsTr("Sobota · przed pierwszym konkursem")] : [qsTr("Dwa treningi i próbna"),qsTr("Same kwalifikacje")]; Accessible.name: weekend.backend.datesEnabled ? qsTr("Dzień pierwszych kwalifikacji") : qsTr("Przygotowanie pierwszego konkursu"); onActivated: weekend.invalidate() }
    RowLayout { Layout.fillWidth: true; Label { text: weekend.backend.datesEnabled ? qsTr("Niedziela") : qsTr("Drugi konkurs"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true } HelpDot { explanation: qsTr("Drugi konkurs może mieć własne kwalifikacje albo korzystać z wcześniejszych.") } }
    SChoice { id: sunday; objectName: "weekend-sunday"; Layout.fillWidth: true; model: [qsTr("Osobne kwalifikacje"),!weekend.backend.datesEnabled ? qsTr("Wspólne kwalifikacje") : first.currentIndex===0 ? qsTr("Kwalifikacje z piątku") : qsTr("Kwalifikacje z soboty")]; Accessible.name: qsTr("Kwalifikacje do drugiego konkursu"); onModelChanged: Qt.callLater(function(){sunday.currentIndex=weekend.sharedSunday?1:0}); onActivated: {weekend.sharedSunday=currentIndex===1;weekend.invalidate()} }
    Label { visible: weekend.sharedSunday; text: qsTr("Oba konkursy korzystają z tych samych kwalifikacji. Przed drugim odbędzie się seria próbna."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    Label { visible: (!weekend.backend.datesEnabled || !date.error) && !previewTimer.running && !weekend.backend.weekendPreview.length || weekend.added; text: weekend.added ? weekend.feedback : weekend.backend.message; color: weekend.added ? Theme.accent : Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    ColumnLayout {
        visible: weekend.backend.weekendPreview.length>0; Layout.fillWidth: true; spacing: 10
        Label { text: weekend.backend.datesEnabled ? qsTr("Plan weekendu") : qsTr("Plan pakietu"); color: Theme.muted; font.pointSize: Theme.caption }
        Repeater {
            model: weekend.backend.weekendPreview
            ColumnLayout {
                required property var modelData
                Layout.fillWidth: true; spacing: 3
                Label { text: weekend.backend.datesEnabled ? [qsTr("Piątek"),qsTr("Sobota"),qsTr("Niedziela")][parent.modelData.day]+" · "+parent.modelData.date : [qsTr("Przygotowanie"),qsTr("Pierwszy konkurs"),qsTr("Drugi konkurs")][parent.modelData.day]; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: parent.modelData.events; color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { visible: !!parent.modelData.qualification; text: parent.modelData.qualification; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Rectangle { Layout.fillWidth: true; implicitHeight: 1; color: Theme.outline }
            }
        }
        SButton { objectName: "weekend-apply"; text: weekend.backend.datesEnabled ? qsTr("Dodaj weekend do kalendarza") : qsTr("Dodaj pakiet do kalendarza"); primary: true; Layout.fillWidth: true; enabled: !previewTimer.running && (!weekend.backend.datesEnabled || !date.error); onClicked: {var friday=weekend.backend.normalizeDate(weekend.backend.weekendPreview[0].date);if(weekend.backend.applyWeekend()){if(weekend.backend.datesEnabled)weekend.start=weekend.backend.offsetDate(friday,7);weekend.feedback=qsTr("Dodano weekend do kalendarza.");weekend.added=true}} }
        Label { visible: weekend.backend.datesEnabled; text: qsTr("Następny weekend możesz dodać od razu — data przesunie się o tydzień."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    }
}
