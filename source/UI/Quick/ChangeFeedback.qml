import QtQuick

QtObject {
    id: feedback
    required property Item target
    property var trigger
    property bool ready: false
    property int duration: Theme.motion
    property real progress: 1
    function restart() {
        if(!ready || !target.visible || !Theme.motion)return
        animation.stop()
        progress=0
        animation.start()
    }
    onTriggerChanged: restart()
    Component.onCompleted: ready=true
    property Connections preferences: Connections { target: Theme; function onMotionChanged(){if(!Theme.motion){animation.stop();feedback.progress=1}} }
    property Connections visibility: Connections { target: feedback.target; function onVisibleChanged(){if(!feedback.target.visible){animation.stop();feedback.progress=1}} }
    property NumberAnimation animation: NumberAnimation { target: feedback; property: "progress"; to: 1; duration: feedback.duration; easing.type: Easing.OutCubic }
}
