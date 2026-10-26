import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: portrait
    property var profile
    property bool compact: false
    readonly property string initials: ((profile.name || "").slice(0,1)+(profile.surname || "").slice(0,1)).toLocaleUpperCase() || "SJ"
    color: Theme.stadium; radius: Theme.dp(10); clip: true
    implicitHeight: Theme.dp(compact ? 145 : 245)
    Image {
        anchors.fill: parent; source: "icons/athlete/terrain.svg"; sourceSize.width: 720; sourceSize.height: 480; fillMode: Image.PreserveAspectCrop; opacity: .7; Accessible.ignored: true
    }
    Label {
        visible: photo.status!==Image.Ready
        anchors.centerIn: parent; text: portrait.initials; font.family: Theme.displayFamily; font.pointSize: Math.max(36,Math.min(100,portrait.width*.32)); font.weight: Font.Black; color: Theme.snow
    }
    Image {
        id: photo
        anchors.fill: parent; source: portrait.profile.photo || ""; sourceSize.width: 900; sourceSize.height: 1000
        fillMode: Image.PreserveAspectFit; asynchronous: true
        Accessible.name: qsTr("Zdjęcie skoczka"); Accessible.ignored: status!==Image.Ready
    }
    Rectangle { anchors.left: parent.left; anchors.top: parent.top; width: Theme.dp(5); height: parent.height; color: Theme.signal }
    Row {
        anchors.right: parent.right; anchors.bottom: parent.bottom; anchors.margins: Theme.dp(10); spacing: Theme.dp(7)
        Image { width: Theme.dp(30); height: Theme.dp(20); source: portrait.profile.flag || ""; sourceSize.width: 60; sourceSize.height: 40; fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
        Label { text: portrait.profile.country || ""; color: Theme.snow; font.family: Theme.mono; font.pointSize: Theme.caption }
    }
}
