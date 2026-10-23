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
        Item { Layout.fillWidth: true }
        Label { visible: navigation && navigation.canGoBack && parent.width > 650; text: qsTr("Alt+← · Wstecz"); color: Theme.muted; font.pointSize: Theme.caption }
    }
}
