import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Column {
    id: overview
    objectName: "seasonOverview"
    property var node
    readonly property var fields: node ? node.values : ({})
    readonly property var latest: fields.latest || ({})
    readonly property var chronologyBackend: fields.chronologyBackend || null
    readonly property var chronology: chronologyBackend ? chronologyBackend.chronology : ({blocked:false})
    spacing: Theme.dp(8)
    Pane {
        objectName: "autoFormNotice"
        width: parent.width
        visible: overview.chronologyBackend && overview.chronologyBackend.autoFormNoticeVisible
        padding: Theme.dp(12)
        background: Rectangle { color: Theme.surface; radius: Theme.controlRadius; border.color: Theme.accent }
        contentItem: ColumnLayout {
            spacing: Theme.dp(4)
            RowLayout {
                Layout.fillWidth: true
                Label { text: qsTr("Autoforma jest włączona"); color: Theme.ink; font.bold: true; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                SButton { text: "×"; quiet: true; Accessible.name: qsTr("Zamknij informację"); onClicked: overview.chronologyBackend.dismissAutoFormNotice(false) }
            }
            Label { text: qsTr("Forma zmienia się w sezonie, przynosząc zwroty akcji i realistyczne przetasowania. Możesz ją wyłączyć przełącznikiem Autoforma."); color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
            SButton { text: qsTr("Wyłącz autoformę"); quiet: true; onClicked: overview.chronologyBackend.dismissAutoFormNotice(true) }
        }
    }
    Pane {
        width: parent.width; visible: overview.chronology.blocked===true
        padding: Theme.dp(12)
        background: Rectangle { color: Theme.surface; border.color: Theme.danger; border.width: 2; radius: Theme.controlRadius }
        contentItem: ColumnLayout {
            spacing: Theme.dp(8)
            Label { text: qsTr("Zablokowane — kolejność wydarzeń"); color: Theme.danger; font.bold: true; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            Label { text: overview.chronology.message || ""; color: Theme.ink; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            SButton { text: qsTr("Przejdź do wydarzenia"); primary: true; onClicked: overview.chronologyBackend.goToChronologyBlocker() }
        }
    }

    RowLayout {
        width: parent.width; height: implicitHeight
        Label { text: qsTr("Rozegrano %1 / %2").arg(overview.fields.played || 0).arg(overview.fields.total || 0); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true }
        Label { text: qsTr("Konkursy do rozegrania: %1 (bez kwalifikacji)").arg(overview.fields.remainingCompetitions || 0); color: Theme.ink; font.pointSize: Theme.caption; Layout.fillWidth: true; Layout.minimumWidth: 0; horizontalAlignment: Text.AlignRight; wrapMode: Text.WordWrap }
    }
    ProgressBar {
        id: seasonProgress
        width: parent.width; height: Theme.dp(4); padding: 0
        value: overview.fields.total ? overview.fields.played / overview.fields.total : 0
        Accessible.name: qsTr("Postęp sezonu")
        background: Rectangle { color: Theme.raised; radius: height/2 }
        contentItem: Item { Rectangle { width: parent.width*seasonProgress.visualPosition; height: parent.height; radius: height/2; color: Theme.accent } }
    }
    Component {
        id: latestComponent
    Pane {
        objectName: "seasonLatestResults"
        visible: !!overview.latest.hill || !!overview.latest.type
        implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
        padding: Theme.dp(10)
        background: Surface { fillColor: Theme.surface; edgeColor: Theme.divider; strokeWidth: 1; sweep: 12 }
        contentItem: Column {
            spacing: Theme.dp(5)
            RowLayout {
                width: parent.width; height: implicitHeight
                Label { text: qsTr("Ostatnie zawody"); color: Theme.ink; font.family: Theme.displayFamily; font.bold: true; font.pointSize: Theme.section; Layout.fillWidth: true }
                Label { text: overview.latest.date || ""; color: Theme.muted; font.pointSize: Theme.caption }
            }
            Label { text: (overview.latest.type || "") + " · " + (overview.latest.hill || "") + (overview.latest.hs ? " · HS"+overview.latest.hs : ""); color: Theme.muted; font.pointSize: Theme.caption; width: parent.width; wrapMode: Text.WordWrap }
            Label { visible: !(overview.latest.results || []).length; text: overview.latest.cancelled ? qsTr("Wydarzenie odwołane") : qsTr("Brak sklasyfikowanych wyników"); color: Theme.muted; font.pointSize: Theme.caption; width: parent.width; wrapMode: Text.WordWrap }
            Repeater {
                model: overview.latest.results || []
                RowLayout {
                    required property var modelData
                    width: parent.width; height: implicitHeight; spacing: Theme.dp(7)
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
        Column {
            id: eventWrapper
            required property var modelData
            required property int index
            width: overview.width; spacing: Theme.dp(8)
            SeasonUpcomingEvent {
                width: parent.width
                quickAction: overview.fields.quickAction ? overview.fields.quickAction.values : ({})
                cancellation: overview.fields.cancelAction ? overview.fields.cancelAction.values : ({})
                event: eventWrapper.modelData
                next: eventWrapper.index===0
                action: overview.fields.action ? overview.fields.action.values : ({})
            }
            Rectangle { visible: eventWrapper.index>0; width: parent.width; height: 1; color: Theme.divider; Accessible.ignored: true }
            Loader { active: eventWrapper.index===0 && (!!overview.latest.hill || !!overview.latest.type); visible: active; width: parent.width; height: item ? item.implicitHeight : 0; sourceComponent: latestComponent }
        }
    }
    Loader { active: !(overview.fields.events || []).length && (!!overview.latest.hill || !!overview.latest.type); visible: active; width: parent.width; height: item ? item.implicitHeight : 0; sourceComponent: latestComponent }
}
