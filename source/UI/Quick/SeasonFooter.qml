import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Pane {
    id: footer
    objectName: "seasonFooter"
    implicitWidth: 0
    width: parent ? parent.width : 0
    readonly property bool shortScreen: Window.window && Window.window.height<600
    readonly property var action: ui.find("pushButton_competitionConfig")
    implicitHeight: Math.max(details.implicitHeight,nextAction.implicitHeight)+topPadding+bottomPadding
    padding: Theme.dp(4)
    background: Rectangle { color: Theme.surface; Rectangle { width: parent.width; height: 1; color: Theme.divider } }
    contentItem: Item {
    RowLayout {
        anchors.fill: parent
        id: content
        spacing: Theme.dp(12)
        RowLayout {
            id: details
            Layout.minimumWidth: 0
            Layout.fillWidth: true; spacing: 10
            Image { visible: !!ui.context.nextHill; source: ui.context.nextFlag || ""; sourceSize.width: 64; sourceSize.height: 48; Layout.preferredWidth: Theme.dp(28); Layout.preferredHeight: Theme.dp(20); fillMode: Image.PreserveAspectFit }
            ColumnLayout {
                Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: 0
                Label { text: ui.context.nextHill || qsTr("Brak kolejnego wydarzenia"); color: Theme.ink; font.pointSize: Theme.body; font.weight: Font.DemiBold; Layout.fillWidth: true; elide: Text.ElideRight }
                Label { visible: !!ui.context.nextHill; text: (ui.context.nextType || "")+" · HS"+ui.context.nextHS+" · "+(ui.context.nextTeam?qsTr("Drużynowy"):qsTr("Indywidualny")); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; elide: Text.ElideRight }
            }
        }
        SButton { id: nextAction; objectName: "seasonNextAction"; text: footer.action ? footer.action.values.text : ""; enabled: !!footer.action && footer.action.values.enabled; hint: footer.action ? footer.action.values.tip || "" : ""; primary: true; font.pointSize: Theme.body; implicitHeight: Math.max(Theme.compact ? 28 : 40,contentItem.implicitHeight+(Theme.compact ? 4 : 12)); Layout.fillWidth: footer.width<Theme.dp(440); Layout.minimumWidth: 0; Layout.maximumWidth: footer.width<Theme.dp(440) ? Infinity : Theme.dp(260); onClicked: if(footer.action)ui.activate(footer.action.values.id) }
    }
}
}
