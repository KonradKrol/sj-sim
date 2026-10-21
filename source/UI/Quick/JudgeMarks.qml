import QtQuick 2.15
import QtQuick.Controls 2.15

Row {
    id: marks
    property var judges: []
    spacing: 4
    Repeater {
        model: marks.judges
        Rectangle {
            width: number.implicitWidth+10; height: number.implicitHeight+6
            color: modelData.discarded ? "transparent" : Theme.raised
            radius: 4
            Accessible.role: Accessible.StaticText
            Accessible.name: modelData.text + (modelData.discarded ? qsTr(" · nota odrzucona") : "")
            Text {
                id: number
                Accessible.ignored: true
                anchors.centerIn: parent
                text: modelData.text
                color: modelData.discarded ? Theme.muted : Theme.ink
                font.family: Theme.mono; font.pointSize: Theme.caption
                font.strikeout: modelData.discarded
            }
        }
    }
}
