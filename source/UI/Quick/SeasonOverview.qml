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
            Pane {
            id: eventCard
            readonly property var modelData: eventWrapper.modelData
            readonly property int index: eventWrapper.index
            readonly property bool next: index===0
            readonly property color ink: next ? Theme.snow : Theme.ink
            readonly property color muted: next ? Theme.headerMuted : Theme.muted
            MenuEntrance { id: entrance; target: eventCard; delay: eventCard.index*35 }
            opacity: entrance.progress
            transform: Translate { y: Theme.dp(12)*(1-entrance.progress) }
            Layout.fillWidth: true; Layout.minimumWidth: 0; padding: Theme.dp(next ? 12 : 9)
            background: Surface { fillColor: eventCard.next ? Theme.stadium : Theme.raised; edgeColor: eventCard.next ? Theme.accent : Theme.divider; strokeWidth: eventCard.next ? 2 : 1; sweep: eventCard.next ? 36 : 12 }
            contentItem: ColumnLayout {
                spacing: Theme.dp(5)
                RowLayout {
                    Layout.fillWidth: true; spacing: Theme.dp(8)
                    Image { source: eventCard.modelData.flag; visible: !!source && status!==Image.Error; Layout.preferredWidth: Theme.dp(25); Layout.preferredHeight: Theme.dp(17); sourceSize.width: 50; sourceSize.height: 34; fillMode: Image.PreserveAspectFit; Accessible.name: eventCard.modelData.country }
                    Label { text: eventCard.modelData.hill; color: eventCard.ink; font.family: Theme.displayFamily; font.pointSize: eventCard.next ? Theme.heading : Theme.section; font.bold: true; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight }
                    Label { text: eventCard.modelData.hs ? "HS"+eventCard.modelData.hs : ""; color: eventCard.muted; font.family: Theme.mono; font.pointSize: Theme.caption }
                }
                Label { text: (eventCard.modelData.qualification ? qsTr("Kwalifikacje") : qsTr("Konkurs")) + " · " + eventCard.modelData.type + (eventCard.modelData.date ? " · "+eventCard.modelData.date : ""); color: eventCard.ink; font.bold: true; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Repeater {
                    model: eventCard.modelData.preparations || []
                    RowLayout {
                        required property var modelData
                        Layout.fillWidth: true; spacing: Theme.dp(5)
                        Label { text: modelData.current ? qsTr("TERAZ") : modelData.cancelled ? "—" : modelData.played ? "✓" : "•"; color: modelData.current ? Theme.snow : modelData.played ? (eventCard.next ? Theme.signal : Theme.success) : eventCard.muted; font.bold: modelData.current; font.pointSize: modelData.current ? Theme.body : Theme.caption }
                        Label { text: (modelData.training ? (modelData.total>1 ? qsTr("Trening %1 z %2").arg(modelData.number).arg(modelData.total) : qsTr("Trening")) : qsTr("Seria próbna")) + (eventCard.modelData.qualification ? qsTr(" przed kwalifikacjami") : qsTr(" przed konkursem")) + (modelData.cancelled ? (modelData.training ? qsTr(" · odwołany") : qsTr(" · odwołana")) : modelData.played ? (modelData.training ? qsTr(" · rozegrany") : qsTr(" · rozegrana")) : modelData.current ? "" : (modelData.training ? qsTr(" · zaplanowany") : qsTr(" · zaplanowana"))) + (modelData.date ? " · "+modelData.date : ""); color: modelData.current ? eventCard.ink : eventCard.muted; font.bold: modelData.current; font.pointSize: modelData.current ? Theme.body : Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                    }
                }
                Label { visible: eventCard.next && eventCard.modelData.current; text: qsTr("TERAZ · %1").arg(eventCard.modelData.qualification ? qsTr("Kwalifikacje") : qsTr("Konkurs")); color: Theme.snow; font.pointSize: Theme.body; font.bold: true; Layout.fillWidth: true }
                RowLayout {
                    visible: eventCard.next; Layout.fillWidth: true
                    SButton {
                        Layout.fillWidth: true; primary: true
                        readonly property var projectedAction: overview.fields.action ? overview.fields.action.values : ({})
                        text: projectedAction.text || ""; enabled: projectedAction.enabled!==false
                        onClicked: ui.activate(projectedAction.id)
                    }
                    SButton {
                        text: "⋯"; quiet: true; Accessible.name: qsTr("Opcje wydarzenia")
                        readonly property var cancellation: ui.find("pushButton_cancelNextCompetition")
                        enabled: !!cancellation && cancellation.values.enabled
                        onClicked: upcomingMenu.open()
                        ActionMenu { id: upcomingMenu; entries: [{text:qsTr("Odwołaj wydarzenie")}]; onSelected: if(cancellation)ui.activate(cancellation.values.id) }
                    }
                }
            }
            }
            Loader { active: eventWrapper.index===0 && (!!overview.latest.hill || !!overview.latest.type); Layout.fillWidth: true; Layout.minimumWidth: 0; sourceComponent: latestComponent }
        }
    }
    Loader { active: !(overview.fields.events || []).length && (!!overview.latest.hill || !!overview.latest.type); Layout.fillWidth: true; Layout.minimumWidth: 0; sourceComponent: latestComponent }
}
