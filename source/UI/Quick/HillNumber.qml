import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: field
    property var backend
    property string fieldKey
    property string label
    property real minimum: 0
    property real maximum: 100
    property real step: 0.1
    spacing: Theme.dp(3)
    Label { text: field.label; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    NumberField {
        objectName: "hill-"+field.fieldKey
        Layout.fillWidth: true
        direct: false; label: field.label
        recordKey: field.backend.profile.id || ""
        fields: ({value:Number(field.backend.profile[field.fieldKey] || 0),minimum:field.minimum,maximum:field.maximum,step:field.step,decimals:field.step<1?1:0,name:"hill-"+field.fieldKey})
        onCommitted: function(value){field.backend.setValue(field.fieldKey,value)}
    }
}
