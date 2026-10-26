import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: editor
    property var backend
    property int viewMode: 0
    spacing: Theme.dp(12)
    Flow {
        Layout.fillWidth: true; spacing: Theme.dp(6)
        Repeater {
            model: 3
            AthleteViewChoice { required property int index; objectName: "athleteView-"+index; view: index; selected: editor.viewMode===index; onClicked: editor.viewMode=index }
        }
    }
    ScrollView {
        id: scroll
        objectName: "athleteEditorScroll"
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
        contentWidth: availableWidth; clip: true
        onWidthChanged: contentItem.contentX=0
        Loader {
            width: scroll.availableWidth
            sourceComponent: editor.viewMode===0 ? cardView : editor.viewMode===1 ? profileView : studioView
            onLoaded: scroll.contentItem.contentY=0
        }
    }
    Component {
        id: cardView
        ColumnLayout {
            spacing: Theme.dp(18)
            GridLayout {
                Layout.fillWidth: true; columns: width>=Theme.dp(690) ? 2 : 1
                columnSpacing: Theme.dp(20); rowSpacing: Theme.dp(14)
                AthleteCard { backend: editor.backend; compact: parent.columns===1; Layout.fillWidth: true; Layout.preferredWidth: Theme.dp(270); Layout.maximumWidth: parent.columns===2 ? Theme.dp(320) : Infinity; Layout.alignment: Qt.AlignTop }
                ColumnLayout {
                    Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: Theme.dp(490); Layout.alignment: Qt.AlignTop; spacing: Theme.dp(16)
                    AthleteIdentity { backend: editor.backend; Layout.fillWidth: true }
                    GridLayout {
                        columns: width>Theme.dp(390) ? 2 : 1; Layout.fillWidth: true; columnSpacing: Theme.dp(10); rowSpacing: Theme.dp(10)
                        AthleteRating { backend: editor.backend; Layout.fillWidth: true; compact: true }
                        AthleteRating { backend: editor.backend; Layout.fillWidth: true; compact: true; form: true }
                    }
                    AthleteLanding { backend: editor.backend; Layout.fillWidth: true }
                }
            }
            AthletePerks { backend: editor.backend; Layout.fillWidth: true }
        }
    }
    Component {
        id: profileView
        ColumnLayout {
            spacing: Theme.dp(20)
            Rectangle {
                Layout.fillWidth: true; implicitHeight: headline.implicitHeight+Theme.dp(36); color: Theme.stadium; radius: Theme.dp(10)
                RowLayout {
                    id: headline
                    anchors.fill: parent; anchors.margins: Theme.dp(18); spacing: Theme.dp(20)
                    ColumnLayout {
                        Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: 0
                        Label { text: editor.backend.profile.name || qsTr("Nowy"); font.pointSize: Theme.section; color: Theme.headerMuted; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                        Label { text: editor.backend.profile.surname || qsTr("skoczek"); font.family: Theme.displayFamily; font.weight: Font.Black; font.pointSize: Theme.fontSize(52); color: Theme.snow; Layout.fillWidth: true; wrapMode: Text.WrapAnywhere }
                        RowLayout {
                            Image { source: editor.backend.profile.flag || ""; Layout.preferredWidth: Theme.dp(28); Layout.preferredHeight: Theme.dp(18); sourceSize.width: 56; sourceSize.height: 36; fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
                            Label { text: editor.backend.profile.country || ""; color: Theme.headerMuted; font.family: Theme.mono; font.pointSize: Theme.caption }
                        }
                    }
                    Label { text: editor.backend.profile.overall || 70; font.family: Theme.displayFamily; font.weight: Font.Black; font.pointSize: Theme.fontSize(80); color: Theme.signal }
                }
            }
            GridLayout {
                Layout.fillWidth: true; columns: width>Theme.dp(670) ? 2 : 1; columnSpacing: Theme.dp(25); rowSpacing: Theme.dp(16)
                ColumnLayout {
                    Layout.fillWidth: true; Layout.preferredWidth: Theme.dp(260); Layout.alignment: Qt.AlignTop; spacing: Theme.dp(14)
                    AthletePortrait { visible: !!editor.backend.profile.photo; profile: editor.backend.profile; compact: true; Layout.fillWidth: true }
                    AthleteIdentity { backend: editor.backend; Layout.fillWidth: true }
                    AthleteRating { backend: editor.backend; Layout.fillWidth: true; compact: true }
                    AthleteRating { backend: editor.backend; Layout.fillWidth: true; compact: true; form: true }
                    AthleteLanding { backend: editor.backend; Layout.fillWidth: true }
                }
                AthletePerks { backend: editor.backend; stacked: true; Layout.fillWidth: true; Layout.preferredWidth: Theme.dp(390); Layout.alignment: Qt.AlignTop }
            }
        }
    }
    Component {
        id: studioView
        ColumnLayout {
            spacing: Theme.dp(16)
            RowLayout {
                Layout.fillWidth: true
                Label { text: editor.backend.profile.label || ""; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.title; font.weight: Font.Bold; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Image { source: editor.backend.profile.flag || ""; sourceSize.width: 64; sourceSize.height: 40; Layout.preferredWidth: Theme.dp(32); Layout.preferredHeight: Theme.dp(20); fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
            }
            GridLayout {
                Layout.fillWidth: true; columns: width>Theme.dp(450) ? 2 : 1; columnSpacing: Theme.dp(12); rowSpacing: Theme.dp(10)
                AthleteRating { backend: editor.backend; Layout.fillWidth: true }
                AthleteRating { backend: editor.backend; form: true; Layout.fillWidth: true }
            }
            Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }
            AthletePerks { backend: editor.backend; Layout.fillWidth: true }
            Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }
            GridLayout {
                Layout.fillWidth: true; columns: width>Theme.dp(690) ? 2 : 1; columnSpacing: Theme.dp(22); rowSpacing: Theme.dp(15)
                AthleteIdentity { backend: editor.backend; Layout.fillWidth: true; Layout.preferredWidth: Theme.dp(420); Layout.alignment: Qt.AlignTop }
                ColumnLayout {
                    Layout.fillWidth: true; Layout.preferredWidth: Theme.dp(320); Layout.alignment: Qt.AlignTop
                    AthletePortrait { visible: !!editor.backend.profile.photo; compact: true; profile: editor.backend.profile; Layout.fillWidth: true }
                    AthleteLanding { backend: editor.backend; Layout.fillWidth: true }
                }
            }
        }
    }
}
