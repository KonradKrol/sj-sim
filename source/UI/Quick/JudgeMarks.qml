import QtQuick 2.15
import QtQuick.Controls 2.15

Row {
    id: marks
    property var judges: []
    readonly property string signature: judges.map(function(judge){return judge.text+":"+judge.discarded}).join("|")
    property var shownJudges: []
    onSignatureChanged: shownJudges=judges.map(function(judge){return judge})
    ChangeFeedback { id: marksArrival; target: marks; trigger: marks.signature; duration: Theme.motion*2 }
    spacing: 4
    Repeater {
        model: marks.shownJudges
        Rectangle {
            opacity: Math.min(1,Math.max(0,(marksArrival.progress-index*0.1)*2))
            transform: Translate { y: (1-opacity)*Theme.dp(5) }
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
