import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: author
    objectName: "authorPage"
    readonly property bool wide: availableWidth >= 900
    readonly property int inset: Theme.compact ? 12 : width < 700 || height < 600 ? 16 : 24
    contentWidth: availableWidth
    clip: true
    Item {
        width: author.availableWidth
        height: sheet.implicitHeight + author.inset * 2
        Pane {
            id: sheet
            objectName: "authorSheet"
            x: (parent.width-width)/2; y: author.inset
            width: Math.min(parent.width-author.inset*2,1160)
            padding: 0
            background: Rectangle { color: Theme.surface }
            contentItem: GridLayout {
                columns: author.wide ? 2 : 1
                columnSpacing: 0; rowSpacing: 0
                Image {
                    objectName: "authorPortrait"
                    source: "qrc:/img/author/konrad.jpg"
                    sourceSize.width: 900; sourceSize.height: 1200
                    asynchronous: true
                    fillMode: Image.PreserveAspectCrop
                    clip: true
                    Layout.fillWidth: true; Layout.fillHeight: author.wide
                    Layout.minimumWidth: 0
                    Layout.preferredWidth: author.wide ? 320 : 0
                    Layout.maximumWidth: author.wide ? 380 : Infinity
                    Layout.preferredHeight: author.wide ? 0 : 260
                    Layout.maximumHeight: author.wide ? 760 : 260
                    Layout.alignment: Qt.AlignTop
                    Accessible.role: Accessible.Graphic
                    Accessible.name: qsTr("Zdjęcie Konrada Króla")
                }
                ColumnLayout {
                    id: biography
                    objectName: "authorBiography"
                    Layout.fillWidth: true; Layout.minimumWidth: 0
                    Layout.preferredWidth: author.wide ? 640 : 0
                    Layout.margins: Theme.compact ? 16 : author.wide ? 28 : 20
                    spacing: Theme.compact ? 10 : 16
                    Label { objectName: "authorHeading"; text: ui.context.heading || ""; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.title; font.weight: Font.ExtraBold; Layout.fillWidth: true; Layout.minimumWidth: 0; wrapMode: Text.WordWrap }
                    Label { text: ui.context.introduction || ""; color: Theme.muted; font.pointSize: Theme.body; lineHeight: 1.25; Layout.fillWidth: true; Layout.minimumWidth: 0; wrapMode: Text.WordWrap }
                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 8
                        Repeater {
                            model: ui.context.projects || []
                            AuthorProject { objectName: "authorProject-"+index; Layout.fillWidth: true; Layout.minimumWidth: 0; title: modelData.name; description: modelData.description; destination: modelData.url; featured: index===0 }
                        }
                    }
                    Label { objectName: "authorWork"; text: ui.context.work || ""; color: Theme.muted; font.pointSize: Theme.body; lineHeight: 1.25; Layout.fillWidth: true; Layout.minimumWidth: 0; wrapMode: Text.WordWrap }
                    Rectangle { Layout.fillWidth: true; implicitHeight: 1; color: Theme.divider; Accessible.ignored: true }
                    Label { text: ui.context.availability || ""; color: Theme.ink; font.pointSize: Theme.body; font.weight: Font.DemiBold; Layout.fillWidth: true; Layout.minimumWidth: 0; wrapMode: Text.WordWrap }
                    RowLayout {
                        Layout.fillWidth: true; spacing: 8
                        AuthorSocial { objectName: "authorGitHub"; text: "GitHub"; platform: "github"; destination: ui.context.github || "" }
                        AuthorSocial { objectName: "authorLinkedIn"; text: "LinkedIn"; platform: "linkedin"; destination: ui.context.linkedin || "" }
                        Item { Layout.fillWidth: true }
                    }
                    Rectangle { Layout.fillWidth: true; implicitHeight: 1; color: Theme.divider; Accessible.ignored: true }
                    SButton {
                        objectName: "authorCreditsButton"
                        text: qsTr("Podziękowania")
                        onClicked: ui.showAuthorCredits()
                    }
                }
            }
        }
    }
}
