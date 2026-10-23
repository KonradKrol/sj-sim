import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: overview
    objectName: "calendarDateOverview"
    property var backend
    property string month: backend.suggestedDate
    property string selectedDay: month
    readonly property bool compact: availableWidth<680
    readonly property var page: {var events=backend.events;return backend.monthData(month)}
    readonly property var dayEvents: {var events=backend.events;return events.filter(function(e){return e.date===overview.selectedDay})}
    signal editEvent(string eventId)
    contentWidth: availableWidth
    clip: true
    ColumnLayout {
        width: overview.availableWidth
        spacing: 12
        Pane {
            Layout.fillWidth: true; padding: 16
            background: Surface { fillColor: Theme.stadium; sweep: 24 }
            contentItem: ColumnLayout {
                spacing: 8
                RowLayout {
                    Layout.fillWidth: true
                    SButton { objectName: "date-month-previous"; text: "‹"; inverse: true; Accessible.name: qsTr("Poprzedni miesiąc"); onClicked: overview.month=overview.backend.offsetDate(overview.month,0,-1) }
                    Label { text: overview.page.title; color: Theme.snow; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                    SButton { objectName: "date-month-next"; text: "›"; inverse: true; Accessible.name: qsTr("Następny miesiąc"); onClicked: overview.month=overview.backend.offsetDate(overview.month,0,1) }
                }
                Label { text: qsTr("%1 konkursów w miesiącu · %2 bez daty w sezonie").arg(overview.page.count).arg(overview.page.undated); color: Theme.headerMuted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                SButton { text: qsTr("Pierwsze zawody"); inverse: true; enabled: !!overview.page.firstEvent; onClicked: {overview.month=overview.page.firstEvent;overview.selectedDay=overview.month} }
            }
        }
        CalendarDateField { Layout.fillWidth: true; backend: overview.backend; name: "month-jump"; label: qsTr("Przejdź do daty"); value: overview.selectedDay; onCommitted: function(iso){overview.selectedDay=iso;overview.month=iso} }
        Label { text: overview.compact ? qsTr("Agenda miesiąca · wybierz zawody, aby je edytować") : qsTr("Wybierz dzień, aby zobaczyć szczegóły lub przypisać zaznaczone konkursy."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        GridLayout {
            visible: !overview.compact
            Layout.fillWidth: true
            columns: 7; columnSpacing: 4; rowSpacing: 4
            Repeater {
                model: overview.page.weekdays
                Label { required property string modelData; text: modelData; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; horizontalAlignment: Text.AlignHCenter }
            }
            Repeater {
                model: overview.page.days
                Pane {
                    id: day
                    required property var modelData
                    Layout.fillWidth: true; Layout.preferredWidth: 100; Layout.preferredHeight: 152*Theme.scale
                    padding: 6
                    opacity: modelData.current ? 1 : .5
                    background: Rectangle { color: day.modelData.iso===overview.selectedDay ? Theme.selection : day.modelData.weekend ? Theme.raised : Theme.surface; radius: 8; border.width: day.modelData.iso===overview.selectedDay ? 2 : 0; border.color: Theme.accent }
                    contentItem: ColumnLayout {
                        spacing: 2
                        SButton { Layout.fillWidth: true; text: day.modelData.day+(day.modelData.today?" · "+qsTr("dziś"):""); quiet: true; implicitHeight: 30; Accessible.name: overview.backend.dateText(day.modelData.iso); onClicked: overview.selectedDay=day.modelData.iso }
                        Repeater {
                            model: day.modelData.events.slice(0,2)
                            SButton {
                                required property var modelData
                                Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredHeight: 44*Theme.scale
                                text: modelData.number+". "+modelData.title
                                font.pointSize: Theme.caption
                                primary: !!modelData.selected
                                hint: modelData.type+" · "+(modelData.played?qsTr("Rozegrany"):modelData.cancelled?qsTr("Odwołany"):qsTr("Zaplanowany"))
                                onClicked: overview.editEvent(modelData.id)
                            }
                        }
                        SButton { visible: day.modelData.events.length>2; Layout.fillWidth: true; implicitHeight: 28; quiet: true; text: qsTr("+%1 więcej").arg(day.modelData.events.length-2); onClicked: overview.selectedDay=day.modelData.iso }
                        Item { Layout.fillHeight: true }
                    }
                }
            }
        }
        ColumnLayout {
            visible: overview.compact
            Layout.fillWidth: true
            Repeater {
                model: overview.page.days.filter(function(day){return day.current && day.events.length>0})
                ColumnLayout {
                    required property var modelData
                    Layout.fillWidth: true
                    Label { text: overview.backend.dateText(parent.modelData.iso); color: Theme.ink; font.bold: true; font.pointSize: Theme.body }
                    Repeater {
                        model: parent.modelData.events
                        SButton { required property var modelData; Layout.fillWidth: true; text: modelData.number+". "+modelData.title+" · "+modelData.type; primary: !!modelData.selected; onClicked: overview.editEvent(modelData.id) }
                    }
                }
            }
            Label { visible: overview.page.count===0; text: qsTr("Ten miesiąc jest wolny. Zaplanuj zawody lub przejdź do innego miesiąca."); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        }
        Pane {
            Layout.fillWidth: true; padding: 12
            background: Surface { fillColor: Theme.surface; sweep: 20 }
            contentItem: ColumnLayout {
                Label { text: overview.backend.dateText(overview.selectedDay); color: Theme.ink; font.pointSize: Theme.section; font.family: Theme.displayFamily }
                Label { visible: overview.dayEvents.length===0; text: qsTr("Dzień bez zawodów — czas na odpoczynek lub podróż."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Repeater { model: overview.dayEvents; SButton { required property var modelData; Layout.fillWidth: true; text: modelData.title+" · "+modelData.type+" · "+(modelData.played?qsTr("Rozegrany"):modelData.cancelled?qsTr("Odwołany"):qsTr("Zaplanowany")); onClicked: overview.editEvent(modelData.id) } }
                SButton { objectName: "date-assign-day"; Layout.fillWidth: true; text: qsTr("Przypisz zaznaczone konkursy do tego dnia"); enabled: overview.backend.selection.count>0 && !overview.backend.selection.locked; onClicked: overview.backend.assignDate(overview.selectedDay) }
            }
        }
        SButton { visible: overview.page.undated>0; text: qsTr("Pokaż konkursy bez daty (%1)").arg(overview.page.undated); Layout.fillWidth: true; onClicked: {overview.backend.selectUndated();overview.editEvent("")} }
        Repeater { model: overview.backend.dateWarnings; Label { required property string modelData; text: "! "+modelData; color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap } }
    }
}
