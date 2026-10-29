import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: parameter
    property var backend
    property var definition: ({})
    property bool detailsOpen: false
    readonly property string unit: definition.unit && definition.unit!=="1" ? definition.unit : ""
    readonly property string explanation: String(definition.help || "").split("\n")[0]
    readonly property bool binary: definition.minimum===0 && definition.maximum===1 && definition.type==="integer" && !(definition.options || []).length
    function format(value) { return Number(value).toLocaleString(Qt.locale(Theme.localeName),'f',definition.type==="integer" ? 0 : 3) }
    Layout.fillWidth: true
    spacing: 4
    PlaygroundNumber {
        Layout.fillWidth: true; Layout.minimumWidth: 0
        name: "balance-" + parameter.definition.key
        caption: parameter.unit ? qsTr("%1 (%2)").arg(parameter.definition.label || "").arg(parameter.unit) : parameter.definition.label || ""
        value: parameter.backend.values[parameter.definition.key] || 0
        minimum: parameter.definition.minimum === undefined ? 0 : parameter.definition.minimum
        maximum: parameter.definition.maximum === undefined ? 100 : parameter.definition.maximum
        step: parameter.definition.step || 1
        decimals: parameter.definition.type === "integer" ? 0 : 3
        options: parameter.definition.options || []
        optionOffset: parameter.definition.optionOffset || 0
        toggle: parameter.binary
        enabled: !parameter.backend.busy
        onCommitted: parameter.backend.setParameter(parameter.definition.key,value)
    }
    Label { text: parameter.explanation; visible: text.length>0; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    Flow {
        Layout.fillWidth: true; spacing: Theme.dp(4)
        SButton { text: qsTr("Przywróć wartość z gry"); quiet: true; enabled: !parameter.backend.busy; onClicked: parameter.backend.resetParameter(parameter.definition.key) }
        SButton { text: qsTr("Zakres i szczegóły"); quiet: true; checkable: true; checked: parameter.detailsOpen; onClicked: parameter.detailsOpen=!parameter.detailsOpen }
    }
    Label {
        visible: parameter.detailsOpen
        text: qsTr("Dozwolony zakres: %1–%2. Domyślna wartość silnika: %3.\nKlucz techniczny: %4").arg(parameter.format(parameter.definition.minimum)).arg(parameter.format(parameter.definition.maximum)).arg(parameter.format(parameter.definition.default)).arg(parameter.definition.key || "")
        color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap
    }
    Rectangle { Layout.fillWidth: true; implicitHeight: 1; color: Theme.divider; Accessible.ignored: true }
}
