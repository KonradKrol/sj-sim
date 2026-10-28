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
    readonly property string scenarioName: values.modified ? qsTr("Własne warunki") : values.preset>=0 ? backend.presets[values.preset] : qsTr("Wylosowane dla skoczni")
    readonly property string variabilityText: values.staticWind ? qsTr("Wiatr nie zmienia się w czasie.") : values.variability<20 ? qsTr("Niewielkie wahania; kierunek zwykle pozostaje podobny.") : values.variability<60 ? qsTr("Warunki wyraźnie się zmieniają, czasem przez kilka kolejnych skoków.") : qsTr("Duże, szybkie zmiany; możliwe silne podmuchy i odwrócenia kierunku.")
    contentWidth: availableWidth
    contentHeight: values.ready ? editor.implicitHeight : availableHeight
    clip: true
    Control {
        id: editor
        visible: workspace.values.ready
        height: visible ? implicitHeight : 0
        width: workspace.availableWidth
        padding: Theme.dp(10)
        background: Item {}
        contentItem: ColumnLayout {
            spacing: Theme.dp(12)
            RowLayout {
                Layout.fillWidth: true
                Label { text: qsTr("Warunki"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.title; font.bold: true; Layout.fillWidth: true }
                Image { source: workspace.values.flag || ""; sourceSize.width: 60; sourceSize.height: 40; fillMode: Image.PreserveAspectFit; Layout.preferredWidth: Theme.dp(28); Layout.preferredHeight: Theme.dp(18); Accessible.ignored: true }
                Label { visible: workspace.availableWidth>Theme.dp(480); text: qsTr("%1 · HS%2").arg(workspace.values.hillName).arg(workspace.values.hs); color: Theme.muted; font.pointSize: Theme.caption; elide: Text.ElideRight; Layout.maximumWidth: Theme.dp(260) }
            }
            RowLayout {
                Layout.fillWidth: true; spacing: Theme.dp(10)
                WeatherPresetChoice {
                    objectName: "weatherSuggest"
                    Layout.fillWidth: true; Layout.minimumWidth: 0
                    title: qsTr("Według skoczni"); symbol: "↻"
                    detail: qsTr("Losowanie uwzględnia perki skoczni")
                    explanation: qsTr("Wylosuj wiatr i opady dopasowane do skoczni.")
                    checked: workspace.values.preset<0 && !workspace.values.modified
                    onClicked: {workspace.forceActiveFocus();workspace.backend.suggest()}
                }
                SButton {
                    objectName: "weatherReroll"
                    text: qsTr("Losuj ponownie"); primary: true
                    enabled: workspace.values.ready
                    onClicked: {workspace.forceActiveFocus();workspace.backend.reroll()}
                }
            }
            WeatherPresetGrid { backend: workspace.backend; Layout.fillWidth: true }
            GridLayout {
                Layout.fillWidth: true; columns: width>=Theme.dp(1100)?2:1
                columnSpacing: Theme.dp(14); rowSpacing: Theme.dp(10)
                ColumnLayout {
                    Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: Theme.dp(760); Layout.alignment: Qt.AlignTop
                    spacing: Theme.dp(7)
                    WindSlopePreview { Layout.fillWidth: true; samples: workspace.backend.preview; heading: workspace.scenarioName }
                    Label { text: workspace.variabilityText; color: Theme.muted; font.pointSize: Theme.body; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                    SButton { objectName: "weatherTuningToggle"; text: workspace.tuning ? qsTr("Ukryj dostosowanie") : qsTr("Dostosuj wiatr"); quiet: true; enabled: workspace.values.ready; onClicked: workspace.tuning=!workspace.tuning }
                    Loader {
                        active: workspace.tuning; visible: active
                        Layout.fillWidth: true
                        Layout.preferredHeight: status===Loader.Ready ? item.implicitHeight : 0
                        sourceComponent: Component { WeatherTuning { backend: workspace.backend } }
                    }
                }
                WeatherPrecipitation { backend: workspace.backend; Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: Theme.dp(290); Layout.alignment: Qt.AlignTop }
            }
            RowLayout {
                Layout.fillWidth: true
                readonly property var gateLabel: ui.find("label_suggestedGate")
                readonly property var autoGate: ui.find("pushButton_autoGate")
                Label { text: parent.gateLabel ? parent.gateLabel.values.text || "" : ""; color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                SButton { text: qsTr("Zastosuj belkę"); quiet: true; visible: ui.context.suggestedGate>=0; enabled: !!parent.autoGate && ui.context.suggestedGate!==ui.context.startingGate; onClicked: {workspace.forceActiveFocus();ui.activate(parent.autoGate.values.id)} }
            }
            Label { visible: !!text; text: workspace.backend.message; color: Theme.danger; font.pointSize: Theme.body; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        }
    }
    EmptyState {
        visible: !workspace.values.ready
        width: workspace.availableWidth; height: visible ? workspace.availableHeight : 0
        title: qsTr("Nie wybrano skoczni")
        description: qsTr("Wybierz paczkę i skocznię w konfiguracji symulacji.")
        actionText: qsTr("Konfiguracja")
        onTriggered: ui.edit(ui.root.values.id,0)
    }
}
