import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: control
    property var backend
    property string fieldKey
    property string label
    property string explanation
    property real minimum: 0
    property real maximum: 100
    property real step: 0.1
    property real factor: 1
    spacing: Theme.dp(3)
    RowLayout {
        Layout.fillWidth: true
        Label { text: control.label; color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        HelpDot { visible: !!control.explanation; explanation: control.explanation }
    }
    NumberField {
        objectName: "weather-"+control.fieldKey
        Layout.fillWidth: true
        direct: false; label: control.label; flexibleDecimal: true
        recordKey: control.fieldKey+":"+control.backend.values.seed
        fields: ({value:Number(control.backend.values[control.fieldKey] || 0)*control.factor,minimum:control.minimum,maximum:control.maximum,step:control.step,decimals:control.step<1?1:0,name:"weather-"+control.fieldKey})
        onCommitted: function(value){control.backend.setValue(control.fieldKey,value/control.factor)}
    }
}
