import QtQuick 2.15

QtObject {
    id: arrival
    required property Item target
    property bool expressive: false
    property Item emphasis: null
    property bool ready: false
    function settle() {
        motion.stop()
        if(target)target.opacity=1
        if(emphasis)emphasis.scale=1
    }
    function play() {
        if(!ready || !target || !target.visible || !Theme.motion)return
        motion.restart()
    }
    Component.onCompleted: {ready=true;Qt.callLater(play)}
    property Connections visibility: Connections {
        target: arrival.target
        function onVisibleChanged(){if(arrival.target && arrival.target.visible)arrival.play();else arrival.settle()}
    }
    property Connections preferences: Connections {
        target: Theme
        function onMotionChanged(){if(!Theme.motion)arrival.settle()}
    }
    property ParallelAnimation motion: ParallelAnimation {
        NumberAnimation { property: "opacity"; target: arrival.target; from: arrival.expressive ? .3 : .65; to: 1; duration: Theme.motion; easing.type: Easing.OutCubic }
        SequentialAnimation {
            NumberAnimation { property: "scale"; target: arrival.emphasis || arrival.target; from: arrival.expressive && arrival.emphasis ? .90 : 1; to: arrival.expressive && arrival.emphasis ? 1.025 : 1; duration: arrival.expressive && arrival.emphasis ? Theme.motion : 0; easing.type: Easing.OutCubic }
            NumberAnimation { property: "scale"; target: arrival.emphasis || arrival.target; to: 1; duration: arrival.expressive && arrival.emphasis ? Theme.motion/2 : 0; easing.type: Easing.OutSine }
        }
    }
}
