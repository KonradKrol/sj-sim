import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: page
    ViewArrival { target: page; expressive: true; emphasis: arrivalHeading }
    readonly property var fields: ui.root.values
    readonly property var entry: fields.tableField || null
    readonly property bool hasRows: !!entry && !!entry.values.table && entry.values.table.count>0
    spacing: 12
    Pane {
        Layout.fillWidth: true; padding: 16
        background: Surface { fillColor: Theme.stadium; sweep: 24 }
        contentItem: RowLayout {
            spacing: 12
            Image { objectName: "classificationContributionFlag"; source: page.fields.flag || ""; sourceSize.width: 80; sourceSize.height: 56; Layout.preferredWidth: 48; Layout.preferredHeight: 32; fillMode: Image.PreserveAspectFit; Accessible.name: page.fields.country || "" }
            ColumnLayout {
                Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: 3
                Label { id: arrivalHeading; text: page.fields.country || ""; color: Theme.snow; font.family: Theme.displayFamily; font.pointSize: Theme.heading }
                Label { text: page.fields.name || ""; color: Theme.snow; opacity: .75; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            }
            ColumnLayout {
                spacing: 2
                Label { text: qsTr("Łączny wkład"); color: Theme.snow; opacity: .75; font.pointSize: Theme.caption }
                Label { text: (page.fields.total || "0,0")+qsTr(" pkt"); color: Theme.snow; font.family: Theme.mono; font.pointSize: Theme.section; font.bold: true }
            }
        }
    }
    Label { text: qsTr("Wkład zawodników i konkursów drużynowych"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    DataTable {
        objectName: "classificationContributionResults"
        visible: page.hasRows
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
        table: page.entry ? page.entry.values.table : null
        ownerId: page.entry ? page.entry.values.id : -1
        plain: true; searchEnabled: false
    }
    EmptyState { visible: !page.hasRows; Layout.fillWidth: true; Layout.fillHeight: true; title: qsTr("Brak wkładu punktowego"); description: qsTr("W wybranym zakresie nie naliczono punktów dla tego kraju.") }
}
