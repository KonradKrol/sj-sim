import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: home
    readonly property bool editorial: !Theme.compact && width >= 1100 && height >= 900
    color: Theme.background
    LayoutMirroring.enabled: Qt.application.layoutDirection === Qt.RightToLeft
    LayoutMirroring.childrenInherit: true
    ColumnLayout {
        anchors.fill: parent
        enabled: !ui.context.exampleBusy
        spacing: 0
        ScrollView {
            id: scroll
            Layout.fillWidth: true; Layout.fillHeight: true
            contentWidth: availableWidth
            clip: true
            ColumnLayout {
                width: scroll.availableWidth
                spacing: 0
                HomeHero { visible: !Theme.compact && home.height >= 1040; Layout.fillWidth: true; compact: home.width < 850 }
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.margins: Theme.compact ? 6 : home.width < 700 || home.height < 760 ? 16 : 32
                    spacing: Theme.compact ? 6 : 16
                    Pane {
                        id: welcome
                        objectName: "homeWelcome"
                        visible: ui.context.showWelcome===true
                        Layout.fillWidth: true
                        padding: Theme.dp(16)
                        background: Surface { fillColor: Theme.stadium; edgeColor: Theme.signal; strokeWidth: 2; sweep: 42 }
                        MenuEntrance { id: welcomeEntrance; target: welcome }
                        opacity: welcomeEntrance.progress
                        transform: Translate { y: Theme.dp(12)*(1-welcomeEntrance.progress) }
                        contentItem: ColumnLayout {
                            spacing: Theme.dp(8)
                            Label { text: qsTr("Witaj w Sj.Sim"); color: Theme.snow; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true; Layout.fillWidth: true }
                            Label { text: qsTr("Rozpocznij gotowy sezon ze znanymi skoczkami i zimowym kalendarzem. Wszystko możesz później edytować."); color: Theme.headerMuted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                            Flow {
                                Layout.fillWidth: true; spacing: Theme.dp(6)
                                SButton { objectName: "homeWelcomeExample"; text: ui.context.exampleBusy ? qsTr("Przygotowywanie…") : qsTr("Rozpocznij przykładowy sezon"); primary: true; enabled: !ui.context.exampleBusy; onClicked: ui.launch("example") }
                                SButton { text: qsTr("Przejdź do menu"); foreground: Theme.snow; quiet: true; onClicked: ui.launch("dismissWelcome") }
                            }
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: qsTr("Symulacja"); font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.weight: Font.DemiBold; color: Theme.ink; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                        SButton { objectName: "homeExample"; visible: !ui.context.showWelcome; text: ui.context.exampleBusy ? qsTr("Przygotowywanie…") : qsTr("Przykładowy sezon"); enabled: !ui.context.exampleBusy; onClicked: ui.launch("example") }
                    }
                    GridLayout {
                        Layout.fillWidth: true
                        columns: home.editorial ? 2 : home.width >= 900 ? 3 : home.width >= 680 ? 2 : 1
                        columnSpacing: Theme.gap; rowSpacing: Theme.gap
                        HomeCard { Layout.rowSpan: home.editorial ? 2 : 1; entranceDelay: 0; expansive: home.editorial; title: qsTr("Sezony"); description: qsTr("Zapisy symulacji, kalendarze i klasyfikacje."); detail: qsTr("Otwórz zapisy"); featured: true; Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: home.editorial ? 600 : 320; onClicked: ui.launch("seasons") }
                        HomeCard { entranceDelay: 65; title: qsTr("Pojedynczy konkurs"); description: qsTr("Konfiguracja konkursu i przebieg rywalizacji."); detail: qsTr("Konfiguruj konkurs"); Layout.fillWidth: true; Layout.fillHeight: true; onClicked: ui.launch("competition") }
                        HomeCard { entranceDelay: 130; title: qsTr("Pojedyncze skoki"); description: qsTr("Symulacja skoków wybranego zawodnika."); detail: qsTr("Konfiguruj skoki"); Layout.fillWidth: true; Layout.fillHeight: true; onClicked: ui.launch("jumps") }
                    }
                    HomeTools { Layout.fillWidth: true; compact: home.width < 700 }
                    RowLayout {
                        Layout.fillWidth: true
                        Item { Layout.fillWidth: true }
                        SButton { objectName: "openAuthor"; text: qsTr("O autorze"); quiet: true; onClicked: ui.launch("author") }
                        SButton { text: qsTr("Zgłoś problem"); quiet: true; onClicked: ui.launch("issue") }
                    }
                }
            }
        }
    }
}
