import QtQuick

QtObject {
    id: entrance
    required property Item target
    property int delay: 0
    property real progress: 1
    function replay() {
        if(!target.visible || !Theme.motion)return
        animation.stop()
        progress=0
        animation.start()
    }
    Component.onCompleted: Qt.callLater(replay)
    property Connections visibility: Connections { target: entrance.target; function onVisibleChanged(){if(entrance.target.visible)entrance.replay()} }
    property Connections preferences: Connections { target: Theme; function onMotionChanged(){if(!Theme.motion){animation.stop();entrance.progress=1}} }
    property SequentialAnimation animation: SequentialAnimation {
        PauseAnimation { duration: Theme.motion ? entrance.delay : 0 }
        NumberAnimation { target: entrance; property: "progress"; to: 1; duration: Theme.motion*3; easing.type: Easing.OutCubic }
    }
}
