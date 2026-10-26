import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: screen
    objectName: "athleteConceptCollector"
    property var backend
    AthleteDesign { id: design; variant: 3 }
    padding: Theme.dp(22)
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    background: Rectangle { color: design.paper }
    contentItem: ColumnLayout {
        spacing: Theme.dp(20)
        AthleteIdentity { backend: screen.backend; design: design; Layout.fillWidth: true }
        GridLayout {
            Layout.fillWidth: true; columns: width>=Theme.dp(780) ? 2 : 1; columnSpacing: Theme.dp(32); rowSpacing: Theme.dp(22)
            Control {
                Layout.fillWidth: true; Layout.preferredWidth: Theme.dp(350); Layout.maximumWidth: parent.columns===2 ? Theme.dp(390) : Infinity; Layout.alignment: Qt.AlignTop
                implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding; padding: Theme.dp(18)
                background: Rectangle {
                    radius: Theme.dp(15); gradient: Gradient { GradientStop { position: 0; color: "#637b9d" } GradientStop { position: .18; color: "#243b60" } GradientStop { position: .6; color: "#112546" } GradientStop { position: 1; color: "#62738e" } }
                    border.width: Theme.dp(2); border.color: "#f2cf8b"
                    Rectangle { anchors.fill: parent; anchors.margins: Theme.dp(7); radius: Theme.dp(10); color: "transparent"; border.width: 1; border.color: "#7d91b0" }
                }
                contentItem: ColumnLayout {
                    spacing: Theme.dp(8)
                    RowLayout {
                        Layout.fillWidth: true; spacing: Theme.dp(15)
                        AthleteRating { backend: screen.backend; design: design; compact: true; Layout.fillWidth: true; Layout.preferredWidth: Theme.dp(130) }
                        ColumnLayout {
                            Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.alignment: Qt.AlignTop; spacing: Theme.dp(4)
                            Label { text: screen.backend.profile.name || qsTr("Nowy"); color: design.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                            Label { text: screen.backend.profile.surname || qsTr("skoczek"); color: design.ink; font.family: Theme.displayFamily; font.pointSize: Theme.title; font.weight: Font.Black; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                            Image { source: screen.backend.profile.flag || ""; Layout.preferredWidth: Theme.dp(38); Layout.preferredHeight: Theme.dp(24); sourceSize.width: 76; sourceSize.height: 48; fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
                        }
                    }
                    Item {
                        Layout.fillWidth: true; Layout.preferredHeight: Theme.dp(125)
                        Image { anchors.fill: parent; source: "icons/athlete/terrain.svg"; sourceSize.width: 600; sourceSize.height: 300; fillMode: Image.PreserveAspectCrop; Accessible.ignored: true }
                        Label { visible: portrait.status!==Image.Ready; anchors.centerIn: parent; text: ((screen.backend.profile.name || "S").slice(0,1)+(screen.backend.profile.surname || "J").slice(0,1)).toLocaleUpperCase(); color: "#bdd0ec"; font.family: Theme.displayFamily; font.pointSize: Theme.fontSize(88); font.weight: Font.Black }
                        Image { id: portrait; visible: !!screen.backend.profile.photo; anchors.fill: parent; source: screen.backend.profile.photo || ""; sourceSize.width: 480; sourceSize.height: 500; fillMode: Image.PreserveAspectFit; asynchronous: true; Accessible.name: qsTr("Zdjęcie skoczka") }
                    }
                    AthleteRating { backend: screen.backend; design: design; form: true; compact: true; Layout.fillWidth: true }
                    AthleteInsignia { backend: screen.backend; family: "medal"; Layout.fillWidth: true; badgeSize: Theme.dp(54) }
                }
            }
            ColumnLayout {
                Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: Theme.dp(660); Layout.alignment: Qt.AlignTop; spacing: Theme.dp(15)
                AthleteBadgeRack { backend: screen.backend; design: design; family: "medal"; vertical: true; Layout.fillWidth: true }
                AthleteLanding { backend: screen.backend; design: design; Layout.fillWidth: true }
            }
        }
    }
}
