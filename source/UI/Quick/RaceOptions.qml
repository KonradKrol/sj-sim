import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: workspace
    objectName: "raceOptionsWorkspace"
    property int section: 0
    signal closeRequested()
    spacing: 12
    RowLayout {
        Layout.fillWidth: true
        Label { text: qsTr("Opcje konkursu"); color: Theme.ink; font.pointSize: Theme.body; font.weight: Font.DemiBold; Layout.fillWidth: true }
        SButton { id: closeButton; text: "×"; quiet: true; Accessible.name: qsTr("Zamknij"); onClicked: workspace.closeRequested() }
    }
    SectionTabs {
        Layout.fillWidth: true
        options: [qsTr("Przebieg"),qsTr("Warunki"),qsTr("Skok")]
        currentIndex: workspace.section; onSelected: workspace.section=index
    }
    ScrollView {
        id: scroll
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
        contentWidth: availableWidth; clip: true
        Column {
            width: scroll.availableWidth; spacing: 12
            Repeater {
                model: workspace.section===0 ? ["toolButton_autoSimulationActions","toolButton_cancelActions","pushButton_sendResultsWebhook"] : workspace.section===1 ? ["pushButton_generateNewWinds","pushButton_windsGeneratorSettings","pushButton_inrunSnowGeneratorSettings"] : ["pushButton_coachGate","pushButton_manipulateJump","label_record","label_pointsBehindLeader","label_lastRoundPosition","label_personalBest"]
                Node { width: parent.width; height: implicitHeight; node: ui.find(modelData) }
            }
            Loader {
                active: workspace.section===2; width: parent.width
                sourceComponent: Component { Node { node: ui.find("quickRaceDetails") } }
            }
            Loader {
                active: workspace.section===0; width: parent.width
                sourceComponent: Component {
                    Column {
                        spacing: 8
                        Node { width: parent.width; height: implicitHeight; node: ui.find("comboBox_virtualClassification") }
                        Node { width: parent.width; height: implicitHeight; node: ui.find("quickVirtualResults") }
                    }
                }
            }
        }
    }
    function focusClose() {closeButton.forceActiveFocus()}
    Keys.onEscapePressed: workspace.closeRequested()
}
