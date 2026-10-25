import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: planner
    objectName: "calendarDatePlanner"
    property var backend
    property string startDate: backend.suggestedDate
    property bool future: false
    property bool weekendMode: false
    property bool requested: false
    function invalidate(){requested=false;backend.discardDatePlan()}
    padding: 12
    background: Surface { fillColor: Theme.surface; sweep: 20 }
    contentItem: ColumnLayout {
        spacing: 12
        Label { text: qsTr("Zaplanuj terminy"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        RowLayout {
            Layout.fillWidth: true; spacing: 6
            SButton { objectName: "date-plan-existing"; text: qsTr("Ustal terminy"); primary: !planner.weekendMode; Layout.fillWidth: true; Layout.preferredWidth: 1; implicitHeight: Theme.controlHeight; font.pointSize: Theme.body; onClicked: {planner.invalidate();planner.backend.discardWeekend();planner.weekendMode=false} }
            SButton { objectName: "date-open-weekend"; text: qsTr("Dodaj weekend"); primary: planner.weekendMode; Layout.fillWidth: true; Layout.preferredWidth: 1; implicitHeight: Theme.controlHeight; font.pointSize: Theme.body; onClicked: {planner.invalidate();planner.weekendMode=true} }
        }
        Label { text: planner.weekendMode ? qsTr("Dodawanie nowych konkursów") : qsTr("Daty istniejących konkursów"); color: Theme.ink; font.pointSize: Theme.body; font.bold: true; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Label { visible: !planner.weekendMode; text: qsTr("Ustal lub zmień daty konkursów, które są już w kalendarzu."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        CalendarWeekendPlanner { visible: planner.weekendMode; Layout.fillWidth: true; backend: planner.backend }
        ColumnLayout {
            visible: !planner.weekendMode
            Layout.fillWidth: true; spacing: 8
            Label { text: qsTr("Konkursy do zaplanowania"); color: Theme.muted; font.pointSize: Theme.caption }
            SChoice { objectName: "date-plan-future"; Layout.fillWidth: true; model: [qsTr("Zaznaczone (%1)").arg(planner.backend.selection.count),qsTr("Wszystkie nierozegrane")]; currentIndex: planner.future?1:0; Accessible.name: qsTr("Konkursy do zaplanowania"); onActivated: {planner.future=currentIndex===1;planner.invalidate()} }
            CheckHelp { Layout.fillWidth: true; controlName: "date-plan-missing"; text: qsTr("Tylko brakujące daty"); checked: missing.checked; explanation: qsTr("Konkursy z ustaloną datą zachowają swoje terminy."); onClicked: {missing.checked=value;planner.invalidate()} }
            Item { id: missing; property bool checked: false; visible: false }
            CalendarDateField { id: startField; Layout.fillWidth: true; backend: planner.backend; name: "plan-start"; label: qsTr("Data pierwszego konkursu"); value: planner.startDate; onCommitted: function(iso){planner.startDate=iso;planner.invalidate()} }
            Label { text: qsTr("Jak rozłożyć konkursy"); color: Theme.muted; font.pointSize: Theme.caption }
            SChoice {
                id: pattern
                objectName: "date-plan-pattern"
                Layout.fillWidth: true
                model: [qsTr("Sobota i niedziela"),qsTr("Ten sam dzień co tydzień"),qsTr("Co kilka dni"),qsTr("Wszystkie jednego dnia"),qsTr("Przesuń, zachowując odległości między datami")]
                Accessible.name: qsTr("Rozkład terminów")
                onActivated: planner.invalidate()
            }
            Label { Layout.fillWidth: true; text: [qsTr("Sobota → niedziela → kolejna sobota."),qsTr("Np. każda sobota, zaczynając od wybranej daty."),qsTr("Ustal, ile dni ma dzielić kolejne konkursy."),qsTr("Wszystkie wybrane konkursy otrzymają tę samą datę."),qsTr("Np. dwa dni między konkursami pozostaną dwoma dniami.")][pattern.currentIndex]; color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap }
            ColumnLayout {
                visible: pattern.currentIndex===2; Layout.fillWidth: true; spacing: 3
                Label { text: qsTr("Co ile dni?"); color: Theme.ink; font.pointSize: Theme.caption }
                NumberField { id: interval; Layout.fillWidth: true; direct: false; label: qsTr("Dni między konkursami"); fields: ({name:"date-plan-interval",value:7,minimum:1,maximum:366,step:1,decimals:0,suffix:qsTr("dni")}); onCommitted: planner.invalidate() }
            }
            ColumnLayout {
                visible: pattern.currentIndex!==4; Layout.fillWidth: true; spacing: 3
                Label { text: qsTr("Treningi"); color: Theme.muted; font.pointSize: Theme.caption }
                SChoice { id: trainingDay; Layout.fillWidth: true; model: [qsTr("W dniu konkursu"),qsTr("Dzień przed konkursem"),qsTr("Dwa dni przed konkursem"),qsTr("Inna liczba dni wcześniej")]; Accessible.name: qsTr("Kiedy zaplanować treningi"); onActivated: planner.invalidate() }
                Label { visible: trainingDay.currentIndex===3; text: qsTr("Ile dni przed konkursem?"); color: Theme.ink; font.pointSize: Theme.caption }
                NumberField { id: trainings; visible: trainingDay.currentIndex===3; Layout.fillWidth: true; direct: false; label: qsTr("Dni przed konkursem"); fields: ({name:"date-plan-training-days",value:3,minimum:0,maximum:14,step:1,decimals:0,suffix:qsTr("dni")}); onCommitted: planner.invalidate() }
            }
            SButton { objectName: "date-plan-preview"; text: qsTr("Podgląd terminów"); primary: true; Layout.fillWidth: true; enabled: !startField.error && (planner.future || (planner.backend.selection.count>0 && !planner.backend.selection.locked)); onClicked: {planner.requested=true;planner.backend.planDates(planner.startDate,["weekends","weekly","interval","same","preserve"][pattern.currentIndex],interval.currentValue,planner.future,trainingDay.currentIndex===3?trainings.currentValue:trainingDay.currentIndex,missing.checked)} }
            Label { visible: planner.requested && !planner.backend.datePlan.length && !!planner.backend.message; text: planner.backend.message; color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            ColumnLayout {
                visible: planner.backend.datePlan.length>0; Layout.fillWidth: true; spacing: 8
                Label { text: qsTr("Podgląd · %1 konkursów").arg(planner.backend.datePlan.length); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section }
                RowLayout {
                    Layout.fillWidth: true
                    SButton { objectName: "date-plan-apply"; text: qsTr("Zapisz terminy"); primary: true; Layout.fillWidth: true; onClicked: {planner.requested=false;planner.backend.applyDatePlan()} }
                    SButton { text: qsTr("Anuluj"); quiet: true; onClicked: planner.invalidate() }
                }
                Repeater {
                    model: planner.backend.datePlan
                    RowLayout {
                        required property var modelData
                        Layout.fillWidth: true; spacing: 8
                        Image { source: parent.modelData.flag || ""; visible: !!parent.modelData.flag; sourceSize.width: 48; sourceSize.height: 32; Layout.preferredWidth: 24; Layout.preferredHeight: 16; fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
                        ColumnLayout {
                            Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: 1
                            Label { text: parent.parent.modelData.number+". "+parent.parent.modelData.title; color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; elide: Text.ElideRight }
                            Label { text: parent.parent.modelData.before+" → "+parent.parent.modelData.after; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                        }
                    }
                }
            }
            Rectangle { Layout.fillWidth: true; implicitHeight: 1; color: Theme.outline }
            Label { text: qsTr("Przesuń zaznaczone terminy"); color: Theme.ink; font.pointSize: Theme.body; font.bold: true }
            Flow {
                Layout.fillWidth: true; spacing: 4
                Repeater { model: [-7,-1,1,7]; SButton { required property int modelData; text: qsTr("%1 dni").arg((modelData>0?"+":"")+modelData); enabled: planner.backend.selection.count>0 && !planner.backend.selection.locked; onClicked: planner.backend.shiftDates(modelData) } }
            }
            SButton {
                text: qsTr("Więcej opcji"); quiet: true; onClicked: more.open()
                ActionMenu { id: more; entries: [{id:"clear",text:qsTr("Usuń daty zaznaczonych"),enabled:planner.backend.selection.count>0&&!planner.backend.selection.locked},{id:"missing",text:qsTr("Zaznacz konkursy bez daty")},{id:"sort",text:qsTr("Uporządkuj nierozegrane według dat")}]; onSelected: {if(entry.id==="clear")planner.backend.clearDates();else if(entry.id==="missing")planner.backend.selectUndated();else planner.backend.sortDates()} }
            }
            Repeater { model: planner.backend.dateWarnings; Label { required property string modelData; text: "! "+modelData; color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap } }
        }
    }
}
