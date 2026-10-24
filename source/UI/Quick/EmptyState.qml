import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: state
    property bool compact: false
    property string title
    property string description
    property string actionObjectName
    property string actionText
    signal triggered()
    Accessible.role: Accessible.StaticText
    Accessible.name: title+". "+description
    implicitHeight: content.implicitHeight + 32
    Column {
        id: content
        anchors.centerIn: parent
        width: Math.max(0,Math.min(480,parent.width-24)); spacing: state.compact ? 6 : 10
        Rectangle { visible: !state.compact; width: 32; height: 4; radius: 2; color: Theme.accent }
        Label { visible: !state.compact || !state.actionText.length; text: state.title; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: state.compact ? Theme.body : Theme.section; width: parent.width; wrapMode: Text.WordWrap }
        Label { visible: text.length>0 && !state.compact; text: state.description; color: Theme.muted; font.pointSize: Theme.caption; width: parent.width; wrapMode: Text.WordWrap }
        SButton { objectName: state.actionObjectName; visible: state.actionText.length>0; text: state.actionText; primary: true; width: Math.min(parent.width,implicitWidth); onClicked: state.triggered() }
    }
}
