import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: lab
    objectName: "formEngineLabScreen"
    readonly property var backend: ui.root.values.backend
    readonly property bool narrow: width<Theme.dp(850)
    property int page: 0
    property int viewVariant: 0
    readonly property var report: backend.result
    readonly property var view: (report.views || [])[viewVariant] || ({})
    readonly property var groups: [qsTr("Okresy formy"),qsTr("Przejścia"),qsTr("Zmiany formy"),qsTr("Początek"),qsTr("Perk formy")]
    onNarrowChanged: if(!narrow && page>2)page=0
    ColumnLayout {
    anchors.fill: parent; anchors.margins: Theme.dp(12)
    spacing: Theme.dp(8)
    Flow {
        Layout.fillWidth: true; spacing: Theme.dp(6)
        SButton { objectName: "formLabRun"; text: lab.backend.busy ? qsTr("Przerwij") : qsTr("Uruchom"); primary: true; onClicked: {if(lab.backend.busy)lab.backend.cancel();else {if(lab.page===1)lab.backend.setOption("usePack",true);lab.backend.start()}} }
        SButton { text: qsTr("Zachowaj A"); enabled: !lab.backend.busy; onClicked: lab.backend.freezeReference() }
        SButton { text: qsTr("Wczytaj"); enabled: !lab.backend.busy; onClicked: lab.backend.loadConfig() }
        SButton { text: qsTr("Zapisz profil"); enabled: !lab.backend.busy; onClicked: lab.backend.saveConfig() }
        SButton { text: qsTr("Używaj w nowych rozgrywkach"); enabled: !lab.backend.busy; onClicked: lab.backend.useForNewGames() }
        SButton { text: qsTr("Eksport wyników"); enabled: !!lab.report.views; onClicked: lab.backend.exportResult() }
    }
    ProgressBar { visible: lab.backend.busy; Layout.fillWidth: true; value: lab.backend.progress }
    Label { visible: lab.backend.busy; text: lab.backend.progressText; color: Theme.muted; font.pointSize: Theme.caption }
    Label { visible: !!text; text: lab.backend.message; color: Theme.ink; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
    SectionTabs { Layout.fillWidth: true; options: lab.narrow ? [qsTr("Próba"),qsTr("Zawodnicy"),qsTr("Parametry"),qsTr("Wyniki")] : [qsTr("Próba"),qsTr("Zawodnicy"),qsTr("Parametry")]; currentIndex: lab.page; onSelected: {lab.page=index;if(index===1)lab.backend.setOption("usePack",true)} }
    RowLayout {
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0; spacing: Theme.dp(16)
        ScrollView {
            id: setupScroll
            visible: lab.page===0 || lab.page===2
            Layout.preferredWidth: Theme.dp(355); Layout.fillWidth: lab.narrow; Layout.fillHeight: true; Layout.minimumHeight: 0; Layout.minimumWidth: 0
            contentWidth: availableWidth; clip: true
            ColumnLayout {
                width: setupScroll.availableWidth; spacing: Theme.dp(10)
                ColumnLayout {
                    visible: lab.page===0; Layout.fillWidth: true; spacing: Theme.dp(10)
                    SCheck { text: qsTr("Użyj zawodników z paczki"); checked: lab.backend.options.usePack; enabled: !lab.backend.busy; onClicked: lab.backend.setOption("usePack",checked); Layout.fillWidth: true }
                    SChoice { visible: lab.backend.options.usePack; model: lab.backend.packs; currentIndex: lab.backend.options.pack; searchable: true; Layout.fillWidth: true; enabled: !lab.backend.busy; onActivated: lab.backend.setOption("pack",index) }
                    SButton { visible: lab.backend.options.usePack; text: qsTr("Zawodnicy: %1 · wybierz i edytuj").arg(lab.backend.selectedAthletes); Layout.fillWidth: true; onClicked: lab.page=1 }
                    Repeater {
                        model: [
                            {key:"count",label:qsTr("Zawodnicy"),minimum:1,maximum:10000},
                            {key:"form",label:qsTr("Forma początkowa"),minimum:1,maximum:100},
                            {key:"perk",label:qsTr("Perk formy (−4 równa, +4 chwiejna)"),minimum:-4,maximum:4},
                            {key:"days",label:qsTr("Dni"),minimum:1,maximum:3650},
                            {key:"repeats",label:qsTr("Powtórzenia (różne seedy)"),minimum:1,maximum:10000},
                            {key:"threads",label:qsTr("Wątki"),minimum:1,maximum:32}]
                        FormLabParameter { required property var modelData; Layout.fillWidth: true; backend: lab.backend; definition: modelData; option: true; visible: !lab.backend.options.usePack || !["count","form","perk"].includes(modelData.key) }
                    }
                    RowLayout { Layout.fillWidth: true; Label { text: qsTr("Seed"); color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true } SField { objectName: "formLabSeed"; Layout.preferredWidth: Theme.dp(130); text: lab.backend.options.seed; enabled: !lab.backend.busy; onEditingFinished: lab.backend.setOption("seed",text) } }
                    SCheck { text: qsTr("Porównaj A z bieżącą konfiguracją B"); checked: lab.backend.options.compare; enabled: !lab.backend.busy; onClicked: lab.backend.setOption("compare",checked); Layout.fillWidth: true }
                    Label { text: qsTr("Bez paczki wszyscy zaczynają od podanej formy i perka. Paczka zachowuje wartości każdego zawodnika. OVR nie wpływa na generator formy."); color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                }
                ColumnLayout {
                    visible: lab.page===2; Layout.fillWidth: true; spacing: Theme.dp(8)
                    SField { id: search; Layout.fillWidth: true; placeholderText: qsTr("Szukaj parametru") }
                    SChoice { id: groupChoice; objectName: "formLabGroup"; Layout.fillWidth: true; model: lab.groups }
                    Repeater {
                        model: lab.backend.definitions
                        FormLabParameter {
                            required property var modelData
                            Layout.fillWidth: true; backend: lab.backend; definition: modelData
                            visible: search.text ? modelData.label.toLocaleLowerCase().indexOf(search.text.toLocaleLowerCase())>=0 : modelData.group===lab.groups[groupChoice.currentIndex]
                        }
                    }
                    SButton { text: qsTr("Przywróć parametry robocze"); enabled: !lab.backend.busy; onClicked: lab.backend.reset() }
                }
            }
        }
        FormLabRoster {
            visible: lab.page===1
            backend: lab.backend
            Layout.preferredWidth: Theme.dp(355); Layout.fillWidth: lab.narrow; Layout.fillHeight: true; Layout.minimumHeight: 0; Layout.minimumWidth: 0
        }
        ScrollView {
            id: resultsScroll
            visible: !lab.narrow || lab.page===3
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0
            contentWidth: availableWidth; clip: true
            ColumnLayout {
                width: resultsScroll.availableWidth; spacing: Theme.dp(12)
                Label { visible: !lab.report.views; text: qsTr("Uruchom próbę, aby zobaczyć przebieg formy i statystyki."); color: Theme.muted; font.pointSize: Theme.body; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                Label { visible: lab.backend.stale; text: qsTr("Wyniki poprzedniej konfiguracji"); color: Theme.warning; font.pointSize: Theme.body }
                SChoice { visible: !!lab.report.compare; Layout.fillWidth: true; model: [qsTr("Konfiguracja A"),qsTr("Konfiguracja B")]; currentIndex: lab.viewVariant; onActivated: lab.viewVariant=index }
                Repeater {
                    model: (lab.report.metrics || [])[lab.viewVariant] || []
                    RowLayout { required property var modelData; Layout.fillWidth: true; Label { text: modelData.label; color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap } Label { text: Number(modelData.mean).toLocaleString(Qt.locale(Theme.localeName),'f',2)+(modelData.ci95!==undefined?" ± "+Number(modelData.ci95).toLocaleString(Qt.locale(Theme.localeName),'f',2):""); color: Theme.ink; font.family: Theme.mono; font.pointSize: Theme.body } }
                }
                Label { visible: !!lab.report.views; text: qsTr("± oznacza przybliżony 95% przedział dla średniej z niezależnych powtórzeń; pojawia się od dwóch powtórzeń."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                BalanceLabPlot { visible: !!lab.report.views; Layout.fillWidth: true; caption: qsTr("Forma — pierwsze trzy osoby"); xLabel: qsTr("Dzień"); yLabel: "F"; series: lab.view.forms || [] }
                BalanceLabPlot { visible: !!lab.report.views; Layout.fillWidth: true; caption: qsTr("Baza formy"); xLabel: qsTr("Dzień"); yLabel: "μ"; series: lab.view.bases || [] }
                BalanceLabPlot { visible: !!lab.report.views; Layout.fillWidth: true; caption: qsTr("Okres dyspozycji"); xLabel: qsTr("Dzień"); yLabel: qsTr("0 kryzys · 1 słaby · 2 typowy · 3 dobry · 4 szczytowy"); series: lab.view.regimes || [] }
                BalanceLabPlot { visible: !!lab.report.views; Layout.fillWidth: true; caption: qsTr("Średnia populacji — pierwsze powtórzenie"); xLabel: qsTr("Dzień"); yLabel: "F"; series: (lab.report.views || []).reduce(function(out,v){return out.concat(v.population)},[]) }
                BalanceLabPlot { visible: !!lab.report.views; Layout.fillWidth: true; caption: qsTr("Rozkład formy — cały batch"); xLabel: "F"; yLabel: qsTr("Obserwacje (%)"); series: lab.report.histograms || [] }
            }
        }
    }
}
}
