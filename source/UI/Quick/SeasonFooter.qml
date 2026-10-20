import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: footer
    readonly property var nextNode: ui.find("label_nextCompetitionIndexAndHill")
    padding: 12
    background: Rectangle { color: Theme.surface; border.color: Theme.divider }
    contentItem: GridLayout {
        columns: footer.width < 600 ? 1 : 2
        rowSpacing: 6
        Label { text: footer.nextNode ? footer.nextNode.values.text : ""; color: Theme.ink; font.pointSize: Theme.body; font.weight: Font.DemiBold; Layout.fillWidth: true; elide: Text.ElideRight }
        Node { objectName: "seasonNextAction"; node: ui.find("pushButton_competitionConfig"); Layout.fillWidth: true; Layout.maximumWidth: footer.width < 600 ? 10000 : 300; Layout.preferredHeight: implicitHeight }
    }
}
