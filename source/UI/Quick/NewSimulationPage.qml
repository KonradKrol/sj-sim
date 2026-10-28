import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: page
    objectName: "newSimulationPage"
    readonly property bool dense: height<650 || Theme.compact
    readonly property var submit: ui.find("pushButton_submit")
    ColumnLayout {
        anchors.fill: parent; anchors.margins: Theme.compact ? 8 : 20
        spacing: page.dense ? 10 : Theme.gap
        ScrollView {
            id: scroll
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
            contentWidth: availableWidth; clip: true
            ColumnLayout {
                width: scroll.availableWidth; spacing: page.dense ? 10 : Theme.gap
                Pane {
                    Layout.fillWidth: true; padding: page.dense ? 12 : 24
                    background: Surface { fillColor: Theme.stadium; sweep: 28 }
                    contentItem: ColumnLayout {
                        spacing: 8
                        Label { text: qsTr("Nowa symulacja"); color: Theme.snow; font.family: Theme.displayFamily; font.pointSize: page.dense ? Theme.heading : Theme.heading*1.5; font.weight: Font.ExtraBold; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                        Label { text: qsTr("Wybierz dane sezonu. Kalendarz utworzysz w kolejnym kroku."); color: Theme.headerMuted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                    }
                }
                GridLayout {
                    Layout.fillWidth: true; columns: page.width<600 ? 1 : 2
                    columnSpacing: 16; rowSpacing: Theme.gap
                    ColumnLayout { Layout.fillWidth: true; Label { text: qsTr("Nazwa symulacji"); color: Theme.muted; font.pointSize: Theme.caption } Node { showLabel: false; node: ui.find("lineEdit_name"); Layout.fillWidth: true } }
                    ColumnLayout { Layout.fillWidth: true; Label { text: qsTr("Pierwszy sezon"); color: Theme.muted; font.pointSize: Theme.caption } Node { showLabel: false; node: ui.find("spinBox_seasonNumber"); Layout.fillWidth: true } }
                }
                GridLayout {
                    Layout.fillWidth: true; columns: page.width>=780 ? 3 : 1
                    columnSpacing: 12; rowSpacing: 12
                    Repeater {
                        model: [qsTr("Skoczkowie"),qsTr("Skocznie"),qsTr("Zasady konkursów")]
                        Pane {
                            Layout.fillWidth: true; Layout.minimumWidth: 0; padding: page.dense ? 10 : 16
                            background: Surface { fillColor: Theme.surface; sweep: 22 }
                            contentItem: ColumnLayout {
                                spacing: 10
                                Label { text: modelData; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                                Node { showLabel: false; node: ui.find("simulationPack"+index); Layout.fillWidth: true }
                                Node { node: ui.find("simulationPackCount"+index); Layout.fillWidth: true }
                            }
                        }
                    }
                }
                Node { node: ui.find("simulationCatalogStatus"); Layout.fillWidth: true }
                Node { node: ui.find("simulationCatalogDetails"); Layout.alignment: Qt.AlignLeft }
                Node { node: ui.find("newSeasonDates"); Layout.fillWidth: true }
                Node { node: ui.find("rememberSimulationPacks"); Layout.fillWidth: true }
            }
        }
        Node { node: page.submit; Layout.fillWidth: true; Layout.maximumWidth: 420; Layout.alignment: Qt.AlignRight }
    }
    Shortcut { sequence: "Ctrl+Return"; enabled: page.submit && page.submit.values.enabled; onActivated: {page.forceActiveFocus();ui.activate(page.submit.values.id)} }
}
