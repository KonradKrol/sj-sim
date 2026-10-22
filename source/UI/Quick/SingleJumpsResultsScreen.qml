import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: page
    objectName: "singleJumpsResultsWorkspace"
    readonly property bool parallel: width >= 1000
    readonly property var statistics: ui.find("toolBox")
    readonly property var table: ui.find("tableView")
    property int section: 0
    ColumnLayout {
        anchors.fill: parent; anchors.margins: Theme.compact ? 8 : 12
        spacing: Theme.gap
        RowLayout {
            Layout.fillWidth: true
            ColumnLayout {
                Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: 2
                RowLayout {
                    Layout.fillWidth: true
                    Node { node: ui.find("label_jumperFlag"); Layout.preferredWidth: 36; Layout.minimumWidth: 36 }
                    Node { node: ui.find("label_jumperNameAndSurname"); Layout.fillWidth: true; Layout.minimumWidth: 0 }
                }
                RowLayout {
                    Layout.fillWidth: true
                    Node { node: ui.find("label_hillFlag"); Layout.preferredWidth: 36; Layout.minimumWidth: 36 }
                    Node { node: ui.find("label_hillName"); Layout.fillWidth: true; Layout.minimumWidth: 0 }
                }
            }
            Node { node: ui.find("pushButton_sortTable"); Layout.preferredWidth: 140; Layout.maximumWidth: 160; Layout.minimumWidth: 0 }
        }
        SectionTabs { visible: !page.parallel; Layout.fillWidth: true; options: [qsTr("Wyniki"),qsTr("Szczegóły i statystyki")]; currentIndex: page.section; onSelected: page.section=index }
        RowLayout {
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
            spacing: Theme.gap
            DataTable {
                objectName: "singleJumpsResultsTable"
                visible: page.parallel || page.section===0
                Layout.fillWidth: true; Layout.fillHeight: true
                Layout.minimumWidth: 0; Layout.minimumHeight: 100; Layout.preferredHeight: 0
                table: page.table ? page.table.values.table : null
                ownerId: page.table ? page.table.values.id : -1
                title: qsTr("Wyniki")
                maximumHeight: Infinity
                columnWidths: [56,Math.max(100,(width-56)*.28),Math.max(100,(width-56)*.25),Math.max(120,(width-56)*.28),Math.max(110,(width-56)*.19)]
            }
            ColumnLayout {
                objectName: "singleJumpsResultsDetails"
                visible: page.parallel || page.section===1
                Layout.fillWidth: !page.parallel; Layout.fillHeight: true
                Layout.preferredWidth: page.parallel ? 360 : 0
                Layout.maximumWidth: page.parallel ? 440 : Infinity
                Layout.minimumHeight: 0; Layout.minimumWidth: 0
                SChoice {
                    objectName: "singleJumpsStatisticsChoice"
                    Layout.fillWidth: true; Layout.minimumWidth: 0
                    model: page.statistics ? page.statistics.values.options : []
                    currentIndex: page.statistics ? page.statistics.values.value : 0
                    Accessible.name: qsTr("Szczegóły i statystyki")
                    onActivated: ui.edit(page.statistics.values.id,currentIndex)
                }
                ScrollView {
                    id: details
                    Layout.fillWidth: true; Layout.fillHeight: true
                    Layout.minimumHeight: 0; Layout.minimumWidth: 0
                    contentWidth: availableWidth; clip: true
                    Node { width: details.availableWidth; node: ui.find("single-jump-details") }
                }
            }
        }
    }
}
