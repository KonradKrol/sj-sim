import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: comparison
    property var recorded: ({})
    visible: appearance.windDiagnostics && recorded.actualWind!==undefined && recorded.measuredWind!==undefined
    function number(value){return Number(value).toLocaleString(Qt.locale(Theme.localeName),'f',2)}
    spacing: Theme.dp(2)
    Label { text: qsTr("Przebyty odcinek: %1 m/s").arg(comparison.number(comparison.recorded.actualWind)); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    Label { text: qsTr("Pomiar: %1 m/s · różnica: %2 m/s").arg(comparison.number(comparison.recorded.measuredWind)).arg(comparison.number(Number(comparison.recorded.measuredWind)-Number(comparison.recorded.actualWind))); color: Theme.ink; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
}
