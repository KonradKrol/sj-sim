import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: workspace
    objectName: "weatherWorkspace"
    property var node
    readonly property var backend: node.values.backend
    readonly property var values: backend.values
    property bool tuning: false
    readonly property var presets: [
        {name:qsTr("Stabilny w plecy"),symbol:"↘",detail:qsTr("Małe wahania"),help:qsTr("Siła wyjściowa 0,6–2,3 m/s. Kierunek i siła zmieniają się nieznacznie.")},
        {name:qsTr("Mocno pod narty"),symbol:"↖",detail:qsTr("Silny wiatr przedni"),help:qsTr("Siła wyjściowa 2–4 m/s. Silny wiatr nie oznacza dużej loteryjności.")},
        {name:qsTr("Loteria"),symbol:"↔",detail:qsTr("Duże i szybkie zmiany"),help:qsTr("Duże różnice lokalne, silne podmuchy i możliwe odwrócenia kierunku. Każde losowanie daje inny przebieg.")},
        {name:qsTr("Zmienny boczny"),symbol:"↗",detail:qsTr("Trudne warunki"),help:qsTr("Siła wyjściowa 1–3 m/s. Wiatr boczny z wyraźnymi wahaniami i podmuchami.")},
        {name:qsTr("Cisza"),symbol:"—",detail:qsTr("Bez wiatru"),help:qsTr("Brak wiatru przez całą symulację. Opady ustawiasz osobno.")}
    ]
    readonly property string scenarioName: values.modified ? qsTr("Własne warunki") : values.preset>=0 ? presets[values.preset].name : qsTr("Propozycja skoczni")
    readonly property string variabilityText: values.staticWind ? qsTr("Wiatr nie zmienia się w czasie.") : values.variability<20 ? qsTr("Niewielkie wahania; kierunek zwykle pozostaje podobny.") : values.variability<60 ? qsTr("Warunki wyraźnie się zmieniają, czasem przez kilka kolejnych skoków.") : qsTr("Duże, szybkie zmiany; możliwe silne podmuchy i odwrócenia kierunku.")
    contentWidth: availableWidth; contentHeight: values.ready ? editor.implicitHeight : availableHeight; clip: true
    Control {
        id: editor
        visible: workspace.values.ready
        height: visible ? implicitHeight : 0
        width: workspace.availableWidth
        padding: Theme.dp(10)
        background: Item {}
        contentItem: ColumnLayout {
            spacing: Theme.dp(10)
            RowLayout {
                Layout.fillWidth: true
                Label { text: qsTr("Wiatr"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.title; font.bold: true; Layout.fillWidth: true }
                Image { source: workspace.values.flag || ""; sourceSize.width: 60; sourceSize.height: 40; fillMode: Image.PreserveAspectFit; Layout.preferredWidth: Theme.dp(28); Layout.preferredHeight: Theme.dp(18); Accessible.ignored: true }
                Label { visible: workspace.availableWidth>Theme.dp(700); text: qsTr("%1 · HS %2").arg(workspace.values.hillName).arg(workspace.values.hs); color: Theme.muted; font.pointSize: Theme.caption; elide: Text.ElideRight; Layout.maximumWidth: Theme.dp(260) }
                SButton { objectName: "weatherSuggest"; text: qsTr("Propozycja skoczni"); quiet: true; enabled: workspace.values.ready; hint: qsTr("Wylosuj wiatr i opady z uwzględnieniem skoczni i jej perków."); onClicked: {workspace.forceActiveFocus();workspace.backend.suggest()} }
            }
            SChoice {
                visible: workspace.availableWidth<Theme.dp(600)
                Layout.fillWidth: true
                model: workspace.presets.map(function(preset){return preset.name})
                currentIndex: workspace.values.modified ? -1 : workspace.values.preset
                displayText: workspace.scenarioName
                objectName: "weatherPresetPicker"
                enabled: workspace.values.ready
                Accessible.name: qsTr("Scenariusz wiatru")
                onActivated: {workspace.forceActiveFocus();workspace.backend.applyPreset(index)}
            }
            GridLayout {
                visible: workspace.availableWidth>=Theme.dp(600)
                Layout.fillWidth: true; columns: width>=Theme.dp(900)?5:width>=Theme.dp(540)?3:2
                columnSpacing: Theme.dp(8); rowSpacing: Theme.dp(8)
                Repeater {
                    model: 5
                    WeatherPresetChoice {
                        required property int index
                        objectName: "weatherPreset-"+index
                        Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: 0
                        title: workspace.presets[index].name; symbol: workspace.presets[index].symbol; detail: workspace.presets[index].detail; explanation: workspace.presets[index].help
                        enabled: workspace.values.ready
                        checked: workspace.values.preset===index && !workspace.values.modified
                        onClicked: {workspace.forceActiveFocus();workspace.backend.applyPreset(index)}
                    }
                }
            }
            GridLayout {
                Layout.fillWidth: true; columns: width>=Theme.dp(1050)?2:1
                columnSpacing: Theme.dp(12); rowSpacing: Theme.dp(10)
                ColumnLayout {
                    Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: Theme.dp(760); Layout.alignment: Qt.AlignTop
                    spacing: Theme.dp(6)
                    WindSlopePreview { Layout.fillWidth: true; samples: workspace.backend.preview; heading: workspace.scenarioName }
                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: workspace.variabilityText; color: Theme.muted; font.pointSize: Theme.body; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                        SButton { objectName: "weatherReroll"; text: qsTr("Losuj ponownie"); enabled: workspace.values.ready; onClicked: {workspace.forceActiveFocus();workspace.backend.reroll()} }
                    }
                }
                WeatherPrecipitation {
                    backend: workspace.backend
                    Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: Theme.dp(260); Layout.alignment: Qt.AlignTop
                }
            }
            RowLayout {
                Layout.fillWidth: true
                SButton { objectName: "weatherTuningToggle"; text: workspace.tuning ? qsTr("Ukryj dostosowanie") : qsTr("Dostosuj"); quiet: true; enabled: workspace.values.ready; onClicked: {workspace.tuning=!workspace.tuning;if(!workspace.tuning)workspace.backend.previewSeconds=0} }
                Item { Layout.fillWidth: true }
                readonly property var gateLabel: ui.find("label_suggestedGate")
                readonly property var autoGate: ui.find("pushButton_autoGate")
                Label { text: parent.gateLabel ? parent.gateLabel.values.text || "" : ""; color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.maximumWidth: Theme.dp(230) }
                SButton { text: qsTr("Zastosuj"); quiet: true; visible: ui.context.suggestedGate>=0; enabled: !!parent.autoGate && ui.context.suggestedGate!==ui.context.startingGate; onClicked: {workspace.forceActiveFocus();ui.activate(parent.autoGate.values.id)} }
                SChoice {
                    visible: workspace.tuning && workspace.availableWidth>=Theme.dp(700)
                    model: [workspace.backend.previewOrigin>0?qsTr("Teraz"):qsTr("Na początku"),qsTr("Za 3 minuty"),qsTr("Za 6 minut")]
                    Accessible.name: qsTr("Czas podglądu wiatru")
                    currentIndex: Math.round(workspace.backend.previewSeconds/180)
                    onActivated: workspace.backend.previewSeconds=index*180
                }
            }
            SChoice {
                visible: workspace.tuning && workspace.availableWidth<Theme.dp(700)
                model: [workspace.backend.previewOrigin>0?qsTr("Teraz"):qsTr("Na początku"),qsTr("Za 3 minuty"),qsTr("Za 6 minut")]
                Accessible.name: qsTr("Czas podglądu wiatru")
                currentIndex: Math.round(workspace.backend.previewSeconds/180)
                onActivated: workspace.backend.previewSeconds=index*180
            }
            Loader {
                active: workspace.tuning; visible: active
                Layout.fillWidth: true
                Layout.preferredHeight: status===Loader.Ready ? item.implicitHeight : 0
                sourceComponent: Component { WeatherTuning { backend: workspace.backend } }
            }
            Label { visible: !!text; text: workspace.backend.message; color: Theme.danger; font.pointSize: Theme.body; wrapMode: Text.WordWrap; Layout.fillWidth: true }


        }
    }    EmptyState {
        visible: !workspace.values.ready
        width: workspace.availableWidth; height: visible ? workspace.availableHeight : 0
        title: qsTr("Nie wybrano skoczni")
        description: qsTr("Wybierz paczkę i skocznię w konfiguracji symulacji.")
        actionText: qsTr("Konfiguracja")
        onTriggered: ui.edit(ui.root.values.id,0)
    }

}
