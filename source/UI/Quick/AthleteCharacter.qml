import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: screen
    objectName: "athleteConceptCharacter"
    property var backend
    AthleteDesign { id: conceptPalette; variant: 4 }
    padding: Theme.dp(24)
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    background: Rectangle { color: conceptPalette.paper; radius: Theme.dp(5) }
    contentItem: ColumnLayout {
        spacing: Theme.dp(20)
        AthleteIdentity { backend: screen.backend; design: conceptPalette; Layout.fillWidth: true }
        GridLayout {
            Layout.fillWidth: true; columns: width>=Theme.dp(770) ? 2 : 1; columnSpacing: Theme.dp(34); rowSpacing: Theme.dp(15)
            ColumnLayout {
                Layout.fillWidth: true; Layout.preferredWidth: Theme.dp(300); Layout.maximumWidth: parent.columns===2 ? Theme.dp(350) : Infinity; Layout.alignment: Qt.AlignVCenter; spacing: Theme.dp(8)
                Item {
                    Layout.fillWidth: true; Layout.preferredHeight: Theme.dp(240)
                    Rectangle {
                        anchors.centerIn: parent; width: Theme.dp(220); height: width; radius: width/2; color: "transparent"; border.width: Theme.dp(1); border.color: conceptPalette.line
                        Rectangle { anchors.fill: parent; anchors.margins: Theme.dp(12); radius: width/2; color: conceptPalette.surface; border.width: Theme.dp(3); border.color: conceptPalette.accent }
                        Image { visible: portrait.status!==Image.Ready; anchors.centerIn: parent; width: Theme.dp(155); height: Theme.dp(226); source: "icons/athlete/athlete.svg"; sourceSize.width: 310; sourceSize.height: 452; Accessible.ignored: true }
                        Image { id: portrait; visible: !!screen.backend.profile.photo; anchors.fill: parent; anchors.margins: Theme.dp(28); source: screen.backend.profile.photo || ""; sourceSize.width: 400; sourceSize.height: 400; fillMode: Image.PreserveAspectFit; asynchronous: true; Accessible.name: qsTr("Zdjęcie skoczka") }
                    }
                }
                Label { text: screen.backend.profile.label || ""; color: conceptPalette.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; Layout.fillWidth: true; horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap }
                AthleteInsignia { backend: screen.backend; family: "coin"; badgeSize: Theme.dp(50); Layout.alignment: Qt.AlignHCenter; Layout.preferredWidth: Theme.dp(156) }
            }
            ColumnLayout {
                Layout.fillWidth: true; Layout.preferredWidth: Theme.dp(660); Layout.minimumWidth: 0; Layout.alignment: Qt.AlignTop; spacing: Theme.dp(16)
                GridLayout {
                    Layout.fillWidth: true; columns: 2; columnSpacing: Theme.dp(22)
                    AthleteRating { backend: screen.backend; design: conceptPalette; Layout.fillWidth: true }
                    AthleteRating { backend: screen.backend; design: conceptPalette; form: true; Layout.fillWidth: true; Layout.alignment: Qt.AlignBottom }
                }
                AthleteLanding { backend: screen.backend; design: conceptPalette; Layout.fillWidth: true }
            }
        }
        Rectangle { Layout.fillWidth: true; height: 1; color: conceptPalette.line }
        AthleteBadgeRack { backend: screen.backend; design: conceptPalette; family: "coin"; Layout.fillWidth: true }
    }
}
