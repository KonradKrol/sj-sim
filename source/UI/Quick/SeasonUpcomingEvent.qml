import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: card
    objectName: "seasonUpcomingEvent"
    property var event: ({})
    property bool next: false
    property var action: ({})
    property var cancellation: ({})
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    padding: Theme.dp(next ? 12 : 8)
    background: Surface { visible: card.next; fillColor: Theme.surface; edgeColor: Theme.accent; strokeWidth: 1.5; sweep: 22 }
    contentItem: Column {
        spacing: Theme.dp(6)
        RowLayout {
            width: parent.width; height: implicitHeight; spacing: Theme.dp(8)
            Image { source: card.event.flag || ""; visible: !!source; sourceSize.width: 48; sourceSize.height: 32; Layout.preferredWidth: Theme.dp(24); Layout.preferredHeight: Theme.dp(16); fillMode: Image.PreserveAspectFit; Accessible.name: card.event.country || "" }
            Label { text: card.event.hill || ""; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: card.next ? Theme.heading : Theme.section; font.bold: true; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight }
            Label { text: card.event.hs ? "HS"+card.event.hs : ""; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption }
        }
        RowLayout {
            width: parent.width; height: implicitHeight; spacing: Theme.dp(8)
            Label { text: (card.event.qualification ? qsTr("Kwalifikacje") : qsTr("Konkurs"))+" · "+(card.event.type || ""); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; Layout.minimumWidth: 0; wrapMode: Text.WordWrap }
            Label { text: card.event.date || ""; color: card.next ? Theme.accent : Theme.ink; font.family: Theme.mono; font.pointSize: Theme.caption; font.bold: card.next }
        }
        Flow {
            width: parent.width
            spacing: Theme.dp(5)
            Repeater {
                model: card.event.preparations || []
                Control {
                    id: preparation
                    required property var modelData
                    readonly property string title: modelData.training ? (modelData.total>1 ? qsTr("Trening %1").arg(modelData.number) : qsTr("Trening")) : qsTr("Próbna")
                    readonly property string statusText: modelData.cancelled ? qsTr("Odwołano") : modelData.played ? qsTr("Rozegrano") : modelData.current ? qsTr("Teraz") : qsTr("Zaplanowano")
                    padding: Theme.dp(4)
                    background: Rectangle { radius: Theme.dp(4); color: preparation.modelData.current ? Theme.selection : Theme.raised }
                    contentItem: Row {
                        spacing: Theme.dp(4)
                        Label { text: preparation.modelData.played ? "✓" : preparation.modelData.cancelled ? "×" : "•"; color: preparation.modelData.played ? Theme.success : preparation.modelData.current ? Theme.accent : Theme.muted; font.pointSize: Theme.caption }
                        Label { text: preparation.title+(preparation.modelData.date && preparation.modelData.date!==card.event.date ? " · "+preparation.modelData.date : ""); color: preparation.modelData.current ? Theme.accent : Theme.muted; font.bold: !!preparation.modelData.current; font.strikeout: !!preparation.modelData.cancelled; font.pointSize: Theme.caption }
                    }
                    Accessible.role: Accessible.StaticText
                    Accessible.name: title+" · "+statusText+" · "+(modelData.date || card.event.date || "")
                    ToolTip.visible: hover.hovered
                    ToolTip.text: Accessible.name
                    HoverHandler { id: hover }
                }
            }
        }

        RowLayout {
            visible: card.next
            width: parent.width; height: implicitHeight; spacing: Theme.dp(10)
            SButton { text: qsTr("Rozegraj"); primary: true; dense: true; Layout.fillWidth: true; enabled: card.action.id!==undefined && card.action.enabled!==false; onClicked: ui.activate(card.action.id) }
            SButton { text: qsTr("Odwołaj"); quiet: true; dense: true; foreground: Theme.danger; enabled: card.cancellation.id!==undefined && card.cancellation.enabled!==false; onClicked: ui.activate(card.cancellation.id) }
        }

    }
}
