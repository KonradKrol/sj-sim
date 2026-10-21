import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: metric
    property string caption
    property string value: "—"
    property color valueColor: Theme.ink
    property bool inlineValue: false
    implicitWidth: Math.max(title.implicitWidth,number.implicitWidth)
    implicitHeight: content.implicitHeight
    Accessible.role: Accessible.StaticText
    Accessible.name: caption + ": " + value
    Column {
        id: content
        width: parent.width; spacing: 0
        Label {
            id: title
            visible: !metric.inlineValue
            width: parent.width
            text: metric.caption
            color: Theme.muted; font.pointSize: Theme.caption-1
            wrapMode: Text.WordWrap
            Accessible.ignored: true
        }
        Label {
            id: number
            width: parent.width
            text: (metric.inlineValue ? metric.caption+" " : "")+metric.value
            color: metric.valueColor
            font.family: Theme.mono; font.pointSize: Theme.caption
            wrapMode: Text.Wrap
            Accessible.ignored: true
        }
    }
}
