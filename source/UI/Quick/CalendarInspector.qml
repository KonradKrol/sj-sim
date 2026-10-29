import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: inspector
    ViewArrival { target: inspector; expressive: false }
    objectName: "calendarInspector"
    property var backend
    property bool compact: false
    signal editRequested()
    property var rulesNode
    property int section: 0
    implicitWidth: 0
    implicitHeight: 520
    padding: compact ? 8 : 16
    background: Surface { fillColor: Theme.surface; sweep: 26 }
    contentItem: Item {
        ColumnLayout {
            id: heading
            objectName: "calendarInspectorHeading"
            anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
            spacing: inspector.compact ? 8 : 12
            RowLayout {
                Layout.fillWidth: true; spacing: 8
                Image { objectName: "calendarInspectorFlag"; visible: !!inspector.backend.selection.flag; source: inspector.backend.selection.flag || ""; sourceSize.width: 48; sourceSize.height: 32; Layout.preferredWidth: 28; Layout.preferredHeight: 20; fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
                Label { text: inspector.backend.selection.title || qsTr("Wybierz konkurs"); color: Theme.ink; font.family: inspector.compact ? Theme.bodyFamily : Theme.displayFamily; font.pointSize: inspector.compact ? Theme.body : Theme.heading; Layout.fillWidth: true; Layout.minimumWidth: 0; wrapMode: inspector.compact ? Text.NoWrap : Text.WordWrap; elide: Text.ElideRight }
            }
            Label { visible: inspector.backend.selection.count > 1; text: qsTr("Zmiany dotyczą %1 konkursów").arg(inspector.backend.selection.count); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            Label { visible: inspector.backend.selection.count > 0 && inspector.backend.selection.locked; text: qsTr("Rozegrane wydarzenia są zablokowane."); color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            Label { visible: inspector.backend.selection.pendingSource>0; text: qsTr("Zachowano kwalifikację z konkursu %1. Sprawdź kolejność i rodzaj konkursów, aby przywrócić powiązanie.").arg(inspector.backend.selection.pendingSource); color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            SectionTabs { visible: inspector.backend.selection.count > 0; options: inspector.backend.datesEnabled ? [qsTr("Konkurs"),qsTr("Zasady"),qsTr("Powiązania"),qsTr("Terminy")] : [qsTr("Konkurs"),qsTr("Zasady"),qsTr("Powiązania")]; currentIndex: inspector.section; Layout.fillWidth: true; onSelected: inspector.section=index }
        }
        ScrollView {
            id: fieldsScroll
            objectName: "calendarInspectorFields"
            anchors.top: heading.bottom; anchors.bottom: parent.bottom
            anchors.left: parent.left; anchors.right: parent.right
            anchors.topMargin: inspector.compact ? 8 : 12
            contentWidth: availableWidth; clip: true
            ColumnLayout {
                width: fieldsScroll.availableWidth
                enabled: inspector.section===3 || (inspector.backend.selection.count > 0 && !inspector.backend.selection.locked)
                CalendarDatePlanner { visible: inspector.section===3 && inspector.backend.datesEnabled; backend: inspector.backend; Layout.fillWidth: true }
                CalendarFields { visible: inspector.section === 0; backend: inspector.backend; Layout.fillWidth: true; onEditRules: {inspector.backend.rulesTarget=target;inspector.section=1;inspector.editRequested()} }
                ColumnLayout {
                    visible: inspector.section === 1
                    Layout.fillWidth: true; spacing: 12
                    SChoice { Layout.fillWidth: true; model: [qsTr("Konkurs"),qsTr("Treningi"),qsTr("Seria próbna")]; currentIndex: inspector.backend.rulesTarget; Accessible.name: qsTr("Zasady wydarzenia"); onActivated: inspector.backend.rulesTarget=currentIndex }
                    Loader {
                        Layout.fillWidth: true; Layout.preferredHeight: item ? item.implicitHeight : 0
                        active: inspector.section === 1
                        source: Qt.resolvedUrl("Node.qml")
                        onLoaded: item.node = Qt.binding(function(){return inspector.rulesNode})
                    }
                }
                ColumnLayout {
                    visible: inspector.section === 2
                    Layout.fillWidth: true; spacing: 12
                    Label { text: qsTr("Klasyfikacje"); color: Theme.ink; font.pointSize: Theme.body; font.weight: Font.DemiBold }
                    Label { visible: inspector.backend.classifications.length === 0; text: qsTr("Brak klasyfikacji w kalendarzu"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                    Repeater {
                        model: inspector.backend.classifications
                        CalendarClassificationScoring { Layout.fillWidth: true; backend: inspector.backend; entry: modelData }
                    }
                    Label { text: qsTr("Kwalifikacja na podstawie konkursu"); color: Theme.muted; font.pointSize: Theme.caption }
                    SChoice { Layout.fillWidth: true; model: inspector.backend.advancementCompetitions; currentIndex: inspector.backend.selection.advancement === undefined ? -1 : inspector.backend.selection.advancement; Accessible.name: qsTr("Kwalifikacja na podstawie konkursu"); onActivated: inspector.backend.update({advancement:currentIndex}) }
                    Label { text: qsTr("Kwalifikacja według klasyfikacji"); color: Theme.muted; font.pointSize: Theme.caption }
                    SChoice { Layout.fillWidth: true; model: inspector.backend.advancementClassifications; currentIndex: inspector.backend.selection.advancementClass === undefined ? -1 : inspector.backend.selection.advancementClass; Accessible.name: qsTr("Kwalifikacja według klasyfikacji"); onActivated: inspector.backend.update({advancementClass:currentIndex}) }
                }
            }
        }
    }
}
