import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: planner
    objectName: "calendarDatePlanner"
    property var backend
    property string startDate: backend.suggestedDate
    property bool future: false
    padding: 12
    background: Surface { fillColor: Theme.surface; sweep: 20 }
    contentItem: ColumnLayout {
        spacing: 8
        Label { text: qsTr("Zaplanuj terminy"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section }
        Label { text: qsTr("Zaznacz konkursy na liście lub zaplanuj wszystkie przyszłe. Najpierw zobaczysz podgląd; całą operację można cofnąć."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        SCheck { objectName: "date-plan-future"; text: qsTr("Wszystkie przyszłe konkursy"); checked: planner.future; Layout.fillWidth: true; onClicked: {planner.future=checked;planner.backend.discardDatePlan()} }
        CalendarDateField { id: startField; Layout.fillWidth: true; backend: planner.backend; name: "plan-start"; label: qsTr("Pierwszy termin"); value: planner.startDate; onCommitted: function(iso){planner.startDate=iso;planner.backend.discardDatePlan()} }
        SChoice {
            id: pattern
            objectName: "date-plan-pattern"
            Layout.fillWidth: true
            model: [qsTr("Weekendy: sobota i niedziela"),qsTr("Co tydzień, w ten sam dzień"),qsTr("Co określoną liczbę dni"),qsTr("Wszystkie tego samego dnia"),qsTr("Nowy początek — zachowaj odstępy")]
            Accessible.name: qsTr("Schemat terminów")
            onActivated: planner.backend.discardDatePlan()
        }
        NumberField {
            id: interval
            visible: pattern.currentIndex===2
            Layout.fillWidth: true; direct: false; label: qsTr("Odstęp w dniach")
            fields: ({name:"date-plan-interval",value:7,minimum:1,maximum:366,step:1,decimals:0})
            onCommitted: planner.backend.discardDatePlan()
        }
        NumberField {
            id: trainings
            Layout.fillWidth: true; direct: false; label: qsTr("Treningi: ile dni przed konkursem (0 = ten sam dzień)")
            fields: ({name:"date-plan-training-days",value:0,minimum:0,maximum:14,step:1,decimals:0})
            onCommitted: planner.backend.discardDatePlan()
        }
        SButton { objectName: "date-plan-preview"; text: qsTr("Pokaż plan"); primary: true; Layout.fillWidth: true; enabled: !startField.error && (planner.future || (planner.backend.selection.count>0 && !planner.backend.selection.locked)); onClicked: planner.backend.planDates(planner.startDate,["weekends","weekly","interval","same","preserve"][pattern.currentIndex],interval.currentValue,planner.future,trainings.currentValue) }
        Label { visible: planner.backend.datePlan.length>0; text: qsTr("Podgląd · konkursy: %1").arg(planner.backend.datePlan.length); color: Theme.ink; font.pointSize: Theme.body; font.bold: true }
        Repeater {
            model: planner.backend.datePlan
            RowLayout {
                required property var modelData
                Layout.fillWidth: true; spacing: 6
                Image { visible: !!parent.modelData.flag; source: parent.modelData.flag || ""; sourceSize.width: 48; sourceSize.height: 32; Layout.preferredWidth: 24; Layout.preferredHeight: 16; fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
                Label { text: parent.modelData.number+". "+parent.modelData.title+" · "+parent.modelData.before+" → "+parent.modelData.after; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; Layout.minimumWidth: 0; wrapMode: Text.WordWrap }
            }
        }
        RowLayout {
            visible: planner.backend.datePlan.length>0; Layout.fillWidth: true
            SButton { objectName: "date-plan-apply"; text: qsTr("Zastosuj plan"); primary: true; Layout.fillWidth: true; onClicked: planner.backend.applyDatePlan() }
            SButton { text: qsTr("Anuluj"); onClicked: planner.backend.discardDatePlan() }
        }
        Label { text: qsTr("Zachowaj istniejące odstępy"); color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Flow {
            Layout.fillWidth: true; spacing: 4
            Repeater {
                model: [-7,-1,1,7]
                SButton { required property int modelData; text: qsTr("%1 dni").arg((modelData>0?"+":"")+modelData); enabled: planner.backend.selection.count>0 && !planner.backend.selection.locked; onClicked: planner.backend.shiftDates(modelData) }
            }
        }
        SButton { text: qsTr("Usuń daty zaznaczonych"); Layout.fillWidth: true; enabled: planner.backend.selection.count>0 && !planner.backend.selection.locked; onClicked: planner.backend.clearDates() }
        SButton { text: qsTr("Zaznacz konkursy bez daty"); Layout.fillWidth: true; onClicked: planner.backend.selectUndated() }
        SButton { text: qsTr("Uporządkuj przyszłe według dat"); Layout.fillWidth: true; hint: qsTr("Kolejność symulacji wynika z listy. Ta operacja zmienia listę i może wymagać poprawienia powiązań kwalifikacji."); onClicked: planner.backend.sortDates() }
        Repeater {
            model: planner.backend.dateWarnings
            Label { required property string modelData; text: "! "+modelData; color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        }
    }
}
