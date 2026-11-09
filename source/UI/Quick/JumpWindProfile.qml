import QtQuick 2.15
import QtQuick.Controls 2.15

Popup {
    id: profile
    enter: PopupFade {}
    exit: PopupFade { leaving: true }
    objectName: "jumpWindProfile"
    property var recorded: ({})
    property var forecastSamples: ui.context.windPreview || []
    property string athlete
    property bool forecast: false
    property point panelPosition: Qt.point(-1,-1)
    signal moved(real panelX,real panelY)
    function movePanel(dx,dy){
        x=Math.max(0,Math.min(parent.width-width,x+dx))
        y=Math.max(0,Math.min(parent.height-height,y+dy))
        moved(x,y)
    }
    Connections {
        target: profile.parent
        function onWidthChanged(){if(profile.opened)profile.movePanel(0,0)}
        function onHeightChanged(){if(profile.opened)profile.movePanel(0,0)}
    }
    parent: Overlay.overlay
    width: Math.min(Theme.dp(580),parent.width-24)
    height: Math.min(contentItem.implicitHeight+topPadding+bottomPadding,parent.height-24)
    x: panelPosition.x>=0 ? Math.max(0,Math.min(parent.width-width,panelPosition.x)) : (parent.width-width)/2
    y: panelPosition.y>=0 ? Math.max(0,Math.min(parent.height-height,panelPosition.y)) : (parent.height-height)/2
    modal: false; dim: false; focus: !forecast; padding: Theme.dp(12)
    closePolicy: forecast ? Popup.CloseOnEscape : Popup.CloseOnEscape | Popup.CloseOnPressOutside
    background: Rectangle { color: Theme.surface; border.color: Theme.outline; radius: Theme.radius }
    contentItem: WindSectorsCompact {
        framed: false
        draggable: profile.forecast
        onMoveRequested: function(dx,dy){
            profile.movePanel(dx,dy)
        }
        forecast: profile.forecast
        recorded: profile.recorded
        forecastSamples: profile.forecastSamples
        forecastWind: Number(ui.context.windValue || 0)
        onDismissed: profile.close()
    }
}
