import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: overview
    objectName: "seasonOverview"
    property var node
    readonly property var fields: node ? node.values : ({})
    spacing: Theme.dp(8)
    RowLayout {
        Layout.fillWidth: true
        Label { text: qsTr("Rozegrano %1 / %2").arg(overview.fields.played || 0).arg(overview.fields.total || 0); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true }
        Label { text: qsTr("Pozostałe kraje: %1").arg(overview.fields.countries || 0); color: Theme.muted; font.pointSize: Theme.caption }
    }
    ProgressBar { Layout.fillWidth: true; value: overview.fields.total ? overview.fields.played / overview.fields.total : 0; Accessible.name: qsTr("Postęp sezonu") }
    Repeater {
        model: overview.fields.events || []
        Pane {
            id: eventCard
            required property var modelData
            required property int index
            readonly property bool next: index===0
            readonly property color ink: next ? Theme.snow : Theme.ink
            readonly property color muted: next ? Theme.headerMuted : Theme.muted
            MenuEntrance { id: entrance; target: eventCard; delay: eventCard.index*35 }
            opacity: entrance.progress
            transform: Translate { y: Theme.dp(12)*(1-entrance.progress) }
            Layout.fillWidth: true; Layout.minimumWidth: 0
            padding: Theme.dp(next ? 14 : 10)
            background: Surface { fillColor: eventCard.next ? Theme.stadium : Theme.raised; edgeColor: eventCard.next ? Theme.accent : Theme.divider; strokeWidth: eventCard.next ? 2 : 1; sweep: eventCard.next ? 36 : 12 }
            contentItem: ColumnLayout {
                spacing: Theme.dp(6)
                RowLayout {
                    Layout.fillWidth: true; spacing: Theme.dp(8)
                    Image { source: eventCard.modelData.flag; visible: !!source && status!==Image.Error; Layout.preferredWidth: Theme.dp(eventCard.next ? 36 : 26); Layout.preferredHeight: Theme.dp(eventCard.next ? 24 : 18); sourceSize.width: 72; sourceSize.height: 48; fillMode: Image.PreserveAspectFit; Accessible.name: eventCard.modelData.country }
                    Label { text: eventCard.modelData.hill; color: eventCard.ink; font.family: Theme.displayFamily; font.pointSize: eventCard.next ? Theme.heading : Theme.section; font.bold: true; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                    Label { visible: eventCard.modelData.qualification; text: "Q"; color: eventCard.ink; font.family: Theme.mono; font.bold: true; font.pointSize: Theme.section; Accessible.name: qsTr("Kwalifikacje") }
                    Label { text: eventCard.modelData.hs ? "HS"+eventCard.modelData.hs : ""; color: eventCard.muted; font.family: Theme.mono; font.pointSize: Theme.body }
                }
                Label { text: eventCard.modelData.date ? qsTr("%1 · %2").arg(eventCard.modelData.date).arg(eventCard.modelData.type) : eventCard.modelData.type; color: eventCard.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Flow {
                    Layout.fillWidth: true; spacing: Theme.dp(6)
                    Label { visible: eventCard.modelData.trainingCount>0; text: qsTr("Treningi %1/%2").arg(eventCard.modelData.trainingPlayed).arg(eventCard.modelData.trainingCount); color: eventCard.modelData.trainingPlayed===eventCard.modelData.trainingCount ? (eventCard.next ? Theme.signal : Theme.success) : eventCard.muted; font.pointSize: Theme.caption }
                    Label { visible: eventCard.modelData.trial; text: eventCard.modelData.trialPlayed ? qsTr("Próba rozegrana") : qsTr("Próba przed konkursem"); color: eventCard.modelData.trialPlayed ? (eventCard.next ? Theme.signal : Theme.success) : eventCard.muted; font.pointSize: Theme.caption }
                }
                Label { visible: eventCard.next; text: qsTr("Teraz: %1").arg(overview.fields.nextEvent || ""); color: Theme.snow; font.pointSize: Theme.body; font.bold: true; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                SButton {
                    visible: eventCard.next; Layout.fillWidth: true; primary: true
                    readonly property var projectedAction: overview.fields.action ? overview.fields.action.values : ({})
                    text: projectedAction.text || ""; enabled: projectedAction.enabled!==false
                    onClicked: ui.activate(projectedAction.id)
                }
            }
        }
    }
}
