import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: lane
    property string country: ""
    property string title
    property var athletes: []
    property bool grouped: false
    property bool canAdd: false
    signal received(string id)
    signal dragPosition(real y)
    signal dragLeft()
    signal removed(string id)
    signal addClicked()
    implicitHeight: contents.implicitHeight+Theme.dp(12)
    height: implicitHeight
    radius: Theme.dp(5); color: drop.containsDrag ? Theme.selection : Theme.raised
    border.color: drop.containsDrag ? Theme.accent : Theme.divider
    Column {
        id: contents; x: Theme.dp(6); y: Theme.dp(6); width: parent.width-Theme.dp(12); spacing: Theme.dp(3)
        RowLayout {
            width: parent.width
            Label { text: lane.title; visible: text.length>0; color: Theme.ink; font.bold: true; font.pointSize: Theme.caption; Layout.fillWidth: true }
            Label { Layout.fillWidth: !lane.title.length; horizontalAlignment: Text.AlignRight; text: lane.athletes.length.toString(); color: Theme.muted; font.pointSize: Theme.caption }
            SButton { text: "+"; quiet: true; enabled: lane.canAdd; Layout.preferredWidth: Theme.dp(30); Accessible.name: qsTr("Dodaj wybranego skoczka"); onClicked: lane.addClicked() }
        }
        Repeater {
            model: lane.athletes
            delegate: Column {
                required property var modelData
                required property int index
                width: contents.width
                Label { visible: lane.grouped && (index===0 || lane.athletes[index-1].countryCode!==modelData.countryCode); text: modelData.countryCode; color: Theme.muted; font.pointSize: Theme.caption; topPadding: Theme.dp(4) }
                RosterBoardAthlete { width: parent.width; athlete: modelData; removable: true; onRemoved: lane.removed(modelData.athleteId) }
            }
        }
        Label { visible: lane.athletes.length===0; text: qsTr("Przeciągnij tutaj skoczków"); color: Theme.muted; font.pointSize: Theme.caption; padding: Theme.dp(6) }
    }
    DropArea {
        id: drop; anchors.fill: parent; keys: ["roster-board"]
        onEntered: drag.accepted=!lane.country || drag.source.athlete.countryCode===lane.country
        onPositionChanged: lane.dragPosition(drag.y)
        onExited: lane.dragLeft()
        onDropped: {lane.dragLeft();var id=drop.source.athlete.athleteId;Qt.callLater(function(){lane.received(id)});drop.acceptProposedAction()}
    }
}
