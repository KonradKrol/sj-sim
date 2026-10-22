import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

SButton {
    id: social
    property string platform
    property url destination
    implicitWidth: content.implicitWidth + 28
    implicitHeight: Math.max(42,content.implicitHeight + 16)
    hint: destination.toString()
    onClicked: Qt.openUrlExternally(destination)
    contentItem: RowLayout {
        id: content
        spacing: 8
        Image {
            objectName: "socialIcon-" + social.platform
            source: Qt.resolvedUrl("icons/"+social.platform+(Theme.dark ? "-dark.svg" : "-light.svg"))
            sourceSize.width: 24; sourceSize.height: 24
            Layout.preferredWidth: 20; Layout.preferredHeight: 20
            Accessible.ignored: true
        }
        Label { text: social.text; font: social.font; color: social.foreground }
    }
}
