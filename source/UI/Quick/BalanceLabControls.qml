import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: controls
    objectName: "balanceLabControls"
    property var backend
    property var question: ({})
    property int panel: 0
    property int generatorPreset: -1
    property bool sourceNotesOpen: false
    property bool fullRegistry: false
    property var selected: []
    readonly property var definitions: backend.definitions || []
    readonly property var members: backend.athletes || []
    readonly property var keys: definitions.map(function(d){return d.key})
    readonly property var axisLabels: definitions.map(function(d){return d.label + (d.unit && d.unit!=="1" ? " · " + d.unit : "")})
    readonly property var dataKeys: ["athlete.overall","athlete.form","athlete.landing","athlete.perk.0","athlete.perk.1","athlete.perk.2"]
    function definition(key){var i=keys.indexOf(key);return i>=0?definitions[i]:({})}
    function relevant(d){
        if(fullRegistry)return true
        var q=question.id
        if(d.group==="generator")return q==="generator" && d.preset===generatorPreset
        if(q==="landing" || q==="marks")return d.group==="landing" || d.key==="hill.realHs" || d.key==="wind.cross" || d.key.indexOf("flight.geometry")===0 || d.key==="flight.usableLandingBeyondRealHsAtK"
        if(d.group==="hill")return (q==="scoring" || q==="gate") && ["hill.pointsAtK","hill.pointsPerMetre","hill.gatePoints","hill.headwindPoints","hill.tailwindPoints"].indexOf(d.key)>=0 || q==="geometry" && d.key==="hill.takeoffBias"
        return String(question.groups || "").split(",").indexOf(d.group)>=0 && ["athlete","field","hill","rules","clock","analysis"].indexOf(d.group)<0
    }
    function toggle(index){var copy=selected.slice();var at=copy.indexOf(index);if(at<0)copy.push(index);else copy.splice(at,1);selected=copy}
    spacing: Theme.dp(8)
    SectionTabs { Layout.fillWidth: true; options: [qsTr("Dane próby"),qsTr("Skoczkowie"),qsTr("Reguły symulacji"),qsTr("Wykres i mapa")]; currentIndex: controls.panel; onSelected: controls.panel=index }
    ScrollView {
        id: settingsScroll
        Layout.fillWidth: true; Layout.fillHeight: true
        contentWidth: availableWidth; clip: true
        ColumnLayout {
            width: settingsScroll.availableWidth
            spacing: Theme.dp(12)
            ColumnLayout {
                visible: controls.panel===0; Layout.fillWidth: true; spacing: Theme.dp(10)
                Label { text: qsTr("Kto skacze i w jakim konkursie?"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: qsTr("Paczka i zasady są wspólne dla trzech wariantów. Pola parametrów poniżej zmieniają tylko serię „Twoje zmiany”."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: qsTr("Paczka skoczków"); color: Theme.muted; font.pointSize: Theme.caption }
                SChoice { objectName: "balanceLabPack"; Layout.fillWidth: true; model: controls.backend.sources.athletePacks; searchable: true; currentIndex: Math.max(0,model.indexOf(controls.backend.config.rosterName || "")); enabled: !controls.backend.busy; Accessible.name: qsTr("Paczka skoczków"); onActivated: controls.backend.choosePack(currentIndex) }
                Label { text: qsTr("Skocznia"); color: Theme.muted; font.pointSize: Theme.caption }
                SChoice { Layout.fillWidth: true; model: controls.backend.sources.hillNames; currentIndex: -1; displayText: controls.backend.config.hill ? controls.backend.config.hill.name : ""; searchable: true; enabled: !controls.backend.busy; Accessible.name: qsTr("Skocznia"); onActivated: controls.backend.chooseHill(currentIndex) }
                SButton { visible: controls.backend.sources.sourceNotes.length>0; text: qsTr("Uwagi do danych (%1)").arg(controls.backend.sources.sourceNotes.split("\n").filter(function(s){return s.length>0}).length); quiet: true; checkable: true; checked: controls.sourceNotesOpen; onClicked: controls.sourceNotesOpen=!controls.sourceNotesOpen }
                Label { text: controls.backend.sources.sourceNotes; visible: controls.sourceNotesOpen && text.length>0; color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: qsTr("Skoczek, którego wyniki analizujesz"); color: Theme.muted; font.pointSize: Theme.caption }
                SChoice { objectName: "balanceLabFocus"; Layout.fillWidth: true; model: controls.members.map(function(a){return a.name}); searchable: true; currentIndex: controls.backend.config.focus; enabled: !controls.backend.busy; Accessible.name: qsTr("Skoczek, którego wyniki analizujesz"); onActivated: controls.backend.setContext("focus",currentIndex) }
                PlaygroundNumber { Layout.fillWidth: true; caption: qsTr("Liczba konkursów dla każdego wariantu"); value: controls.backend.config.competitions; minimum: 2; maximum: 1000000; step: 100; onCommitted: controls.backend.setContext("competitions",value) }
                PlaygroundNumber { visible: !controls.backend.config.roster; Layout.fillWidth: true; caption: qsTr("Liczba skoczków"); value: controls.backend.config.athletes; minimum: 2; maximum: 512; step: 1; onCommitted: controls.backend.setContext("athletes",value) }
                Label { text: qsTr("Numer losowania — ten sam numer pozwala powtórzyć próbę"); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: qsTr("Pełna próba liczy trzy warianty, więc 500 konkursów oznacza 1500 symulowanych konkursów. Punkty wykresu i komórki mapy to dodatkowe próby z osobnym limitem."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                SField { Layout.fillWidth: true; text: controls.backend.config.seed; Accessible.name: qsTr("Ziarno losowania"); placeholderText: qsTr("Ziarno losowania"); enabled: !controls.backend.busy; onEditingFinished: controls.backend.setContext("seed",text) }
                Label { text: qsTr("Format konkursu"); color: Theme.muted; font.pointSize: Theme.caption }
                SChoice { id: format; Layout.fillWidth: true; currentIndex: controls.backend.config.rulesData && controls.backend.config.rulesData["competition-type"]===1 ? 2 : controls.backend.config.rulesData && controls.backend.config.rulesData.rounds && controls.backend.config.rulesData.rounds[0].ko ? 1 : 0; model: [qsTr("Indywidualny"),qsTr("KO"),qsTr("Drużynowy")]; enabled: !controls.backend.busy; Accessible.name: qsTr("Format konkursu"); onActivated: controls.backend.setFormat(currentIndex,teamSize.currentIndex+2) }
                SChoice { id: teamSize; visible: format.currentIndex===2; Layout.fillWidth: true; model: [qsTr("2 skoczków w drużynie"),qsTr("3 skoczków w drużynie"),qsTr("4 skoczków w drużynie")]; currentIndex: controls.backend.config.rulesData ? Math.max(0,controls.backend.config.rulesData["jumpers-in-team-count"]-2) : 2; enabled: !controls.backend.busy; Accessible.name: qsTr("Wielkość drużyny"); onActivated: controls.backend.setFormat(2,currentIndex+2) }
                SButton { visible: format.currentIndex===2; text: qsTr("Drużyny według krajów"); quiet: true; enabled: !controls.backend.busy; onClicked: controls.backend.setContext("teamAssignments",[]) }
                Label { text: qsTr("Zestaw zasad konkursu"); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                SChoice { Layout.fillWidth: true; model: controls.backend.sources.ruleNames; currentIndex: -1; displayText: controls.backend.config.rulesData && controls.backend.config.rulesData.name ? controls.backend.config.rulesData.name : qsTr("Zasady eksperymentu"); searchable: true; enabled: !controls.backend.busy; Accessible.name: qsTr("Zestaw zasad"); onActivated: controls.backend.chooseRules(currentIndex) }
                Label { text: qsTr("Źródło pogody w konkursach"); color: Theme.muted; font.pointSize: Theme.caption }
                SChoice { Layout.fillWidth: true; model: [qsTr("Ręczne pole wiatru"),qsTr("Automatyczna propozycja w każdym konkursie"),qsTr("Losuj wybrany preset w każdym konkursie")]; currentIndex: controls.backend.config.weatherSource || 0; Accessible.name: qsTr("Źródło pogody"); onActivated: controls.backend.setContext("weatherSource",currentIndex) }
                Label { text: qsTr("Ręczne pole używa parametrów wiatru i opadów. Pozostałe tryby losują pogodę według współczynników generatora."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                SChoice { visible: controls.backend.config.weatherSource===2; Layout.fillWidth: true; model: [qsTr("Stały wiatr w plecy"),qsTr("Silny wiatr pod narty"),qsTr("Loteryjny"),qsTr("Zmienny boczny"),qsTr("Cisza"),qsTr("Stały pod narty"),qsTr("Silny w plecy"),qsTr("Stały boczny"),qsTr("Zmienny pod narty"),qsTr("Zmienny w plecy"),qsTr("Różne kierunki na zeskoku")]; currentIndex: controls.backend.config.weatherPreset || 0; Accessible.name: qsTr("Preset pogody w konkursach"); onActivated: controls.backend.setContext("weatherPreset",currentIndex) }
                Label { text: qsTr("Liczba uczestników w kolejnych seriach"); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                SField { Layout.fillWidth: true; text: controls.backend.config.rulesData && controls.backend.config.rulesData.rounds ? controls.backend.config.rulesData.rounds.map(function(r){return r.count}).join(", ") : (controls.backend.config.rounds || []).join(", "); placeholderText: qsTr("Limity serii, np. 50, 30"); Accessible.name: qsTr("Limity serii"); enabled: !controls.backend.busy; onEditingFinished: {var rounds=text.split(",").map(function(v){return Number(v.trim())});controls.backend.setContext("rounds",rounds)} }
                Label { text: qsTr("Wpisz limity oddzielone przecinkami, np. 50, 30: pierwsza seria do 50 uczestników, druga do 30. W konkursie drużynowym są to drużyny."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Repeater { model: ["hill.k","hill.hs","hill.realHs","hill.perk.0","hill.perk.1","hill.perk.2","hill.perk.3","weather.month","weather.surface","gate","gate.coachReduction","gate.reference","event","clock.mode","clock.interval","dsq","rules.dsq","rules.windCompensation","rules.gateCompensation","rules.measurement","rules.judges","rules.95hs"]; BalanceLabParameter { required property string modelData; backend: controls.backend; definition: controls.definition(modelData) } }
            }
            ColumnLayout {
                visible: controls.panel===1; Layout.fillWidth: true; spacing: Theme.dp(10)
                Label { text: qsTr("Zmieniasz skoczka w serii „Twoje zmiany”"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: qsTr("Wybór analizowanego skoczka zmienisz w „Dane próby”. Edycje tutaj służą tylko eksperymentowi i nie zapisują się do paczki."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Repeater { model: controls.dataKeys; BalanceLabParameter { required property string modelData; backend: controls.backend; definition: controls.definition(modelData) } }
                Label { text: qsTr("Edycja grupy i składy drużyn"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                RowLayout {
                    Layout.fillWidth: true
                    SButton { text: qsTr("Wszyscy"); quiet: true; onClicked: controls.selected=controls.members.map(function(a){return a.index}) }
                    SButton { text: qsTr("Wyczyść wybór"); quiet: true; onClicked: controls.selected=[] }
                }
                SField { id: rosterSearch; Layout.fillWidth: true; placeholderText: qsTr("Filtruj po nazwisku lub kraju…"); Accessible.name: qsTr("Filtr skoczków do edycji grupowej") }
                SButton { text: qsTr("Zaznacz widocznych"); quiet: true; onClicked: controls.selected=controls.members.filter(function(a){return !rosterSearch.text.length || (a.name+" "+a.country).toLowerCase().indexOf(rosterSearch.text.toLowerCase())>=0}).map(function(a){return a.index}) }
                ListView {
                    Layout.fillWidth: true; Layout.preferredHeight: Theme.dp(220); clip: true
                    model: controls.members.filter(function(a){return !rosterSearch.text.length || (a.name+" "+a.country).toLowerCase().indexOf(rosterSearch.text.toLowerCase())>=0}); spacing: 2
                    ScrollBar.vertical: ScrollBar {}
                    delegate: SCheck { required property var modelData; width: ListView.view.width; text: qsTr("%1 (%2) · OVR: %3 · forma: %4").arg(modelData.name).arg(modelData.country).arg(modelData.overall).arg(modelData.form); checked: controls.selected.indexOf(modelData.index)>=0; onClicked: controls.toggle(modelData.index) }
                }
                SButton { text: qsTr("Tylko zaznaczeni w stawce"); enabled: !controls.backend.busy && controls.selected.length>=2; onClicked: {controls.backend.setContext("selectedRoster",controls.selected);controls.selected=[]} }
                Label { text: qsTr("Co zmienić zaznaczonym skoczkom?"); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                SChoice { id: groupKey; Layout.fillWidth: true; model: [qsTr("OVR"),qsTr("Forma"),qsTr("Klasa lądowania"),qsTr("Podejście"),qsTr("Specjalizacja"),qsTr("Regularność")]; Accessible.name: qsTr("Zmieniany parametr grupy") }
                Label { text: qsTr("Jak zastosować zmianę?"); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                SChoice { id: operation; Layout.fillWidth: true; model: [qsTr("Ustaw wszystkim"),qsTr("Dodaj do obecnej wartości")]; Accessible.name: qsTr("Sposób zmiany grupy") }
                PlaygroundNumber { id: groupValue; Layout.fillWidth: true; caption: operation.currentIndex===0 ? qsTr("Nowa wartość dla każdego zaznaczonego skoczka") : qsTr("Ile dodać? Wartość ujemna odejmuje"); value: 10; minimum: -100; maximum: 100; step: 1 }
                SButton { text: qsTr("Zmień zaznaczonych (%1)").arg(controls.selected.length); enabled: !controls.backend.busy && controls.selected.length>0; onClicked: controls.backend.editAthletes(controls.selected,["overall","form","landing","perk0","perk1","perk2"][groupKey.currentIndex],groupValue.currentValue,operation.currentIndex===1) }
                PlaygroundNumber { id: team; Layout.fillWidth: true; caption: qsTr("Numer drużyny testowej"); value: 1; minimum: 1; maximum: 128; step: 1 }
                SButton { text: qsTr("Przypisz zaznaczonych do drużyny"); enabled: !controls.backend.busy && controls.selected.length>0; onClicked: controls.backend.setTeam(controls.selected,team.currentValue-1) }
                Label { text: qsTr("Reprezentacje bez pełnego składu nie startują. Drużyny testowe łączą dowolnych skoczków; nie zmieniają paczki źródłowej."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            }
            ColumnLayout {
                visible: controls.panel===2; Layout.fillWidth: true; spacing: Theme.dp(10)
                Label { text: qsTr("Parametry istotne dla tego tematu"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: qsTr("Zmieniaj jeden parametr i obserwuj podgląd. Opis pod polem wyjaśnia jego działanie. Pełna próba pozwala sprawdzić, czy mała różnica jest powtarzalna."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                SCheck { text: qsTr("Pokaż wszystkie parametry symulacji"); checked: controls.fullRegistry; onClicked: controls.fullRegistry=!controls.fullRegistry }
                SField { id: search; Layout.fillWidth: true; placeholderText: qsTr("Szukaj, np. upadek, forma, podmuch…"); Accessible.name: qsTr("Szukaj parametru") }
                SChoice { visible: controls.question.id==="generator"; Layout.fillWidth: true; model: [qsTr("Automatyczna propozycja"),qsTr("Stały wiatr w plecy"),qsTr("Silny wiatr pod narty"),qsTr("Loteryjny"),qsTr("Zmienny boczny"),qsTr("Cisza"),qsTr("Stały pod narty"),qsTr("Silny w plecy"),qsTr("Stały boczny"),qsTr("Zmienny pod narty"),qsTr("Zmienny w plecy"),qsTr("Różne kierunki na zeskoku")]; Accessible.name: qsTr("Badany preset pogody"); onActivated: controls.generatorPreset=currentIndex-1 }
                Loader {
                    active: controls.panel===2
                    Layout.fillWidth: true
                    sourceComponent: Component {
                        ColumnLayout {
                            spacing: Theme.dp(10)
                            Repeater { model: controls.definitions.filter(function(d){return controls.relevant(d) && (!search.text.length || (d.label+" "+d.key).toLowerCase().indexOf(search.text.toLowerCase())>=0)}); BalanceLabParameter { required property var modelData; backend: controls.backend; definition: modelData } }
                        }
                    }
                }
            }
            ColumnLayout {
                visible: controls.panel===3; Layout.fillWidth: true; spacing: Theme.dp(10)
                Label { text: qsTr("Jakie wartości chcesz porównać?"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                Label { text: qsTr("Wykres testuje wartości jednego parametru. Mapa testuje każdą parę wartości z dwóch osi. Parametry osi są zmieniane we wszystkich trzech wariantach; pozostałe różnice między wariantami zostają."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                SButton { text: qsTr("OVR × forma"); quiet: true; onClicked: {controls.backend.setOption("axisX","athlete.overall");controls.backend.setOption("fromX",40);controls.backend.setOption("toX",100);controls.backend.setOption("axisY","athlete.form");controls.backend.setOption("fromY",20);controls.backend.setOption("toY",100)} }
                SButton { text: qsTr("Belka × wiatr"); quiet: true; onClicked: {controls.backend.setOption("axisX","gate");controls.backend.setOption("fromX",6);controls.backend.setOption("toX",14);controls.backend.setOption("axisY","wind.mean");controls.backend.setOption("fromY",-4);controls.backend.setOption("toY",4)} }
                Label { text: qsTr("Poziomo — zmieniany parametr"); color: Theme.muted; font.pointSize: Theme.caption }
                SChoice { Layout.fillWidth: true; model: controls.axisLabels; searchable: true; currentIndex: Math.max(0,controls.keys.indexOf(controls.backend.options.axisX)); Accessible.name: qsTr("Parametr osi X"); onActivated: controls.backend.setOption("axisX",controls.keys[currentIndex]) }
                PlaygroundNumber { Layout.fillWidth: true; caption: qsTr("Najmniejsza wartość na osi poziomej"); value: controls.backend.options.fromX; minimum: -100000; maximum: 100000; decimals: 3; onCommitted: controls.backend.setOption("fromX",value) }
                PlaygroundNumber { Layout.fillWidth: true; caption: qsTr("Największa wartość na osi poziomej"); value: controls.backend.options.toX; minimum: -100000; maximum: 100000; decimals: 3; onCommitted: controls.backend.setOption("toX",value) }
                Label { text: qsTr("Pionowo — drugi parametr mapy"); color: Theme.muted; font.pointSize: Theme.caption }
                SChoice { Layout.fillWidth: true; model: controls.axisLabels; searchable: true; currentIndex: Math.max(0,controls.keys.indexOf(controls.backend.options.axisY)); Accessible.name: qsTr("Parametr osi Y"); onActivated: controls.backend.setOption("axisY",controls.keys[currentIndex]) }
                PlaygroundNumber { Layout.fillWidth: true; caption: qsTr("Najmniejsza wartość na osi pionowej"); value: controls.backend.options.fromY; minimum: -100000; maximum: 100000; decimals: 3; onCommitted: controls.backend.setOption("fromY",value) }
                PlaygroundNumber { Layout.fillWidth: true; caption: qsTr("Największa wartość na osi pionowej"); value: controls.backend.options.toY; minimum: -100000; maximum: 100000; decimals: 3; onCommitted: controls.backend.setOption("toY",value) }
                PlaygroundNumber { Layout.fillWidth: true; caption: qsTr("Liczba testowanych wartości na każdej osi"); value: controls.backend.options.resolution; minimum: 3; maximum: 15; step: 2; onCommitted: controls.backend.setOption("resolution",value) }
                Label { text: qsTr("Mapa %1 × %1 = %2 kombinacji parametrów. Więcej wartości oznacza gęstszą mapę i dłuższe obliczenia.").arg(controls.backend.options.resolution).arg(controls.backend.options.resolution * controls.backend.options.resolution); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                SButton { objectName: "balanceLabMatrix"; text: qsTr("Uruchom porównanie na mapie"); primary: true; enabled: !controls.backend.busy; onClicked: controls.backend.start(true) }
                Label { text: qsTr("Kliknij komórkę, aby przenieść wartości do bieżącego wariantu. Liczba prób na komórkę jest pokazana przy mapie."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            }
        }
    }
}
