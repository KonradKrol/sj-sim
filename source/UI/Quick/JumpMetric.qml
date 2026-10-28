import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: metric
    property string caption
    property string value: "—"
    property color valueColor: Theme.ink
    property bool inlineValue: false
    property bool interactive: false
    signal clicked()
    ChangeFeedback { id: valueFeedback; target: metric; trigger: metric.value }
    implicitWidth: inlineValue ? inlineTitle.implicitWidth+inlineNumber.implicitWidth+6 : Math.max(title.implicitWidth,number.implicitWidth)
    implicitHeight: inlineValue ? Math.max(inlineTitle.implicitHeight,inlineNumber.implicitHeight) : stacked.implicitHeight
    Accessible.role: interactive ? Accessible.Button : Accessible.StaticText
    Accessible.name: caption + ": " + value
    Accessible.onPressAction: if(interactive)clicked()
    activeFocusOnTab: interactive
    Keys.onReturnPressed: if(interactive)clicked()
    Keys.onSpacePressed: if(interactive)clicked()
    Rectangle { anchors.fill: parent; visible: metric.interactive && (pointer.containsMouse || metric.activeFocus); color: Theme.selection; radius: 3 }
    MouseArea {
        id: pointer
        anchors.fill: parent; enabled: metric.interactive; hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: {metric.forceActiveFocus();metric.clicked()}
    }
    Column {
        id: stacked
        visible: !metric.inlineValue
        width: parent.width
        Label {
            id: title
            width: parent.width
            text: metric.caption; color: Theme.muted; font.pointSize: Theme.caption-1
            wrapMode: Text.WordWrap; Accessible.ignored: true
        }
        Label {
            id: number
            opacity: 0.55+valueFeedback.progress*0.45
            width: parent.width
            text: metric.value; color: metric.valueColor
            font.family: Theme.mono; font.pointSize: Theme.caption
            wrapMode: Text.Wrap; Accessible.ignored: true
        }
    }
    Row {
        visible: metric.inlineValue
        width: parent.width; spacing: 6
        Label {
            id: inlineTitle
            width: implicitWidth
            text: metric.caption; color: Theme.muted; font.pointSize: Theme.caption-1
            wrapMode: Text.NoWrap; Accessible.ignored: true
        }
        Label {
            id: inlineNumber
            opacity: 0.55+valueFeedback.progress*0.45
            width: Math.max(0,parent.width-inlineTitle.width-6)
            text: metric.value; color: metric.valueColor
            font.family: Theme.mono; font.pointSize: Theme.caption
            wrapMode: Text.Wrap; Accessible.ignored: true
        }
    }
}
