import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Popup {
    id: welcome
    objectName: "homeWelcome"
    parent: Overlay.overlay
    visible: ui.context.showWelcome===true
    modal: true; focus: true
    closePolicy: Popup.NoAutoClose
    width: Math.min(Theme.dp(560),parent.width-Theme.dp(24))
    x: (parent.width-width)/2; y: Math.max(Theme.dp(12),(parent.height-height)/2)
    padding: Theme.dp(20)
    enter: PopupFade {}
    exit: PopupFade { leaving: true }
    background: Surface { fillColor: Theme.stadium; edgeColor: Theme.signal; strokeWidth: 2; sweep: 42 }
    contentItem: ColumnLayout {
        spacing: Theme.dp(12)
        Label { text: qsTr("Witaj w Sj.Sim"); color: Theme.snow; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Label { text: qsTr("Rozegraj sezon ze znanymi skoczkami, skoczniami i zimowym kalendarzem najważniejszych zawodów. Wszystko jest przygotowane — później możesz to zmienić."); color: Theme.headerMuted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        SButton { objectName: "homeWelcomeExample"; Layout.fillWidth: true; text: ui.context.exampleBusy ? qsTr("Przygotowywanie…") : qsTr("Rozpocznij przykładowy sezon"); primary: true; enabled: !ui.context.exampleBusy; onClicked: ui.launch("example") }
        SButton { Layout.fillWidth: true; text: qsTr("Przejdź do menu"); foreground: Theme.snow; quiet: true; enabled: !ui.context.exampleBusy; onClicked: ui.launch("dismissWelcome") }
    }
}
