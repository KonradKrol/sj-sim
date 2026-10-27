import QtQuick

Item {
    width: Theme.dp(12); height: Theme.dp(8)
    property color tint: Theme.muted
    Accessible.ignored: true
    Rectangle { width: parent.width*.6; height: Theme.dp(1.6); x: 0; y: parent.height*.4; color: parent.tint; rotation: 45 }
    Rectangle { width: parent.width*.6; height: Theme.dp(1.6); x: parent.width*.4; y: parent.height*.4; color: parent.tint; rotation: -45 }
}
