import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: live
    property var backend
    property int selectedVariant: 0
    readonly property var variants: backend.previewReport.variants || []
    readonly property var result: variants.filter(function(v){return v.sourceVariant===live.selectedVariant})[0] || ({})
    function format(value){return typeof value==="number" ? value.toLocaleString(Qt.locale(Theme.localeName),'f',1) : "—"}
    padding: Theme.dp(12)
    background: Rectangle { color: Theme.selection; radius: Theme.dp(6) }
    contentItem: ColumnLayout {
        spacing: 6
        Label { text: live.backend.previewBusy ? qsTr("Przeliczanie podglądu…") : qsTr("Szybki podgląd · %1 konkursów na wariant").arg(live.backend.previewReport.completed || 0); color: Theme.ink; font.pointSize: Theme.body; font.bold: true; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Label { visible: !!live.backend.diagnostics.error; text: live.backend.diagnostics.error || ""; color: Theme.warning; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Label { visible: live.variants.length>0; text: qsTr("Skoczek: %1 m · %2 pkt · miejsce %3").arg(live.result.focusDistance ? live.format(live.result.focusDistance.mean) : "—").arg(live.result.focusPoints ? live.format(live.result.focusPoints.mean) : "—").arg(live.result.focus ? live.format(live.result.focus.place.mean) : "—"); color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Label { text: qsTr("Mała próba służy do oceny kierunku zmian. Dokładne porównanie uruchom przyciskiem Uruchom."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    }
}
