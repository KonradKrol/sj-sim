import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: card
    property var backend
    property bool compact: false
    readonly property var athlete: backend.profile
    padding: Theme.dp(16)
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    background: Rectangle { color: Theme.stadium; radius: Theme.dp(12); border.width: 1; border.color: "#3b5267" }
    contentItem: ColumnLayout {
        spacing: Theme.dp(14)
        RowLayout {
            Label { text: "SJ.SIM"; color: Theme.headerMuted; font.family: Theme.displayFamily; font.pointSize: Theme.section }
            Item { Layout.fillWidth: true }
            Label { text: "OVR"; color: Theme.signal; font.family: Theme.mono; font.pointSize: Theme.caption }
            Label { text: card.athlete.overall || 70; color: Theme.signal; font.family: Theme.displayFamily; font.pointSize: Theme.fontSize(42); font.weight: Font.Bold }
        }
        AthletePortrait { Layout.fillWidth: true; profile: card.athlete; compact: card.compact }
        ColumnLayout {
            Layout.fillWidth: true; spacing: 0
            Label { text: card.athlete.name || qsTr("Nowy"); color: Theme.headerMuted; font.pointSize: Theme.section; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            Label { text: card.athlete.surname || qsTr("skoczek"); color: Theme.snow; font.family: Theme.displayFamily; font.pointSize: Theme.fontSize(38); font.weight: Font.Bold; Layout.fillWidth: true; wrapMode: Text.WrapAnywhere }
        }
        Rectangle { Layout.fillWidth: true; height: 1; color: "#3b5267" }
        RowLayout {
            Layout.fillWidth: true
            ColumnLayout { spacing: 2; Label { text: qsTr("REKORD"); color: Theme.headerMuted; font.family: Theme.mono; font.pointSize: Theme.caption } Label { text: card.athlete.personalBest>0 ? qsTr("%1 m").arg(Number(card.athlete.personalBest).toLocaleString(Qt.locale(),'f',1)) : "—"; color: Theme.snow; font.family: Theme.displayFamily; font.pointSize: Theme.section } }
            Item { Layout.fillWidth: true }
            ColumnLayout { spacing: 2; Label { text: qsTr("FORMA"); color: Theme.headerMuted; font.family: Theme.mono; font.pointSize: Theme.caption } Label { text: card.athlete.form || 70; color: Theme.snow; font.family: Theme.displayFamily; font.pointSize: Theme.section } }
        }
        Row {
            Layout.fillWidth: true; spacing: Theme.dp(5)
            Repeater {
                model: card.athlete.perks || []
                Rectangle {
                    required property int modelData
                    required property int index
                    visible: modelData!==0
                    width: Theme.dp(40); height: Theme.dp(34); radius: Theme.dp(5); color: "#29465c"; border.color: Math.abs(modelData)===2 ? "#f5cf84" : "#66869d"
                    Image { anchors.centerIn: parent; width: Theme.dp(25); height: width; source: "icons/athlete/"+[["training","reserve"],["flight","small"],["steady","volatile"]][parent.index][parent.modelData>0?0:1]+".svg"; sourceSize.width: 50; sourceSize.height: 50; Accessible.ignored: true }
                    Label { visible: Math.abs(parent.modelData)===2; anchors.right: parent.right; anchors.top: parent.top; text: "II"; color: "#f5cf84"; font.family: Theme.mono; font.pointSize: Theme.caption }
                }
            }
        }
    }
}
