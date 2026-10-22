import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: footer
    readonly property var action: ui.find("pushButton_competitionConfig")
    implicitHeight: content.implicitHeight+topPadding+bottomPadding
    padding: 8
    background: Rectangle { color: Theme.surface; border.color: Theme.divider }
    contentItem: GridLayout {
        id: content
        columns: footer.width<600 ? 1 : 2; rowSpacing: 4; columnSpacing: 16
        RowLayout {
            Layout.fillWidth: true; spacing: 10
            Image { visible: !!ui.context.nextHill; source: ui.context.nextFlag || ""; sourceSize.width: 64; sourceSize.height: 48; Layout.preferredWidth: 32; Layout.preferredHeight: 24; fillMode: Image.PreserveAspectFit }
            ColumnLayout {
                Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: 0
                Label { text: ui.context.nextHill || qsTr("Brak kolejnego wydarzenia"); color: Theme.ink; font.pointSize: Theme.body; font.weight: Font.DemiBold; Layout.fillWidth: true; elide: Text.ElideRight }
                Label { visible: !!ui.context.nextHill; text: (ui.context.nextType || "")+" · HS"+ui.context.nextHS+" · "+(ui.context.nextTeam?qsTr("Drużynowy"):qsTr("Indywidualny")); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; elide: Text.ElideRight }
            }
        }
        SButton { objectName: "seasonNextAction"; text: footer.action ? footer.action.values.text : ""; enabled: !!footer.action && footer.action.values.enabled; hint: footer.action ? footer.action.values.tip || "" : ""; primary: true; font.pointSize: Theme.body; implicitHeight: Math.max(40,contentItem.implicitHeight+12); Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.maximumWidth: footer.width<600 ? Infinity : 300; onClicked: if(footer.action)ui.activate(footer.action.values.id) }
    }
}
