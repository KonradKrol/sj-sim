import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: inspector
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
    contentItem: ColumnLayout {
        spacing: inspector.compact ? 8 : 12
        Label { text: inspector.backend.selection.title || qsTr("Wybierz konkurs"); color: Theme.ink; font.family: inspector.compact ? Theme.bodyFamily : Theme.displayFamily; font.pointSize: inspector.compact ? Theme.body : Theme.heading; Layout.fillWidth: true; wrapMode: inspector.compact ? Text.NoWrap : Text.WordWrap; elide: Text.ElideRight }
        Label { visible: inspector.backend.selection.count > 1; text: qsTr("Zmiany dotyczą %1 konkursów").arg(inspector.backend.selection.count); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Label { visible: inspector.backend.selection.count > 0 && inspector.backend.selection.locked; text: qsTr("Rozegrane wydarzenia są zablokowane."); color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Label { visible: inspector.backend.selection.pendingSource>0; text: qsTr("Zachowano kwalifikację z konkursu %1. Przenieś źródło przed ten konkurs, aby przywrócić powiązanie.").arg(inspector.backend.selection.pendingSource); color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        SectionTabs { visible: inspector.backend.selection.count > 0; options: [qsTr("Konkurs"),qsTr("Zasady"),qsTr("Powiązania")]; currentIndex: inspector.section; Layout.fillWidth: true; onSelected: inspector.section=index }
        ScrollView {
            id: fieldsScroll
            Layout.fillWidth: true; Layout.fillHeight: true
            Layout.minimumHeight: 0; Layout.preferredHeight: 0
            contentWidth: availableWidth; clip: true
            ColumnLayout {
                width: fieldsScroll.availableWidth
                enabled: inspector.backend.selection.count > 0 && !inspector.backend.selection.locked
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
                    SButton { text: qsTr("Zastosuj zasady"); primary: true; Layout.fillWidth: true; onClicked: inspector.backend.applyCustomRules() }
                    SButton { text: qsTr("Odrzuć zmiany zasad"); quiet: true; Layout.fillWidth: true; onClicked: inspector.backend.resetCustomRules() }
                }
                ColumnLayout {
                    visible: inspector.section === 2
                    Layout.fillWidth: true; spacing: 12
                    Label { text: qsTr("Klasyfikacje"); color: Theme.ink; font.pointSize: Theme.body; font.weight: Font.DemiBold }
                    Label { visible: inspector.backend.classifications.length === 0; text: qsTr("Brak klasyfikacji w kalendarzu"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                    Repeater { model: inspector.backend.classifications; SCheck { Layout.fillWidth: true; text: modelData.name; tristate: true; checkState: modelData.mixed ? Qt.PartiallyChecked : modelData.checked ? Qt.Checked : Qt.Unchecked; onClicked: inspector.backend.setClassification(modelData.index,checked) } }
                    Label { text: qsTr("Kwalifikacja na podstawie konkursu"); color: Theme.muted; font.pointSize: Theme.caption }
                    SChoice { Layout.fillWidth: true; model: inspector.backend.advancementCompetitions; currentIndex: inspector.backend.selection.advancement === undefined ? -1 : inspector.backend.selection.advancement; Accessible.name: qsTr("Kwalifikacja na podstawie konkursu"); onActivated: inspector.backend.update({advancement:currentIndex}) }
                    Label { text: qsTr("Kwalifikacja według klasyfikacji"); color: Theme.muted; font.pointSize: Theme.caption }
                    SChoice { Layout.fillWidth: true; model: inspector.backend.advancementClassifications; currentIndex: inspector.backend.selection.advancementClass === undefined ? -1 : inspector.backend.selection.advancementClass; Accessible.name: qsTr("Kwalifikacja według klasyfikacji"); onActivated: inspector.backend.update({advancementClass:currentIndex}) }
                }
            }
        }
    }
}
