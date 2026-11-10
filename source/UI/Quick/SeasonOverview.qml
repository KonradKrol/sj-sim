import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: overview
    objectName: "seasonOverview"
    property var node
    readonly property var fields: node ? node.values : ({})
    readonly property var latest: fields.latest || ({})
    spacing: Theme.dp(8)
    RowLayout {
        Layout.fillWidth: true
        Label { text: qsTr("Rozegrano %1 / %2").arg(overview.fields.played || 0).arg(overview.fields.total || 0); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true }
        Label { text: qsTr("Pozostałe kraje: %1").arg(overview.fields.countries || 0); color: Theme.muted; font.pointSize: Theme.caption }
    }
    ProgressBar { Layout.fillWidth: true; value: overview.fields.total ? overview.fields.played / overview.fields.total : 0; Accessible.name: qsTr("Postęp sezonu") }
    Component {
        id: latestComponent
    Pane {
        objectName: "seasonLatestResults"
        visible: !!overview.latest.hill || !!overview.latest.type
        Layout.fillWidth: true; Layout.minimumWidth: 0; padding: Theme.dp(10)
        background: Surface { fillColor: Theme.surface; edgeColor: Theme.divider; strokeWidth: 1; sweep: 12 }
        contentItem: ColumnLayout {
            spacing: Theme.dp(5)
            RowLayout {
                Layout.fillWidth: true
                Label { text: qsTr("Ostatnie zawody"); color: Theme.ink; font.family: Theme.displayFamily; font.bold: true; font.pointSize: Theme.section; Layout.fillWidth: true }
                Label { text: overview.latest.date || ""; color: Theme.muted; font.pointSize: Theme.caption }
            }
            Label { text: (overview.latest.type || "") + " · " + (overview.latest.hill || "") + (overview.latest.hs ? " · HS"+overview.latest.hs : ""); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            Label { visible: !(overview.latest.results || []).length; text: overview.latest.cancelled ? qsTr("Wydarzenie odwołane") : qsTr("Brak sklasyfikowanych wyników"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true }
            Repeater {
                model: overview.latest.results || []
                RowLayout {
                    required property var modelData
                    Layout.fillWidth: true; spacing: Theme.dp(7)
                    Label { text: modelData.position+"."; color: modelData.position===1?Theme.accent:Theme.muted; font.family: Theme.mono; font.bold: true; font.pointSize: Theme.body; Layout.preferredWidth: Theme.dp(24) }
                    Image { source: modelData.photo || ""; visible: !!source && status===Image.Ready; Layout.preferredWidth: Theme.dp(26); Layout.preferredHeight: Theme.dp(26); sourceSize.width: 52; sourceSize.height: 52; fillMode: Image.PreserveAspectCrop; clip: true; Accessible.ignored: true }
                    Image { source: modelData.flag; visible: !!source && status!==Image.Error; Layout.preferredWidth: Theme.dp(22); Layout.preferredHeight: Theme.dp(15); sourceSize.width: 44; sourceSize.height: 30; fillMode: Image.PreserveAspectFit; Accessible.name: modelData.country }
                    Label { text: modelData.name; color: Theme.ink; font.bold: modelData.position===1; font.pointSize: Theme.body; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight }
                    Label { text: qsTr("%1 pkt").arg(modelData.points); color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption }
                }
            }
        }
    }
    }
    Repeater {
        model: overview.fields.events || []
        ColumnLayout {
            id: eventWrapper
            required property var modelData
            required property int index
            Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: Theme.dp(8)
            SeasonUpcomingEvent {
                Layout.fillWidth: true; Layout.minimumWidth: 0
                event: eventWrapper.modelData
                next: eventWrapper.index===0
                action: overview.fields.action ? overview.fields.action.values : ({})
            }
            Rectangle { visible: eventWrapper.index>0; Layout.fillWidth: true; implicitHeight: 1; color: Theme.divider; Accessible.ignored: true }
            Loader { active: eventWrapper.index===0 && (!!overview.latest.hill || !!overview.latest.type); Layout.fillWidth: true; Layout.minimumWidth: 0; sourceComponent: latestComponent }
        }
    }
    Loader { active: !(overview.fields.events || []).length && (!!overview.latest.hill || !!overview.latest.type); Layout.fillWidth: true; Layout.minimumWidth: 0; sourceComponent: latestComponent }
}
