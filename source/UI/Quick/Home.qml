import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: home
    readonly property bool editorial: width >= 1100 && height >= 900
    color: Theme.background
    LayoutMirroring.enabled: Qt.application.layoutDirection === Qt.RightToLeft
    LayoutMirroring.childrenInherit: true
    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        ScrollView {
            id: scroll
            Layout.fillWidth: true; Layout.fillHeight: true
            contentWidth: availableWidth
            clip: true
            ColumnLayout {
                width: scroll.availableWidth
                spacing: 0
                HomeHero { visible: home.height >= 1040; Layout.fillWidth: true; compact: home.width < 850 }
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.margins: home.width < 700 || home.height < 760 ? 16 : 32
                    spacing: 16
                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: qsTr("Symulacja"); font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.weight: Font.DemiBold; color: Theme.ink; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                        Label { visible: home.width > 850; text: qsTr("01 — 03"); color: Theme.muted; font.pointSize: Theme.caption; font.family: Theme.mono }
                    }
                    GridLayout {
                        Layout.fillWidth: true
                        columns: home.editorial ? 2 : home.width >= 900 ? 3 : home.width >= 680 ? 2 : 1
                        columnSpacing: 16; rowSpacing: 16
                        HomeCard { Layout.rowSpan: home.editorial ? 2 : 1; number: "01"; expansive: home.editorial; title: qsTr("Sezony"); description: qsTr("Zapisy symulacji, kalendarze i klasyfikacje."); detail: qsTr("Otwórz zapisy"); featured: true; Layout.fillWidth: true; Layout.fillHeight: true; Layout.preferredWidth: home.editorial ? 600 : 320; onClicked: ui.launch("seasons") }
                        HomeCard { number: "02"; title: qsTr("Pojedynczy konkurs"); description: qsTr("Konfiguracja konkursu i przebieg rywalizacji."); detail: qsTr("Konfiguruj konkurs"); Layout.fillWidth: true; Layout.fillHeight: true; onClicked: ui.launch("competition") }
                        HomeCard { number: "03"; title: qsTr("Pojedyncze skoki"); description: qsTr("Symulacja skoków wybranego zawodnika."); detail: qsTr("Konfiguruj skoki"); Layout.fillWidth: true; Layout.fillHeight: true; onClicked: ui.launch("jumps") }
                    }
                    HomeTools { Layout.fillWidth: true; compact: home.width < 700 }
                    RowLayout {
                        Layout.fillWidth: true
                        Item { Layout.fillWidth: true }
                        SButton { text: qsTr("Autorzy"); quiet: true; onClicked: ui.launch("credits") }
                        SButton { text: qsTr("Zgłoś problem"); quiet: true; onClicked: ui.launch("issue") }
                    }
                }
            }
        }
    }
    Shortcut { sequence: "Ctrl+1"; onActivated: ui.launch("seasons") }
    Shortcut { sequence: "Ctrl+2"; onActivated: ui.launch("competition") }
    Shortcut { sequence: "Ctrl+3"; onActivated: ui.launch("jumps") }
}
