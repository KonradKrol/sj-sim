import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

SButton {
    id: button
    property var event
    text: (event.number ? event.number+". " : "")+event.title
    Accessible.name: text+", "+(event.type || "")
    implicitHeight: Math.max(40,content.implicitHeight+12)
    implicitWidth: 220
    contentItem: RowLayout {
        id: content
        spacing: 6
        Image { visible: !!button.event.flag; source: button.event.flag || ""; sourceSize.width: 48; sourceSize.height: 32; Layout.preferredWidth: 22; Layout.preferredHeight: 16; fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
        Label { text: button.text; color: button.foreground; font: button.font; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight; verticalAlignment: Text.AlignVCenter }
        Label { visible: !!button.event.qualification; text: "Q"; color: button.primary ? button.foreground : Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption; Accessible.name: qsTr("Kwalifikacje") }
    }
}
