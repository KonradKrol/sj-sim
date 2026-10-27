import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: screen
    objectName: "athleteConceptMedals"
    property var backend
    AthleteDesign { id: conceptPalette; variant: 0 }
    padding: Theme.dp(24)
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    background: Rectangle { color: conceptPalette.paper; Rectangle { width: Theme.dp(6); height: parent.height; color: conceptPalette.accent } }
    contentItem: ColumnLayout {
        spacing: Theme.dp(20)
        AthleteIdentity { backend: screen.backend; design: conceptPalette; Layout.fillWidth: true }
        Rectangle { Layout.fillWidth: true; height: 1; color: conceptPalette.line }
        GridLayout {
            Layout.fillWidth: true; columns: width>=Theme.dp(850) ? 2 : 1; columnSpacing: Theme.dp(32); rowSpacing: Theme.dp(20)
            ColumnLayout {
                Layout.fillWidth: true; Layout.preferredWidth: Theme.dp(210); Layout.maximumWidth: parent.columns===2 ? Theme.dp(260) : Infinity; Layout.alignment: Qt.AlignTop; spacing: Theme.dp(14)
                AthleteRating { backend: screen.backend; design: conceptPalette; Layout.fillWidth: true }
                AthleteRating { backend: screen.backend; design: conceptPalette; form: true; compact: true; Layout.fillWidth: true }
                Image { visible: !!screen.backend.profile.photo; source: screen.backend.profile.photo || ""; sourceSize.width: 480; sourceSize.height: 500; Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: 0; Layout.preferredHeight: Theme.dp(120); fillMode: Image.PreserveAspectFit; asynchronous: true; Accessible.name: qsTr("Zdjęcie skoczka") }
                AthleteInsignia { backend: screen.backend; Layout.fillWidth: true; badgeSize: Theme.dp(52) }
            }
            ColumnLayout {
                Layout.fillWidth: true; Layout.preferredWidth: Theme.dp(760); Layout.minimumWidth: 0; Layout.alignment: Qt.AlignTop; spacing: Theme.dp(18)
                AthleteBadgeRack { backend: screen.backend; design: conceptPalette; Layout.fillWidth: true }
                Rectangle { Layout.fillWidth: true; height: 1; color: conceptPalette.line }
                AthleteEquipped { backend: screen.backend; design: conceptPalette; Layout.fillWidth: true }
                AthleteLanding { backend: screen.backend; design: conceptPalette; Layout.fillWidth: true }
            }
        }
    }
}
