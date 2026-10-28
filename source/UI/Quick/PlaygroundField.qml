import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: field
    property var backend
    property int selectedVariant: 0
    property int sortMetric: 0
    property string search: ""
    signal variantSelected(int index)
    signal athleteSelected(int index)
    readonly property var variants: backend.report.variants || []
    readonly property var variant: variants[selectedVariant] || ({})
    readonly property var rows: (variant.athletes || []).filter(function(a){return a.name.toLowerCase().indexOf(field.search.toLowerCase())>=0}).slice().sort(function(a,b){if(field.sortMetric===0)return (a.place.mean===null?9999:a.place.mean)-(b.place.mean===null?9999:b.place.mean);if(field.sortMetric===1)return (b.total.mean || 0)-(a.total.mean || 0);return (b.distance.mean || 0)-(a.distance.mean || 0)})
    function format(v){return typeof v==="number" ? v.toLocaleString(Qt.locale(Theme.localeName),'f',1) : "—"}
    spacing: 10
    Label { text: qsTr("Cała stawka · agregaty konkursów"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    RowLayout {
        Layout.fillWidth: true
        SChoice { Layout.fillWidth: true; model: field.variants.map(function(v){return v.name}); currentIndex: field.selectedVariant; Accessible.name: qsTr("Wariant"); onActivated: field.variantSelected(currentIndex) }
        SChoice { Layout.fillWidth: true; model: [qsTr("Średnie miejsce"),qsTr("Średnia suma"),qsTr("Średnia odległość")]; currentIndex: field.sortMetric; Accessible.name: qsTr("Sortowanie wyników"); onActivated: field.sortMetric=currentIndex }
    }
    SField { Layout.fillWidth: true; placeholderText: qsTr("Szukaj skoczka…"); Accessible.name: placeholderText; onTextEdited: field.search=text }
    Label { text: qsTr("Kliknij skoczka, aby zbadać jego rozkłady. Różnice odnoszą się do wariantu bazowego; statystyki obejmują wszystkie ukończone konkursy."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    ListView {
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 120
        clip: true; spacing: 5; model: field.rows
        ScrollBar.vertical: ScrollBar {}
        delegate: ItemDelegate {
            id: row
            required property var modelData
            width: ListView.view.width
            background: Rectangle { color: row.hovered || row.visualFocus ? Theme.selection : Theme.surface; radius: Theme.dp(5); border.width: row.visualFocus ? 2 : 0; border.color: Theme.accent }
            contentItem: ColumnLayout {
                spacing: 5
                Label { text: row.modelData.name; color: Theme.ink; font.pointSize: Theme.body; font.bold: true; Layout.fillWidth: true; elide: Text.ElideRight }
                GridLayout {
                    Layout.fillWidth: true; columns: row.width<650?2:4
                    Label { text: qsTr("Miejsce: %1").arg(field.format(row.modelData.place.mean)); color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                    Label { text: qsTr("Suma: %1 pkt").arg(field.format(row.modelData.total.mean)); color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                    Label { text: qsTr("Odległość: %1 m").arg(field.format(row.modelData.distance.mean)); color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                    Label { text: qsTr("Zwycięstwa: %1 %").arg(field.format(100*row.modelData.wins/Math.max(1,row.modelData.starts))); color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                }
                Label { text: qsTr("Zmiana miejsca: %1 · metrów: %2 · starty: %3").arg(field.format(row.modelData.pairedPlace.mean)).arg(field.format(row.modelData.pairedDistance.mean)).arg(row.modelData.starts); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            }
            onClicked: field.athleteSelected(modelData.index)
        }
    }
}
