import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Popup {
    id: popup
    parent: Overlay.overlay
    modal: true; focus: true
    width: Math.min(Theme.dp(420),parent ? parent.width-24 : 340)
    height: Math.min(implicitHeight,parent ? parent.height-24 : implicitHeight)
    x: parent ? (parent.width-width)/2 : 0
    y: parent ? Math.max(12,(parent.height-height)/2) : 0
    padding: Theme.dp(16)
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    enter: PopupFade {}
    exit: PopupFade { leaving: true }
    readonly property var liveControl: ui.find("checkBox_liveCompetition")
    readonly property bool progressive: !!liveControl && !!liveControl.values.value
    readonly property var waits: [1,3,5,10,15,30]
    readonly property var flights: [3,5,7,10]
    onOpened: ui.setLiveConfigurationOpen(true)
    onClosed: ui.setLiveConfigurationOpen(false)
    background: Surface { fillColor: Theme.surface; edgeColor: Theme.outline; strokeWidth: 1 }
    contentItem: ScrollView {
        id: scroll
        contentWidth: availableWidth; implicitHeight: content.implicitHeight
        clip: true
        ColumnLayout {
            id: content
            width: scroll.availableWidth; spacing: Theme.dp(10)
            Label { text: qsTr("Relacja i automatyczne skoki"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            CheckHelp {
                Layout.fillWidth: true; controlName: "raceProgressiveReveal"
                text: qsTr("Pokazuj skok stopniowo")
                checked: popup.progressive
                editable: !!popup.liveControl && popup.liveControl.values.enabled
                explanation: qsTr("Wstępny wiatr, odległość i lądowanie, noty, a potem końcowy wiatr i wynik. Ostatnia sekunda jest stała. Bez not relacja jest krótsza.")
                onClicked: ui.activate(popup.liveControl.values.id)
            }
            Label { text: qsTr("Czas relacji"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true }
            SChoice {
                objectName: "raceFlightDuration"; Layout.fillWidth: true
                enabled: popup.progressive
                model: popup.flights.map(function(value){return qsTr("%1 s").arg(value)})
                displayText: qsTr("%1 s").arg(ui.context.liveFlightSeconds || 7)
                currentIndex: popup.flights.indexOf(Number(ui.context.liveFlightSeconds || 7))
                Accessible.name: qsTr("Czas całej relacji")
                onActivated: ui.setLivePreference("flightSeconds",popup.flights[index])
            }
            Rectangle { Layout.fillWidth: true; implicitHeight: 1; color: Theme.divider }
            CheckHelp {
                Layout.fillWidth: true; controlName: "raceAutomaticPlayback"
                text: qsTr("Oddawaj skoki automatycznie")
                checked: !!ui.context.automaticPlayback
                editable: !ui.context.complete && !ui.context.roundComplete
                explanation: qsTr("Kolejni zawodnicy skaczą bez klikania. Automat zatrzymuje się na końcu grupy lub serii; przechodzisz dalej ręcznie.")
                onClicked: ui.setAutomaticPlayback(value)
            }
            Label { text: qsTr("Przerwa przed następnym skokiem"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true }
            SChoice {
                objectName: "raceWaitDuration"; Layout.fillWidth: true
                model: popup.waits.map(function(value){return qsTr("%1 s").arg(value)})
                displayText: qsTr("%1 s").arg(ui.context.liveWaitSeconds || 5)
                currentIndex: popup.waits.indexOf(Number(ui.context.liveWaitSeconds || 5))
                Accessible.name: qsTr("Przerwa przed następnym skokiem")
                onActivated: ui.setLivePreference("waitSeconds",popup.waits[index])
            }
            SButton { text: qsTr("Gotowe"); Layout.fillWidth: true; onClicked: popup.close() }
        }
    }
}
