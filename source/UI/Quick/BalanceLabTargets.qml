import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: targets
    objectName: "balanceLabTargets"
    property var backend
    readonly property var rows: backend.config.labTargets || []
    readonly property var targetResults: backend.fullResult.targets || []
    readonly property var keys: ["pairedDistance","pairedPoints","pairedPlace","meanDistance","meanPlace","meanPoints","fallProbability"]
    function change(index,key,value){var copy=JSON.parse(JSON.stringify(rows));copy[index][key]=value;backend.setTargets(copy)}
    function state(s){return s==="pass"?qsTr("Spełniony"):s==="fail"?qsTr("Niespełniony"):qsTr("Za mało pewności, aby ocenić")}
    function number(v){return typeof v==="number"?v.toLocaleString(Qt.locale(Theme.localeName),'f',2):"—"}
    spacing: Theme.dp(12)
    Label { text: qsTr("Cele balansu"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section }
    Label { text: qsTr("Średnie oceniamy przedziałem 95% z pełnej próby. Ryzyko upadku przy zadanej odległości jest dokładną wartością modelu. Zmieniony cel wymaga ponownego sprawdzenia."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    Label { text: qsTr("Ocena celu średniej wymaga co najmniej 30 powtórzeń. Przy mniejszej próbie wynik pozostaje nierozstrzygnięty."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    Label { text: qsTr("Przykład: wybierz średnią odległość i wpisz oczekiwany zakres 130–135 m. Wynik oceny pojawi się po pełnej próbie. Dla różnicy miejsc wartość ujemna oznacza poprawę."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    Repeater {
        model: targets.rows
        Pane {
            id: goalRow
            required property var modelData
            required property int index
            Layout.fillWidth: true; padding: Theme.dp(10)
            background: Rectangle { color: Theme.surface; radius: Theme.dp(6) }
            contentItem: ColumnLayout {
                spacing: 8
                Label { text: qsTr("Nazwa — co chcesz osiągnąć?"); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    SField { Layout.fillWidth: true; text: modelData.name; Accessible.name: qsTr("Nazwa celu"); enabled: !targets.backend.busy; onEditingFinished: targets.change(goalRow.index,"name",text) }
                Label { text: qsTr("Co ma spełniać oczekiwany zakres?"); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    SChoice { Layout.fillWidth: true; model: [qsTr("Różnica odległości względem gry"),qsTr("Różnica punktów względem gry"),qsTr("Różnica miejsca względem gry"),qsTr("Średnia odległość"),qsTr("Średnie miejsce"),qsTr("Średnie punkty za skok"),qsTr("Ryzyko upadku na zadanej odległości · %")]; currentIndex: targets.keys.indexOf(modelData.metric); Accessible.name: qsTr("Miara celu"); enabled: !targets.backend.busy; onActivated: {targets.change(goalRow.index,"distance",targets.backend.values["hill.hs"] || 134);targets.change(goalRow.index,"metric",targets.keys[currentIndex])} }
                Label { text: qsTr("Wariant oceniany przez ten cel"); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    SChoice { Layout.fillWidth: true; model: [qsTr("Balans z gry"),qsTr("Zapamiętane ustawienia"),qsTr("Twoje zmiany")]; currentIndex: modelData.variant; Accessible.name: qsTr("Wariant celu"); enabled: !targets.backend.busy; onActivated: targets.change(goalRow.index,"variant",currentIndex) }
                SButton { text: modelData.scenarioId ? qsTr("Zapisz ponownie dane scenariusza") : qsTr("Zachowaj te dane do kolejnych kontroli celu"); quiet: true; enabled: !targets.backend.busy; onClicked: targets.backend.setContext("captureTarget",goalRow.index) }
                Label { visible: !!modelData.scenarioId; text: qsTr("Dane skoczków, skoczni i pogody są zapisane. Przy kolejnych kontrolach scenariusz użyje aktualnie strojonych współczynników silnika."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                GridLayout {
                    Layout.fillWidth: true; columns: width>600?3:1
                    PlaygroundNumber { visible: modelData.metric==="fallProbability"; Layout.fillWidth: true; caption: qsTr("Odległość · m"); value: modelData.distance || 134; minimum: 0; maximum: 1000; decimals: 1; onCommitted: targets.change(goalRow.index,"distance",value) }
                    PlaygroundNumber { Layout.fillWidth: true; caption: qsTr("Dolna granica oczekiwanego wyniku"); value: modelData.minimum; minimum: -10000; maximum: 10000; decimals: 2; onCommitted: targets.change(goalRow.index,"minimum",value) }
                    PlaygroundNumber { Layout.fillWidth: true; caption: qsTr("Górna granica oczekiwanego wyniku"); value: modelData.maximum; minimum: -10000; maximum: 10000; decimals: 2; onCommitted: targets.change(goalRow.index,"maximum",value) }
                }
                SButton { text: qsTr("Usuń cel"); quiet: true; enabled: !targets.backend.busy; onClicked: {var copy=targets.rows.slice();copy.splice(goalRow.index,1);targets.backend.setTargets(copy)} }
            }
        }
    }
    SButton { text: qsTr("Dodaj cel"); enabled: !targets.backend.busy && targets.rows.length<64; onClicked: {var copy=targets.rows.slice();copy.push({name:qsTr("Nowy cel"),variant:2,metric:"pairedDistance",minimum:0,maximum:5,distance:targets.backend.values["hill.hs"] || 134});targets.backend.setTargets(copy)} }
    Label { visible: targets.backend.stale; text: qsTr("Wyniki celów dotyczą wcześniejszych ustawień. Uruchom pełną próbę."); color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    Repeater {
        model: targets.targetResults
        Label { required property var modelData; text: modelData.name+" · "+targets.state(modelData.state)+(modelData.previousState ? " · "+qsTr("Poprzednio: %1").arg(targets.state(modelData.previousState)) : "")+"\n"+qsTr("Wynik: %1 · przedział 95%: %2–%3 · cel: %4–%5").arg(targets.number(modelData.result?modelData.result.mean:undefined)).arg(targets.number(modelData.result?modelData.result.ciLow:undefined)).arg(targets.number(modelData.result?modelData.result.ciHigh:undefined)).arg(targets.number(modelData.minimum)).arg(targets.number(modelData.maximum)); color: modelData.state==="fail"?Theme.warning:Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    }
}
