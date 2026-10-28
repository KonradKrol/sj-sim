import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: playground
    objectName: "playgroundScreen"
    anchors.fill: parent; anchors.margins: Theme.dp(14)
    readonly property var backend: ui.root.values.backend
    readonly property bool hasReport: !!backend.report.variants
    property int section: 0
    property int resultVariant: 1
    property int resultAthlete: 0
    spacing: 10
    RowLayout {
        Layout.fillWidth: true; spacing: 8
        ColumnLayout {
            Layout.fillWidth: true; spacing: 2
            Label { text: qsTr("Laboratorium symulacji v2"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading }
            Label { text: qsTr("Odizolowane dane · powtarzalne losowanie · porównania wariantów"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        }
        SButton { objectName: "playgroundRun"; text: qsTr("Uruchom"); primary: true; enabled: !playground.backend.busy; onClicked: playground.backend.start() }
        SButton { objectName: "playgroundCancel"; text: qsTr("Przerwij"); visible: playground.backend.busy; onClicked: playground.backend.cancel() }
    }
    Flow {
        Layout.fillWidth: true; spacing: 5
        SButton { text: qsTr("Wczytaj konfigurację"); quiet: true; enabled: !playground.backend.busy; onClicked: playground.backend.loadConfig() }
        SButton { text: qsTr("Zapisz konfigurację"); quiet: true; onClicked: playground.backend.saveConfig() }
        SButton { text: qsTr("Eksportuj wyniki"); quiet: true; enabled: playground.hasReport; onClicked: playground.backend.exportResults() }
        SButton { text: qsTr("Użyj konfiguracji raportu"); quiet: true; enabled: playground.hasReport && !playground.backend.busy; onClicked: {playground.backend.useReportConfig();playground.section=0} }
    }
    ColumnLayout {
        visible: playground.backend.busy; Layout.fillWidth: true; spacing: 3
        ProgressBar { Layout.fillWidth: true; from: 0; to: playground.backend.config.competitions; value: playground.backend.completed }
        Label { text: qsTr("%1 / %2 powtórzeń · %3 skoków · %4 s").arg(playground.backend.completed).arg(playground.backend.config.competitions).arg(playground.backend.jumpCount).arg(playground.backend.elapsed.toLocaleString(Qt.locale(Theme.localeName),'f',1)); color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    }
    Label { visible: playground.backend.message.length>0; text: playground.backend.message; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    SectionTabs {
        Layout.fillWidth: true
        options: [qsTr("Eksperyment"),qsTr("Porównanie"),qsTr("Skoczek"),qsTr("Konkurs"),qsTr("Historia"),"CLI",qsTr("Lądowanie"),qsTr("Wiatr"),qsTr("Kalibracja"),qsTr("Stawka")]
        optionEnabled: [true,playground.hasReport,playground.hasReport,playground.hasReport,true,true,true,true,true,playground.hasReport]
        currentIndex: playground.section
        onSelected: playground.section=index
    }
    Loader {
        id: content
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
        sourceComponent: playground.section===0?setup:playground.section===1?comparison:playground.section===2?athlete:playground.section===3?replay:playground.section===4?history:playground.section===5?cli:playground.section===6?landing:playground.section===7?wind:playground.section===8?targets:field
    }
    Connections { target: playground.backend; function onReportChanged(){if(playground.hasReport){playground.resultVariant=Math.min(playground.resultVariant,playground.backend.report.variants.length-1);playground.resultAthlete=playground.backend.report.config.focus;playground.section=1}} }
    Component { id: field; PlaygroundField { backend: playground.backend; selectedVariant: playground.resultVariant; onVariantSelected: function(index){playground.resultVariant=index}; onAthleteSelected: function(index){playground.resultAthlete=index;playground.section=2} } }
    Component { id: landing; PlaygroundAnalysis { backend: playground.backend; wind: false } }
    Component { id: wind; PlaygroundAnalysis { backend: playground.backend; wind: true } }
    Component { id: targets; PlaygroundTargets { backend: playground.backend } }
    Component { id: setup; PlaygroundSetup { backend: playground.backend } }
    Component { id: comparison; PlaygroundResults { backend: playground.backend; selectedVariant: playground.resultVariant; selectedAthlete: playground.resultAthlete; onVariantSelected: function(index){playground.resultVariant=index}; onAthleteSelected: function(index){playground.resultAthlete=index} } }
    Component { id: athlete; PlaygroundResults { backend: playground.backend; individual: true; selectedVariant: playground.resultVariant; selectedAthlete: playground.resultAthlete; onVariantSelected: function(index){playground.resultVariant=index}; onAthleteSelected: function(index){playground.resultAthlete=index}; onReplayRequested: {playground.section=3;playground.backend.replay(variant,competition)} } }
    Component { id: replay; PlaygroundReplay { backend: playground.backend; selectedVariant: playground.resultVariant; selectedAthlete: playground.resultAthlete; onVariantSelected: function(index){playground.resultVariant=index}; onAthleteSelected: function(index){playground.resultAthlete=index} } }
    Component {
        id: history
        ListView {
            objectName: "playgroundHistory"; clip: true; model: playground.backend.history; spacing: 6
            ScrollBar.vertical: ScrollBar {}
            delegate: ItemDelegate { required property var modelData; required property int index; width: ListView.view.width; text: modelData.name+" · "+modelData.time+" · "+modelData.competitions+" × "+modelData.variants; enabled: !playground.backend.busy; font.pointSize: Theme.body; onClicked: playground.backend.loadRun(index) }
            EmptyState { anchors.fill: parent; visible: !playground.backend.history.length; title: qsTr("Brak zapisanych eksperymentów"); description: qsTr("Zakończony lub normalnie przerwany eksperyment zapisuje ukończone porównania. Konfigurację można później ponownie uruchomić.") }
        }
    }
    Component {
        id: cli
        ScrollView {
            id: cliScroll
            contentWidth: availableWidth; clip: true
            ColumnLayout {
                width: cliScroll.availableWidth; spacing: 12
                Label { text: qsTr("Automatyzacja i analiza poza aplikacją"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: qsTr("Zapisz konfigurację z tego ekranu albo wygeneruj przykład. CLI korzysta z tego samego silnika i uruchamia się bez interfejsu graficznego."); color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                TextArea { Layout.fillWidth: true; readOnly: true; selectByMouse: true; wrapMode: TextEdit.Wrap; font.family: Theme.mono; font.pointSize: Theme.body; text: "sj-sim --playground-example experiment.json\n\nsj-sim --playground --config experiment.json --output results/run-01 --threads 8\n\nsj-sim --playground --config experiment.json --replay 1:42 --output replay.json\n\nsj-sim --playground --config experiment.json --analysis 1 --output analysis.json"; background: Rectangle { color: Theme.surface; radius: 8 } }
                Label { text: qsTr("Windows: użyj sj-sim.exe. Indeksy wariantu i konkursu w CLI liczą się od zera. Folder wyniku musi być nowy; istniejące raporty nie są nadpisywane."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: qsTr("Eksport: pełny raport JSON, konfiguracja, porównania CSV, agregaty każdego skoczka i przykładowe skoki. Analiza w Pythonie lub przez agenta nie wymaga klikania w UI."); color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            }
        }
    }
}
