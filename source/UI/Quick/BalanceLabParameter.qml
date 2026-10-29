import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

RowLayout {
    id: parameter
    property var backend
    property var definition: ({})
    Layout.fillWidth: true
    spacing: 4
    PlaygroundNumber {
        Layout.fillWidth: true; Layout.minimumWidth: 0
        name: "balance-" + parameter.definition.key
        caption: parameter.definition.label || ""
        help: parameter.definition.help || ""
        value: parameter.backend.values[parameter.definition.key] || 0
        minimum: parameter.definition.minimum || 0
        maximum: parameter.definition.maximum || 100
        step: parameter.definition.step || 1
        decimals: parameter.definition.type === "integer" ? 0 : 3
        options: parameter.definition.options || []
        enabled: !parameter.backend.busy
        onCommitted: parameter.backend.setParameter(parameter.definition.key,value)
    }
    SButton { text: qsTr("Reset"); quiet: true; enabled: !parameter.backend.busy; Accessible.name: qsTr("Przywróć wartość z gry"); hint: qsTr("Przywróć wartość z gry"); onClicked: parameter.backend.resetParameter(parameter.definition.key) }
}
