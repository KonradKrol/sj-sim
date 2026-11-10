import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: planner
    objectName: "calendarDatePlanner"
    property var backend
    property string startDate: backend.suggestedDate
    property bool future: false
    property int mode: 0
    property int qualificationDays: 0
    property bool requested: false
    property bool allWarnings: false
    function invalidate(){requested=false;backend.discardDatePlan()}
    padding: Theme.dp(12)
    background: Surface { fillColor: Theme.surface; sweep: 20 }
    contentItem: ColumnLayout {
        spacing: Theme.dp(12)
        Label { text: qsTr("Szybkie planowanie"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; Layout.fillWidth: true }
        GridLayout {
            Layout.fillWidth: true; columns: planner.width>Theme.dp(620) ? 3 : 1; columnSpacing: Theme.dp(6); rowSpacing: Theme.dp(6)
            Repeater {
                model: [{key:0,icon:"↗",title:qsTr("Dodaj kwalifikacje")},{key:1,icon:"▦",title:planner.backend.datesEnabled ? qsTr("Dodaj weekend") : qsTr("Dodaj pakiet konkursów")}].concat(planner.backend.datesEnabled ? [{key:2,icon:"◷",title:qsTr("Ustal wiele terminów")}] : [])
                ItemDelegate {
                    required property var modelData
                    objectName: modelData.key===0 ? "date-open-qualifications" : modelData.key===1 ? "date-open-weekend" : "date-plan-existing"
                    Layout.fillWidth: true; Layout.preferredWidth: 0
                    implicitHeight: Math.max(40,contentItem.implicitHeight+16)
                    highlighted: planner.mode===modelData.key
                    contentItem: Label { text: parent.modelData.icon+"  "+parent.modelData.title; color: parent.highlighted ? Theme.accent : Theme.ink; font.pointSize: Theme.body; wrapMode: Text.WordWrap }
                    background: Rectangle { radius: Theme.dp(5); color: parent.highlighted ? Theme.selection : parent.hovered ? Theme.raised : Theme.surface; border.color: parent.highlighted ? Theme.accent : Theme.outline }
                    onClicked: {planner.forceActiveFocus();planner.invalidate();planner.backend.discardWeekend();planner.mode=modelData.key}
                }
            }
        }
        ColumnLayout {
            visible: planner.mode===0; Layout.fillWidth: true; spacing: Theme.dp(8)
            Label { text: qsTr("Kwalifikacje do zaznaczonych konkursów"); color: Theme.ink; font.pointSize: Theme.section; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            Label { text: qsTr("Dodaj po jednych kwalifikacjach przed każdym zaznaczonym konkursem. Awans i zgodne zasady zostaną powiązane automatycznie."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            ButtonGroup { id: qualificationDayGroup }
            Flow {
                visible: planner.backend.datesEnabled
                Layout.fillWidth: true; spacing: Theme.dp(12)
                Repeater {
                    model: [qsTr("W dniu konkursu"),qsTr("Dzień wcześniej")]
                    RadioButton {
                        id: dayChoice
                        required property int index
                        required property string modelData
                        text: modelData
                        ButtonGroup.group: qualificationDayGroup
                        checked: planner.qualificationDays===index
                        font.pointSize: Theme.caption
                        implicitHeight: Math.max(Theme.dp(24),contentItem.implicitHeight+Theme.dp(6))
                        padding: Theme.dp(2)
                        onClicked: planner.qualificationDays=index
                        indicator: Rectangle {
                            width: Theme.dp(12); height: width; radius: width/2
                            x: dayChoice.leftPadding; y: (dayChoice.height-height)/2
                            color: "transparent"; border.width: dayChoice.visualFocus ? 2 : 1
                            border.color: dayChoice.checked || dayChoice.visualFocus ? Theme.accent : Theme.outline
                            Rectangle { anchors.centerIn: parent; width: Theme.dp(6); height: width; radius: width/2; color: Theme.accent; visible: dayChoice.checked }
                        }
                        contentItem: Text { text: dayChoice.text; font: dayChoice.font; color: Theme.ink; leftPadding: Theme.dp(18); verticalAlignment: Text.AlignVCenter }
                    }
                }
            }
            SButton { text: qsTr("Dodaj kwalifikacje"); primary: true; enabled: planner.backend.selection.count>0 && !planner.backend.selection.locked; onClicked: planner.backend.addQualifications(planner.backend.datesEnabled ? planner.qualificationDays : 0) }
            Label { visible: planner.backend.selection.count===0; text: qsTr("Wróć do listy i zaznacz konkursy, do których chcesz dodać kwalifikacje."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        }
        CalendarWeekendPlanner { visible: planner.mode===1; Layout.fillWidth: true; backend: planner.backend }
        Repeater { model: planner.backend.datesEnabled ? planner.backend.dateWarnings : []; Label { required property string modelData; text: modelData; color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap } }
        ColumnLayout {
            visible: planner.mode===2 && planner.backend.datesEnabled
            Layout.fillWidth: true; spacing: Theme.dp(12)
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
                model: [qsTr("Sobota i niedziela"),qsTr("Ten sam dzień co tydzień"),qsTr("Co kilka dni"),qsTr("Wszystkie jednego dnia"),qsTr("Przesuń, zachowując odległości między datami"),qsTr("Piątek, sobota i niedziela")]
                Accessible.name: qsTr("Rozkład terminów")
                onActivated: planner.invalidate()
            }
            Label { Layout.fillWidth: true; text: [qsTr("Sobota → niedziela → kolejna sobota."),qsTr("Np. każda sobota, zaczynając od wybranej daty."),qsTr("Ustal, ile dni ma dzielić kolejne konkursy."),qsTr("Wszystkie wybrane konkursy otrzymają tę samą datę."),qsTr("Np. dwa dni między konkursami pozostaną dwoma dniami."),qsTr("Piątek → sobota → niedziela → kolejny piątek.")][pattern.currentIndex]; color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap }
            ColumnLayout {
                visible: pattern.currentIndex===2; Layout.fillWidth: true; spacing: 3
                Label { text: qsTr("Co ile dni?"); color: Theme.ink; font.pointSize: Theme.caption }
                NumberField { id: interval; Layout.fillWidth: true; direct: false; label: qsTr("Dni między konkursami"); fields: ({name:"date-plan-interval",value:7,minimum:1,maximum:366,step:1,decimals:0,suffix:qsTr("dni")}); onCommitted: planner.invalidate() }
            }
            ColumnLayout {
                visible: pattern.currentIndex!==4; Layout.fillWidth: true; spacing: 3
                Label { text: qsTr("Kiedy mają odbyć się treningi?"); color: Theme.muted; font.pointSize: Theme.caption }
                Label { text: qsTr("Dotyczy treningów już dodanych do tych konkursów."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                SChoice { id: trainingDay; Layout.fillWidth: true; model: [qsTr("W dniu konkursu"),qsTr("Dzień przed konkursem"),qsTr("Dwa dni przed konkursem"),qsTr("Inna liczba dni wcześniej")]; Accessible.name: qsTr("Kiedy zaplanować treningi"); onActivated: planner.invalidate() }
                Label { visible: trainingDay.currentIndex===3; text: qsTr("Ile dni przed konkursem?"); color: Theme.ink; font.pointSize: Theme.caption }
                NumberField { id: trainings; visible: trainingDay.currentIndex===3; Layout.fillWidth: true; direct: false; label: qsTr("Dni przed konkursem"); fields: ({name:"date-plan-training-days",value:3,minimum:0,maximum:14,step:1,decimals:0,suffix:qsTr("dni")}); onCommitted: planner.invalidate() }
            }
            SButton { objectName: "date-plan-preview"; text: qsTr("Podgląd terminów"); primary: true; Layout.fillWidth: true; enabled: !startField.error && (planner.future || (planner.backend.selection.count>0 && !planner.backend.selection.locked)); onClicked: {planner.requested=true;planner.backend.planDates(planner.startDate,["weekends","weekly","interval","same","preserve","fri-sun"][pattern.currentIndex],interval.currentValue,planner.future,trainingDay.currentIndex===3?trainings.currentValue:trainingDay.currentIndex,missing.checked)} }
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
                text: qsTr("Więcej opcji") + " ▾"; onClicked: more.open()
                ActionMenu { id: more; entries: [{id:"clear",text:qsTr("Usuń daty zaznaczonych"),enabled:planner.backend.selection.count>0&&!planner.backend.selection.locked},{id:"missing",text:qsTr("Zaznacz konkursy bez daty")},{id:"sort",text:qsTr("Uporządkuj nierozegrane według dat")}]; onSelected: {if(entry.id==="clear")planner.backend.clearDates();else if(entry.id==="missing")planner.backend.selectUndated();else planner.backend.sortDates()} }
            }
        }
    }
}
