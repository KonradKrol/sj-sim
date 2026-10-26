import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: screen
    objectName: "athleteConceptPatches"
    property var backend
    AthleteDesign { id: design; variant: 1 }
    padding: Theme.dp(18)
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    background: Rectangle { color: Theme.background }
    contentItem: ColumnLayout {
        spacing: Theme.dp(18)
        RowLayout {
            Layout.fillWidth: true
            Label { text: qsTr("Zawodnik"); font.family: Theme.displayFamily; font.pointSize: Theme.title; font.weight: Font.Black; color: design.ink }
            Rectangle { Layout.fillWidth: true; height: 1; color: design.line }
            Image { source: screen.backend.profile.flag || ""; Layout.preferredWidth: Theme.dp(35); Layout.preferredHeight: Theme.dp(22); sourceSize.width: 70; sourceSize.height: 44; fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
        }
        AthleteIdentity { backend: screen.backend; design: design; Layout.fillWidth: true }
        GridLayout {
            Layout.fillWidth: true; columns: width>=Theme.dp(800) ? 2 : 1; columnSpacing: Theme.dp(35); rowSpacing: Theme.dp(18)
            ColumnLayout {
                Layout.fillWidth: true; Layout.preferredWidth: Theme.dp(330); Layout.maximumWidth: parent.columns===2 ? Theme.dp(370) : Infinity; Layout.alignment: Qt.AlignTop; spacing: Theme.dp(14)
                GridLayout {
                    columns: 2; Layout.fillWidth: true; columnSpacing: Theme.dp(18)
                    AthleteRating { backend: screen.backend; design: design; compact: true; Layout.fillWidth: true; Layout.preferredWidth: Theme.dp(170) }
                    AthleteRating { backend: screen.backend; design: design; form: true; compact: true; Layout.fillWidth: true; Layout.preferredWidth: Theme.dp(140); Layout.alignment: Qt.AlignBottom }
                }
                Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }
                Image { visible: !!screen.backend.profile.photo; source: screen.backend.profile.photo || ""; sourceSize.width: 480; sourceSize.height: 500; Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: 0; Layout.preferredHeight: Theme.dp(120); fillMode: Image.PreserveAspectFit; asynchronous: true; Accessible.name: qsTr("Zdjęcie skoczka") }
                AthleteLanding { backend: screen.backend; design: design; Layout.fillWidth: true }
                AthletePatchSet { backend: screen.backend; Layout.fillWidth: true }
            }
            AthletePatchRack { backend: screen.backend; design: design; Layout.fillWidth: true; Layout.preferredWidth: Theme.dp(680); Layout.minimumWidth: 0; Layout.alignment: Qt.AlignTop }
        }
    }
}
