import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: workspace
    objectName: "raceOptionsWorkspace"
    property int section: 0
    onSectionChanged: { scroll.cancelFlick(); scroll.contentY = 0 }
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
    Flickable {
        id: scroll
        objectName: "raceOptionsScroll"
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
        contentWidth: width
        contentHeight: optionsContent.height
        readonly property bool overflow: contentHeight > height
        interactive: overflow
        boundsBehavior: Flickable.StopAtBounds
        flickableDirection: Flickable.VerticalFlick
        clip: true
        onOverflowChanged: if (!overflow) { cancelFlick(); contentY = 0 }
        ScrollBar.vertical: ScrollBar { policy: scroll.overflow ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff }
            Column {
                id: optionsContent
                width: parent.width; spacing: 12
                RaceSimulation { visible: workspace.section===0; width: parent.width; onStarted: workspace.closeRequested() }
                Repeater {
                    model: workspace.section===0 ? ["toolButton_cancelActions","pushButton_sendResultsWebhook"] : workspace.section===1 ? ["pushButton_generateNewWinds","pushButton_windsGeneratorSettings","pushButton_inrunSnowGeneratorSettings"] : ["pushButton_coachGate","pushButton_manipulateJump","label_record","label_pointsBehindLeader","label_lastRoundPosition","label_personalBest"]
                    Node { width: parent.width; height: implicitHeight; node: ui.find(modelData) }
                }
                Loader {
                    active: workspace.section===2; visible: active; width: parent.width
                    height: active && item ? item.implicitHeight : 0
                    sourceComponent: Component { Node { node: ui.find("quickRaceDetails") } }
                }
                Loader {
                    active: workspace.section===0; visible: active; width: parent.width
                    height: active && item ? item.implicitHeight : 0
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
