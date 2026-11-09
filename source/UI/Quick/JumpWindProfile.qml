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
    parent: Overlay.overlay
    width: Math.min(Theme.dp(580),parent.width-24)
    height: Math.min(contentItem.implicitHeight+topPadding+bottomPadding,parent.height-24)
    x: (parent.width-width)/2; y: (parent.height-height)/2
    modal: false; dim: false; focus: true; padding: Theme.dp(12)
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    background: Rectangle { color: Theme.surface; border.color: Theme.outline; radius: Theme.radius }
    contentItem: WindSectorsCompact {
        framed: false
        forecast: profile.forecast
        recorded: profile.recorded
        forecastSamples: profile.forecastSamples
        forecastWind: Number(ui.context.windValue || 0)
        onDismissed: profile.close()
    }
}
