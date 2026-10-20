import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import SjSim 1.0

Rectangle {
    id: panel
    property var chart
    implicitHeight: 330
    color: Theme.surface
    radius: Theme.radius
    activeFocusOnTab: true
    Keys.onLeftPressed: plot.inspect(-1)
    Keys.onRightPressed: plot.inspect(1)
    Keys.onPressed: if (event.key === Qt.Key_Home) { plot.resetZoom(); event.accepted = true }
    border.color: panel.activeFocus ? Theme.accent : Theme.outline
    ColumnLayout {
        anchors.fill: parent; anchors.margins: 12
        spacing: 4
        SimulationPlot {
            id: plot
            chart: panel.chart
            ink: Theme.ink; line: Theme.outline; accent: Theme.accent
            Layout.fillWidth: true; Layout.fillHeight: true
            Accessible.role: Accessible.Chart
            Accessible.name: qsTr("Wykres statystyk")
            Accessible.description: hoverText
        }
        RowLayout {
            Layout.fillWidth: true
            Label { text: plot.hoverText || qsTr("Przeciągnij, aby przybliżyć · strzałki: punkty"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            SButton { text: qsTr("Cały wykres"); quiet: true; onClicked: plot.resetZoom() }
        }
    }
}
