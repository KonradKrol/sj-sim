import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: panel
    property var backend
    property bool expanded: false
    readonly property var diagnostic: backend.profile.formDiagnostic || ({})
    readonly property var rows: diagnostic.rows || []
    readonly property var groups: [qsTr("Stan"),qsTr("Dynamika"),qsTr("Następna zmiana okresu"),qsTr("Losowanie")]
    visible: Object.keys(diagnostic).length>0
    spacing: Theme.dp(8)
    ItemDelegate {
        objectName: "formDiagnosticsToggle"
        Layout.fillWidth: true; implicitHeight: Math.max(Theme.dp(32),Theme.bodyMetrics.height+Theme.dp(8))
        padding: Theme.dp(6)
        onClicked: panel.expanded=!panel.expanded
        Accessible.name: panel.expanded ? qsTr("Zwiń diagnostykę formy") : qsTr("Rozwiń diagnostykę formy")
        background: Rectangle { color: parent.hovered || panel.expanded ? Theme.raised : "transparent"; radius: Theme.dp(4) }
        contentItem: RowLayout {
            Label { text: qsTr("Diagnostyka formy"); font.pointSize: Theme.body; font.bold: true; color: Theme.ink; Layout.fillWidth: true }
            Label { text: panel.expanded ? "−" : "+"; color: Theme.muted; font.pointSize: Theme.section }
        }
    }
    ColumnLayout {
        visible: panel.expanded; Layout.fillWidth: true; spacing: Theme.dp(10)
        Label { visible: !!panel.diagnostic.pending; text: qsTr("Stan procesu nie został jeszcze zainicjalizowany."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Repeater {
            model: panel.groups
            ColumnLayout {
                id: section
                required property string modelData
                readonly property var entries: panel.rows.filter(function(row){return row.group===section.modelData})
                visible: entries.length>0; Layout.fillWidth: true; spacing: Theme.dp(4)
                Label { text: section.modelData; color: Theme.ink; font.pointSize: Theme.body; font.bold: true }
                GridLayout {
                    Layout.fillWidth: true; columns: width>=Theme.dp(700) ? 2 : 1
                    columnSpacing: Theme.dp(22); rowSpacing: Theme.dp(4)
                    Repeater {
                        model: section.entries
                        RowLayout {
                            id: entry
                            required property var modelData
                            Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: 0
                            Label { text: entry.modelData.label; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; Layout.minimumWidth: 0; wrapMode: Text.WordWrap }
                            Label { text: entry.modelData.value; color: Theme.ink; font.family: Theme.mono; font.pointSize: Theme.caption; Layout.preferredWidth: Theme.dp(115); Layout.maximumWidth: Theme.dp(160); Layout.minimumWidth: 0; wrapMode: Text.WrapAnywhere }
                            HoverHandler { id: hover }
                            ToolTip.visible: hover.hovered && !!entry.modelData.help
                            ToolTip.text: entry.modelData.help || ""
                            ToolTip.delay: 500
                        }
                    }
                }
            }
        }
    }
}
