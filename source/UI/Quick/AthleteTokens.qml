import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: screen
    objectName: "athleteConceptTokens"
    property var backend
    AthleteDesign { id: design; variant: 2 }
    padding: Theme.dp(22)
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    background: Rectangle { color: design.paper; radius: Theme.dp(22) }
    contentItem: ColumnLayout {
        spacing: Theme.dp(18)
        AthleteIdentity { backend: screen.backend; design: design; Layout.fillWidth: true }
        AthleteEquipped { backend: screen.backend; design: design; family: "coin"; framed: true; Layout.fillWidth: true }
        Rectangle { Layout.fillWidth: true; height: 1; color: design.line }
        GridLayout {
            Layout.fillWidth: true; columns: width>=Theme.dp(880) ? 2 : 1; columnSpacing: Theme.dp(30); rowSpacing: Theme.dp(20)
            ColumnLayout {
                Layout.fillWidth: true; Layout.preferredWidth: Theme.dp(300); Layout.maximumWidth: parent.columns===2 ? Theme.dp(350) : Infinity; Layout.alignment: Qt.AlignTop; spacing: Theme.dp(14)
                GridLayout {
                    columns: 2; Layout.fillWidth: true; columnSpacing: Theme.dp(20)
                    AthleteRating { backend: screen.backend; design: design; compact: true; Layout.fillWidth: true }
                    AthleteRating { backend: screen.backend; design: design; form: true; compact: true; Layout.fillWidth: true }
                }
                AthleteLanding { backend: screen.backend; design: design; Layout.fillWidth: true }
                Image { visible: !!screen.backend.profile.photo; source: screen.backend.profile.photo || ""; sourceSize.width: 480; sourceSize.height: 480; Layout.fillWidth: true; Layout.preferredHeight: Theme.dp(120); fillMode: Image.PreserveAspectFit; asynchronous: true; Accessible.name: qsTr("Zdjęcie skoczka") }
            }
            AthleteBadgeRack { backend: screen.backend; design: design; family: "coin"; Layout.fillWidth: true; Layout.preferredWidth: Theme.dp(700); Layout.alignment: Qt.AlignTop; Layout.minimumWidth: 0 }
        }
    }
}
