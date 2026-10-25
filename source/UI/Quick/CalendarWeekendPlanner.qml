import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: weekend
    property var backend
    property string start: backend.suggestedDate
    property bool requested: false
    property bool added: false
    function invalidate(){requested=false;added=false;backend.discardWeekend()}
    spacing: 8
    Label { text: qsTr("Dodaj kwalifikacje i dwa nowe konkursy — sobotni i niedzielny."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    CalendarDateField { id: date; name: "weekend-start"; label: qsTr("Weekend od"); backend: weekend.backend; value: weekend.start; Layout.fillWidth: true; onCommitted: function(iso){weekend.start=iso;weekend.invalidate()} }
    Label { text: qsTr("Skocznia"); color: Theme.muted; font.pointSize: Theme.caption }
    SChoice { id: hill; objectName: "weekend-hill"; Layout.fillWidth: true; model: weekend.backend.hills; optionImages: weekend.backend.hillFlags; Accessible.name: qsTr("Skocznia weekendu"); onActivated: weekend.invalidate() }
    RowLayout {
        Layout.fillWidth: true
        Label { text: qsTr("Zasady konkursów"); color: Theme.muted; font.pointSize: Theme.caption }
        HelpDot { explanation: qsTr("Oba konkursy otrzymają te zasady. Kwalifikacje i serie pomocnicze mają jedną serię. Klasyfikacje przypiszesz potem w Powiązaniach.") }
        Item { Layout.fillWidth: true }
    }
    SChoice { id: rules; objectName: "weekend-rules"; Layout.fillWidth: true; model: weekend.backend.rules.slice(1); Accessible.name: qsTr("Zasady konkursów weekendu"); onActivated: weekend.invalidate() }
    Label { text: qsTr("Pierwsze kwalifikacje"); color: Theme.muted; font.pointSize: Theme.caption }
    SChoice { id: first; objectName: "weekend-first-qualification"; Layout.fillWidth: true; model: [qsTr("Piątek · dwa treningi przed kwalifikacjami"),qsTr("Sobota · przed pierwszym konkursem")]; Accessible.name: qsTr("Dzień pierwszych kwalifikacji"); onActivated: weekend.invalidate() }
    Label { text: qsTr("Niedziela"); color: Theme.muted; font.pointSize: Theme.caption }
    SChoice { id: sunday; objectName: "weekend-sunday"; Layout.fillWidth: true; model: [qsTr("Osobne kwalifikacje"),qsTr("Wspólne kwalifikacje · seria próbna")]; Accessible.name: qsTr("Kwalifikacje do niedzielnego konkursu"); onActivated: weekend.invalidate() }
    Label { visible: sunday.currentIndex===1; text: qsTr("Oba konkursy korzystają z pierwszych kwalifikacji."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    SButton { objectName: "weekend-preview"; text: qsTr("Podgląd weekendu"); primary: true; Layout.fillWidth: true; enabled: !date.error && hill.currentIndex>=0 && rules.currentIndex>=0; onClicked: {weekend.requested=true;weekend.added=false;weekend.backend.previewWeekend(weekend.start,hill.currentIndex,rules.currentIndex,first.currentIndex===1,sunday.currentIndex===1)} }
    Label { visible: (weekend.requested && !weekend.backend.weekendPreview.length || weekend.added) && !!weekend.backend.message; text: weekend.backend.message; color: weekend.added ? Theme.accent : Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    ColumnLayout {
        visible: weekend.backend.weekendPreview.length>0; Layout.fillWidth: true; spacing: 10
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
        SButton { objectName: "weekend-apply"; text: qsTr("Dodaj weekend do kalendarza"); primary: true; Layout.fillWidth: true; onClicked: {var friday=weekend.backend.normalizeDate(weekend.backend.weekendPreview[0].date);if(weekend.backend.applyWeekend()){weekend.requested=false;weekend.added=true;weekend.start=weekend.backend.offsetDate(friday,7)}else{weekend.requested=true;weekend.added=false}} }
        Label { text: qsTr("Następny weekend możesz dodać od razu — data przesunie się o tydzień."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    }
}
