import QtQuick 2.15
import QtQuick.Controls 2.15

Row {
    id: marks
    property var judges: []
    readonly property string signature: judges.map(function(judge){return judge.text+":"+judge.discarded+":"+judge.revealed}).join("|")
    function synchronize() {
        if(!markModel)return
        while(markModel.count>judges.length)markModel.remove(markModel.count-1)
        for(var i=0;i<judges.length;++i) {
            var value={displayText:String(judges[i].text),discarded:!!judges[i].discarded,revealed:judges[i].revealed!==false}
            if(i>=markModel.count)markModel.append(value)
            else markModel.set(i,value)
        }
    }
    onSignatureChanged: synchronize()
    Component.onCompleted: synchronize()
    ListModel { id: markModel }
    FontMetrics { id: numberMetrics; font.family: Theme.mono; font.pointSize: Theme.caption }
    spacing: Theme.dp(4)
    Repeater {
        model: markModel
        Rectangle {
            required property string displayText
            required property bool discarded
            required property bool revealed
            opacity: revealed ? 1 : .35
            Behavior on opacity { NumberAnimation { duration: Theme.motion } }
            width: Math.max(number.implicitWidth,numberMetrics.advanceWidth("20,0"))+Theme.dp(10); height: number.implicitHeight+Theme.dp(6)
            color: discarded ? "transparent" : Theme.raised
            radius: Theme.controlRadius/2
            Accessible.role: Accessible.StaticText
            Accessible.name: revealed ? displayText+(discarded ? qsTr(" · nota odrzucona") : "") : qsTr("Oczekiwanie na notę")
            Text {
                id: number
                Accessible.ignored: true
                anchors.centerIn: parent
                text: parent.displayText
                color: parent.discarded ? Theme.muted : Theme.ink
                font.family: Theme.mono; font.pointSize: Theme.caption
                Behavior on color { ColorAnimation { duration: Theme.motion } }
            }
            Rectangle {
                x: (parent.width-number.implicitWidth)/2-Theme.dp(2)
                y: parent.height/2
                width: parent.discarded ? number.implicitWidth+Theme.dp(4) : 0
                height: 1
                color: Theme.muted
                Behavior on width { NumberAnimation { duration: Theme.motion; easing.type: Easing.OutCubic } }
                Accessible.ignored: true
            }
        }
    }
}
