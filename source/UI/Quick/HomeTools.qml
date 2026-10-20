import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: tools
    property bool compact: false
    color: Theme.surface; radius: Theme.radius; border.color: Theme.outline
    implicitHeight: (compact ? 174 : 76) * Theme.scale
    Rectangle { anchors.left: parent.left; anchors.top: parent.top; anchors.bottom: parent.bottom; width: 3; color: Theme.accent }
    GridLayout {
        id: content
        anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; anchors.margins: 16
        columns: tools.compact ? 1 : 3
        columnSpacing: 24; rowSpacing: 12
        ColumnLayout {
            id: workshop
            Layout.fillWidth: true; spacing: 4
            Label { text: qsTr("Baza danych"); color: Theme.ink; font.pointSize: Theme.body; font.weight: Font.DemiBold }
            Label { text: qsTr("%1 zawodników · %2 skoczni").arg(ui.library.jumpers).arg(ui.library.hills); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        }
        SButton { id: database; Layout.preferredHeight: 44 * Theme.scale; text: qsTr("Edytuj bazę danych"); Layout.fillWidth: true; onClicked: ui.launch("database") }
        SButton { id: settings; Layout.preferredHeight: 44 * Theme.scale; text: qsTr("Ustawienia"); Layout.fillWidth: true; onClicked: ui.launch("settings") }
    }
}
