import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: generator
    objectName: "formGeneratorWorkspace"
    property var node
    readonly property var fields: node.values
    readonly property bool parallel: width >= 1000 && height >= 520
    property int section: 0
    ColumnLayout {
        anchors.fill: parent; spacing: 12
        SectionTabs { visible: !generator.parallel; options: [qsTr("Zawodnicy"),qsTr("Parametry")]; currentIndex: generator.section; Layout.fillWidth: true; onSelected: generator.section=index }
        RowLayout {
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0; spacing: 24
            ScrollView {
                id: parameters
                visible: generator.parallel || generator.section === 1
                Layout.fillWidth: !generator.parallel; Layout.preferredWidth: generator.parallel ? 300 : 0
                Layout.fillHeight: true; Layout.minimumHeight: 0; Layout.minimumWidth: 0
                contentWidth: availableWidth; clip: true
                ColumnLayout {
                    width: parameters.availableWidth; spacing: 12
                    Label { text: qsTr("Parametry formy"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section }
                    Repeater { model: generator.fields.parameters || []; Node { node: modelData; Layout.fillWidth: true } }
                    Label { text: qsTr("Presety"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.topMargin: 8 }
                    Repeater { model: generator.fields.presets || []; Node { node: modelData; Layout.fillWidth: true } }
                }
            }
            ColumnLayout {
                visible: generator.parallel || generator.section === 0
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
                spacing: 8
                RowLayout {
                    Layout.fillWidth: true
                    Label { text: qsTr("Zawodnicy"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true }
                    Node { node: generator.fields.lists || null; Layout.preferredWidth: Math.min(220,generator.width/2); Layout.minimumWidth: 0 }
                }
                DataTable {
                    objectName: "formGeneratorTable"
                    Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0; Layout.minimumWidth: 0
                    table: generator.fields.table ? generator.fields.table.values.table : null
                    ownerId: generator.fields.table ? generator.fields.table.values.id : -1
                    actions: ui.actions
                    title: qsTr("Parametry zawodników")
                }
            }
        }
        Node { objectName: "formGeneratorSubmit"; node: generator.fields.submit || null; Layout.fillWidth: true; Layout.preferredHeight: implicitHeight }
    }
}
