import QtQuick 2.15
import QtQuick.Controls 2.15

QtObject {
    id: sizing
    required property Control control
    function schedule() { update.restart() }
    property Timer update: Timer {
        interval: 0
        onTriggered: if(sizing.control && sizing.control.contentItem)
            sizing.control.implicitHeight=Math.ceil(sizing.control.contentItem.implicitHeight+sizing.control.topPadding+sizing.control.bottomPadding)
    }
    property Connections content: Connections {
        target: sizing.control ? sizing.control.contentItem : null
        function onImplicitHeightChanged(){sizing.schedule()}
    }
    property Connections padding: Connections {
        target: sizing.control
        function onContentItemChanged(){sizing.schedule()}
        function onTopPaddingChanged(){sizing.schedule()}
        function onBottomPaddingChanged(){sizing.schedule()}
    }
    Component.onCompleted: schedule()
}
