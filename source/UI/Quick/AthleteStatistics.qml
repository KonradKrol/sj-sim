import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: stats
    property var node
    readonly property var backend: node.values.backend
    readonly property var report: backend.report
    property int metric: 0
    property bool filtersOpen: false
    onMetricChanged: {plot.selectedPoint=0;if(metric<3)backend.previewPoint(metric,0)}
    Component.onCompleted: backend.previewPoint(0,0)
    spacing: Theme.dp(12)
    RowLayout {
        Layout.fillWidth: true
        Image { visible: source.toString().length>0; source: stats.report.photo || ""; Layout.preferredWidth: Theme.dp(48); Layout.preferredHeight: Theme.dp(60); fillMode: Image.PreserveAspectFit }
        ColumnLayout {
            Layout.fillWidth: true; spacing: Theme.dp(2)
            Label { text: stats.report.name; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.title; font.bold: true; Layout.fillWidth: true; elide: Text.ElideRight }
            RowLayout { Image { source: stats.report.flag || ""; Layout.preferredWidth: Theme.dp(24); Layout.preferredHeight: Theme.dp(16); fillMode: Image.PreserveAspectFit } Label { text: stats.report.country; color: Theme.muted; font.pointSize: Theme.caption } }
        }
        SButton { text: qsTr("Filtry i eksport"); quiet: true; checkable: true; checked: stats.filtersOpen; onClicked: stats.filtersOpen=!stats.filtersOpen }
    }
    ScrollView {
        visible: stats.filtersOpen
        Layout.fillWidth: true; Layout.preferredHeight: Math.min(Theme.dp(240),stats.height*.35)
        contentWidth: availableWidth; clip: true
        Loader { width: parent.width; source: Qt.resolvedUrl("Node.qml"); onLoaded: item.node=Qt.binding(function(){return stats.node.values.filters}) }
    }
    GridLayout {
        Layout.fillWidth: true; columns: width>Theme.dp(500)?4:2
        columnSpacing: Theme.dp(16); rowSpacing: Theme.dp(8)
        Repeater {
            model: [{label:qsTr("Najlepsze miejsce"),value:stats.report.best},{label:qsTr("Średnie miejsce"),value:stats.report.mean},{label:qsTr("Najgorsze miejsce"),value:stats.report.worst},{label:qsTr("Odchylenie"),value:stats.report.deviation}]
            ColumnLayout {
                required property var modelData
                Layout.fillWidth: true; spacing: Theme.dp(2)
                Label { text: modelData.label; color: Theme.muted; font.pointSize: Theme.caption }
                Label { text: modelData.value; color: Theme.ink; font.family: Theme.mono; font.pointSize: Theme.section; font.bold: true }
            }
        }
    }
    SChoice { Layout.fillWidth: true; model: stats.report.plots.map(function(p){return p.name}); currentIndex: stats.metric; onActivated: stats.metric=currentIndex }
    BalanceLabPlot {
        id: plot
        onSelectedPointChanged: if(stats.metric<3)stats.backend.previewPoint(stats.metric,selectedPoint)
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: Theme.dp(200)
        invertY: stats.metric===0
        precision: stats.metric===0 || stats.metric===2 ? 0 : stats.metric===4 ? 1 : 2
        yPadding: stats.metric===0 ? (maxY>minY ? 0 : .5) : NaN
        caption: ""
        xLabel: qsTr("Kolejne występy"); yLabel: stats.report.plots[stats.metric].unit || ""
        colors: [Theme.accent]
        series: [{name:stats.report.plots[stats.metric].name,points:stats.report.plots[stats.metric].points,colorIndex:0}]
        visible: stats.report.plots[stats.metric].points.length>0
    }
    Label {
        Layout.fillWidth: true; wrapMode: Text.WordWrap; maximumLineCount: 3; elide: Text.ElideRight
        text: stats.metric<3 ? stats.backend.pointDescription : ((stats.report.plots[stats.metric].points[plot.selectedPoint] || {}).label || "")
        color: Theme.muted; font.pointSize: Theme.caption
    }
    EmptyState { visible: stats.report.plots[stats.metric].points.length===0; Layout.fillWidth: true; Layout.fillHeight: true; title: qsTr("Brak danych"); description: qsTr("Zmień filtry lub rozegraj zawody z udziałem zawodnika.") }
}
