import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: group
    default property alias buttons: row.data
    implicitWidth: row.implicitWidth+leftPadding+rightPadding
    implicitHeight: row.implicitHeight+topPadding+bottomPadding
    padding: Theme.dp(4)
    background: Rectangle { color: Theme.surface; radius: Theme.dp(6); border.color: Theme.divider }
    contentItem: RowLayout { id: row; spacing: Theme.dp(4) }
}
