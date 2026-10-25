import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    implicitHeight: release.implicitHeight + (Theme.compact ? 4 : 8)
    color: Theme.surface
    Rectangle { width: parent.width; height: 1; color: Theme.divider }
    RowLayout {
        anchors.fill: parent; anchors.leftMargin: Theme.compact ? 8 : 16; anchors.rightMargin: Theme.compact ? 8 : 16
        VersionBadge { id: release }
        Label { objectName: "jumpSeriesShortcutHint"; visible: !!navigation && !!navigation.current && navigation.current.profile === "CompetitionManagerWindow"; text: qsTr("Shift+klik · skok z wybranej serii"); color: Theme.muted; font.pointSize: Theme.caption; Layout.minimumWidth: 0; Layout.fillWidth: true; Layout.preferredWidth: 0; horizontalAlignment: Text.AlignRight; elide: Text.ElideRight }
    }
}
