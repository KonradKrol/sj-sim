import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: copy
    property var backend
    property int sourcePack: 0
    property int sourceAthlete: 0
    readonly property var athletes: backend.legacyAthletes(sourcePack)
    signal finished()
    padding: Theme.dp(15)
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    background: Rectangle { color: Theme.raised; radius: Theme.dp(8); border.color: Theme.outline }
    contentItem: ColumnLayout {
        spacing: Theme.dp(10)
        Label { text: qsTr("Skopiuj z klasycznego modelu"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Label { text: qsTr("OVR: średnia wybicia i lotu. Forma jest przeskalowana, lądowanie przybliżone. Dane osobowe, zdjęcie i rekord są kopiowane. Perki dobierz od zera; pozostałe parametry i cechy nie przechodzą. Oryginał pozostaje bez zmian."); color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        GridLayout {
            Layout.fillWidth: true; columns: width>Theme.dp(600) ? 2 : 1; columnSpacing: Theme.dp(10); rowSpacing: Theme.dp(8)
            SChoice { Layout.fillWidth: true; model: copy.backend.legacyPacks.map(function(p){return p.name}); currentIndex: copy.sourcePack; Accessible.name: qsTr("Paczka źródłowa"); onActivated: {copy.sourcePack=index;copy.sourceAthlete=0} }
            SChoice { objectName: "athleteCopySource"; Layout.fillWidth: true; model: copy.athletes.map(function(p){return p.name}); currentIndex: copy.sourceAthlete; searchable: true; Accessible.name: qsTr("Skoczek do skopiowania"); onActivated: copy.sourceAthlete=index }
        }
        RowLayout {
            SButton { objectName: "athleteCopyConfirm"; text: qsTr("Utwórz kopię"); primary: true; enabled: copy.athletes.length>0; onClicked: {copy.backend.copyLegacy(copy.sourcePack,copy.sourceAthlete);copy.finished()} }
            SButton { objectName: "athleteCopyClose"; text: qsTr("Zamknij"); quiet: true; onClicked: copy.finished() }
        }
    }
}
