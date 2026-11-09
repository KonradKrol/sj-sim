import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: group
    default property alias buttons: row.data
    implicitWidth: row.implicitWidth+leftPadding+rightPadding
    implicitHeight: row.implicitHeight+topPadding+bottomPadding
    padding: Theme.dp(2)
    background: Item {}
    contentItem: RowLayout { id: row; spacing: Theme.dp(7) }
}
