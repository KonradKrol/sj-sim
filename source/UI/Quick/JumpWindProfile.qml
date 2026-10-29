import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Popup {
    enter: PopupFade {}
    exit: PopupFade { leaving: true }
    id: profile
    objectName: "jumpWindProfile"
    property var recorded: ({})
    property string athlete
    property bool forecast: false
    property int phase: 1
    readonly property var phaseKeys: ["before","during","after"]
    parent: Overlay.overlay
    width: Math.min(660,parent.width-24)
    height: Math.min(Theme.dp(forecast ? 240 : 300)+Theme.bodyMetrics.height*5,parent.height-24)
    x: (parent.width-width)/2; y: (parent.height-height)/2
    modal: false; dim: false; focus: true; padding: Theme.dp(12)
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    background: Rectangle { color: Theme.surface; border.color: Theme.accent; border.width: 2; radius: Theme.dp(6) }
    contentItem: ColumnLayout {
        spacing: Theme.dp(8)
        RowLayout {
            Layout.fillWidth: true
            ColumnLayout {
                Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: 1
                Label { text: profile.forecast ? qsTr("Wiatr przed skokiem") : qsTr("Wiatr na zeskoku"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.bold: true; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: profile.athlete; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; elide: Text.ElideRight }
            }
            SButton { text: "×"; quiet: true; Accessible.name: qsTr("Zamknij podgląd wiatru"); onClicked: profile.close() }
        }
        SectionTabs {
            objectName: "jumpWindPhase"
            visible: !profile.forecast
            Layout.fillWidth: true
            options: [qsTr("Przed"),qsTr("W locie"),qsTr("Po")]; currentIndex: profile.phase
            onSelected: profile.phase=index
        }
        ScrollView {
            id: scroll
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
            contentWidth: availableWidth; clip: true
            ColumnLayout {
                width: scroll.availableWidth; spacing: Theme.dp(8)
                WindSlopePreview {
                    objectName: "recordedWindSlope"
                    Layout.fillWidth: true
                    interactive: false; heading: profile.forecast ? qsTr("Teraz") : qsTr("Zapisany wiatr")
                    samples: profile.recorded[profile.forecast ? "before" : profile.phaseKeys[profile.phase]] || []
                }
                Label {
                    text: profile.forecast ? qsTr("Przybliżony układ wiatru. Warunki mogą zmienić się w trakcie skoku.") : profile.phase===1 ? qsTr("Średnia na przebytym odcinku. — oznacza odcinek poza skokiem.") : profile.phase===0 ? qsTr("Pole wiatru tuż przed wybiciem.") : qsTr("Pole wiatru w chwili zakończenia skoku.")
                    color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true
                }
            }
        }
    }
}
