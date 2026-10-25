import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: setup
    property var backend
    readonly property var config: backend.config
    property int selectedVariant: 0
    property int selectedGroup: 0
    readonly property var groups: ["conditions","athlete","field","hill","engine","experimental"]
    readonly property var definitions: backend.definitions.filter(function(p){return p.group===setup.groups[setup.selectedGroup]})
    property int sweepKey: 0
    function parameterValue(key,index){var changes=config.variants[index].changes;return changes[key]!==undefined?changes[key]:config.parameters[key]!==undefined?config.parameters[key]:key==="hill.k"?config.hill.k:key==="hill.hs"?config.hill.hs:backend.definitions.filter(function(p){return p.key===key})[0].default}
    contentWidth: availableWidth; clip: true
    onConfigChanged: selectedVariant=Math.max(0,Math.min(selectedVariant,config.variants.length-1))
    ColumnLayout {
        width: setup.availableWidth; spacing: 12
        enabled: !setup.backend.busy
        GridLayout {
            columns: setup.width<780?1:2
            Layout.fillWidth: true; columnSpacing: 14; rowSpacing: 12
            Pane {
                Layout.fillWidth: true; Layout.alignment: Qt.AlignTop; padding: 14
                background: Surface { fillColor: Theme.surface; sweep: 22 }
                contentItem: ColumnLayout {
                    spacing: 8
                    Label { text: qsTr("Dane i liczba prób"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section }
                    SField { objectName: "playgroundName"; Layout.fillWidth: true; text: setup.config.name; Accessible.name: qsTr("Nazwa eksperymentu"); onEditingFinished: setup.backend.setValue("name",text) }
                    GridLayout {
                        columns: 2; Layout.fillWidth: true; columnSpacing: 10; rowSpacing: 8
                        PlaygroundNumber { Layout.fillWidth: true; name: "playground-count"; caption: qsTr("Konkursy na wariant"); value: setup.config.competitions; minimum: 1; maximum: 1000000; step: 100; onCommitted: setup.backend.setValue("competitions",value) }
                        PlaygroundNumber { Layout.fillWidth: true; name: "playground-threads"; caption: qsTr("Wątki"); value: setup.config.threads; minimum: 1; maximum: 64; onCommitted: setup.backend.setValue("threads",value) }
                        PlaygroundNumber { Layout.fillWidth: true; name: "playground-athletes"; caption: qsTr("Skoczkowie"); value: setup.config.athletes; minimum: 2; maximum: 512; help: qsTr("Zmiana liczby przełącza na stawkę syntetyczną."); onCommitted: setup.backend.setValue("athletes",value) }
                        PlaygroundNumber { Layout.fillWidth: true; name: "playground-samples"; caption: qsTr("Przykłady konkursów"); value: setup.config.sampleLimit; minimum: 0; maximum: 4096; step: 16; help: qsTr("Liczba pierwszych konkursów ze szczegółami wybranego skoczka. Wszystkie konkursy wchodzą do statystyk. Dowolny konkurs można odtworzyć z jego numeru."); onCommitted: setup.backend.setValue("sampleLimit",value) }
                    }
                    Label { text: qsTr("Ziarno losowania"); color: Theme.muted; font.pointSize: Theme.caption }
                    SField { objectName: "playgroundSeed"; Layout.fillWidth: true; text: setup.config.seed; validator: RegularExpressionValidator { regularExpression: /[0-9]{1,20}/ } Accessible.name: qsTr("Ziarno losowania"); onEditingFinished: setup.backend.setValue("seed",text) }
                    Label { text: qsTr("Stawka"); color: Theme.muted; font.pointSize: Theme.caption }
                    SChoice { objectName: "playgroundRoster"; Layout.fillWidth: true; model: setup.backend.athletePacks; displayText: setup.config.roster ? setup.config.rosterName || qsTr("Stawka z konfiguracji") : qsTr("Stawka syntetyczna"); onActivated: setup.backend.useAthletePack(currentIndex) }
                    Label { text: qsTr("Skocznia"); color: Theme.muted; font.pointSize: Theme.caption }
                    SChoice { objectName: "playgroundHill"; Layout.fillWidth: true; model: setup.backend.hillNames; displayText: setup.config.hill.name+" · HS"+setup.config.hill.hs; onActivated: setup.backend.useHill(currentIndex) }
                    RowLayout {
                        visible: !setup.config.hill.source; Layout.fillWidth: true
                        PlaygroundNumber { Layout.fillWidth: true; name: "playground-k"; caption: qsTr("K"); value: setup.config.hill.k; minimum: 10; maximum: 300; onCommitted: {var hill=Object.assign({},setup.config.hill);hill.k=value;hill.hs=Math.max(value,hill.hs);setup.backend.setValue("hill",hill)} }
                        PlaygroundNumber { Layout.fillWidth: true; name: "playground-hs"; caption: qsTr("HS"); value: setup.config.hill.hs; minimum: setup.config.hill.k; maximum: 400; onCommitted: {var hill=Object.assign({},setup.config.hill);hill.hs=value;setup.backend.setValue("hill",hill)} }
                    }
                    Label { text: qsTr("Zasady"); color: Theme.muted; font.pointSize: Theme.caption }
                    SChoice { objectName: "playgroundRules"; Layout.fillWidth: true; model: setup.backend.ruleNames; displayText: setup.config.rulesData ? setup.config.rulesData.name : qsTr("Zasady eksperymentu"); onActivated: setup.backend.useRules(currentIndex) }
                    RowLayout {
                        visible: !setup.config.rulesData; Layout.fillWidth: true
                        PlaygroundNumber { Layout.fillWidth: true; name: "playground-rounds"; caption: qsTr("Serie"); value: setup.config.rounds.length; minimum: 1; maximum: 8; onCommitted: {var rounds=[];for(var i=0;i<value;i++)rounds.push(i===0?setup.config.athletes:Math.min(setup.config.athletes,setup.config.rounds[i] || setup.config.rounds[setup.config.rounds.length-1]));setup.backend.setValue("rounds",rounds)} }
                        PlaygroundNumber { Layout.fillWidth: true; name: "playground-cut"; caption: qsTr("Limit po pierwszej serii"); value: setup.config.rounds[1] || setup.config.athletes; minimum: 1; maximum: setup.config.athletes; enabled: setup.config.rounds.length>1; onCommitted: {var rounds=setup.config.rounds.slice();for(var i=1;i<rounds.length;i++)rounds[i]=Math.min(rounds[i],value);rounds[1]=value;setup.backend.setValue("rounds",rounds)} }
                    }
                    PlaygroundNumber { Layout.fillWidth: true; name: "playground-focus"; caption: qsTr("Wybrany skoczek · numer na liście"); value: setup.config.focus+1; minimum: 1; maximum: setup.config.athletes; onCommitted: setup.backend.setValue("focus",value-1) }
                    Label { text: qsTr("%1 konkursów × %2 wariantów = %3 konkursów").arg(setup.config.competitions).arg(setup.config.variants.length).arg(setup.config.competitions*setup.config.variants.length); color: Theme.ink; font.family: Theme.mono; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                }
            }
            Pane {
                Layout.fillWidth: true; Layout.alignment: Qt.AlignTop; padding: 14
                background: Surface { fillColor: Theme.surface; sweep: 22 }
                contentItem: ColumnLayout {
                    spacing: 8
                    Label { text: qsTr("Warianty i parametry"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section }
                    Label { text: qsTr("Pierwszy wariant jest odniesieniem. Pozostałe dziedziczą wspólne wartości, chyba że ustawisz własne."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                    Flow {
                        Layout.fillWidth: true; spacing: 5
                        Repeater { model: setup.config.variants; SButton { required property var modelData; required property int index; text: modelData.name; primary: setup.selectedVariant===index; implicitHeight: Theme.controlHeight; font.pointSize: Theme.body; onClicked: setup.selectedVariant=index } }
                        SButton { objectName: "playgroundAddVariant"; text: "+"; Accessible.name: qsTr("Dodaj wariant"); onClicked: setup.backend.addVariant() }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        SField { Layout.fillWidth: true; text: setup.config.variants[setup.selectedVariant].name; Accessible.name: qsTr("Nazwa wariantu"); onEditingFinished: setup.backend.renameVariant(setup.selectedVariant,text) }
                        SButton { text: qsTr("Usuń"); quiet: true; enabled: setup.selectedVariant>0; onClicked: setup.backend.removeVariant(setup.selectedVariant) }
                    }
                    SChoice { Layout.fillWidth: true; model: [qsTr("Warunki"),qsTr("Wybrany skoczek"),qsTr("Stawka"),qsTr("Skocznia"),qsTr("Silnik"),qsTr("Współczynniki eksperymentalne")]; Accessible.name: qsTr("Grupa parametrów"); currentIndex: setup.selectedGroup; onActivated: setup.selectedGroup=currentIndex }
                    GridLayout {
                        Layout.fillWidth: true; columns: 2; columnSpacing: 10; rowSpacing: 8
                        Repeater { model: setup.definitions; PlaygroundNumber { required property var modelData; Layout.fillWidth: true; Layout.minimumWidth: 0; name: "playground-parameter-"+modelData.key; caption: modelData.label; value: setup.parameterValue(modelData.key,setup.selectedVariant); minimum: modelData.minimum; maximum: modelData.maximum; step: modelData.step; decimals: step<1?2:0; help: modelData.help; onCommitted: setup.backend.setVariantParameter(setup.selectedVariant,modelData.key,value) } }
                    }
                    Rectangle { Layout.fillWidth: true; implicitHeight: 1; color: Theme.outline }
                    Label { text: qsTr("Gotowe porównania"); color: Theme.ink; font.pointSize: Theme.body; font.bold: true }
                    Flow {
                        Layout.fillWidth: true; spacing: 4
                        Repeater { model: [{key:"wind",name:qsTr("Wiatr")},{key:"windAdvantage",name:qsTr("Przewaga wiatru")},{key:"form",name:qsTr("Forma")},{key:"balance",name:qsTr("Wybicie / lot")},{key:"landing",name:qsTr("Lądowanie")},{key:"randomness",name:qsTr("Losowość")}]; SButton { required property var modelData; text: modelData.name; quiet: true; onClicked: setup.backend.preset(modelData.key) } }
                    }
                    Label { text: qsTr("Skanowanie parametru"); color: Theme.ink; font.pointSize: Theme.body; font.bold: true }
                    SChoice { id: sweep; Layout.fillWidth: true; model: setup.backend.definitions.map(function(p){return p.label}); Accessible.name: qsTr("Parametr do skanowania"); onActivated: setup.sweepKey=currentIndex }
                    RowLayout {
                        Layout.fillWidth: true
                        PlaygroundNumber { id: from; Layout.fillWidth: true; caption: qsTr("Od"); name: "playground-sweep-from"; value: setup.backend.definitions[setup.sweepKey].minimum; minimum: value; maximum: setup.backend.definitions[setup.sweepKey].maximum; step: setup.backend.definitions[setup.sweepKey].step; decimals: 2 }
                        PlaygroundNumber { id: to; Layout.fillWidth: true; caption: qsTr("Do"); name: "playground-sweep-to"; value: setup.backend.definitions[setup.sweepKey].maximum; minimum: setup.backend.definitions[setup.sweepKey].minimum; maximum: value; step: setup.backend.definitions[setup.sweepKey].step; decimals: 2 }
                        PlaygroundNumber { id: steps; Layout.fillWidth: true; caption: qsTr("Wartości"); name: "playground-sweep-count"; value: 5; minimum: 2; maximum: 31 }
                    }
                    SButton { objectName: "playgroundSweep"; Layout.fillWidth: true; text: qsTr("Zbuduj warianty skanowania"); onClicked: setup.backend.sweep(setup.backend.definitions[setup.sweepKey].key,from.currentValue,to.currentValue,steps.currentValue) }
                    Label { text: qsTr("Tworzenie skanowania zastępuje listę wariantów. Dane gry pozostają bez zmian."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                }
            }
        }
    }
}
