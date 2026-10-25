import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: input
    property string caption
    property string name
    property real value: 0
    property real minimum: 0
    property real maximum: 100
    property real step: 1
    property int decimals: 0
    property string help: ""
    property bool toggle: false
    property var options: []
    readonly property real currentValue: number.currentValue
    signal committed(real value)
    spacing: 3
    RowLayout {
        Layout.fillWidth: true
        Label { text: input.caption; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        HelpDot { visible: input.help.length>0; explanation: input.help }
    }
    SCheck { visible: input.toggle; text: checked?qsTr("Włączone"):qsTr("Wyłączone"); checked: input.value!==0; onClicked: input.committed(checked?1:0) }
    SChoice { visible: input.options.length>0; Layout.fillWidth: true; model: input.options; currentIndex: input.value; Accessible.name: input.caption; onActivated: input.committed(currentIndex) }
    NumberField { id: number; visible: !input.toggle && !input.options.length; Layout.fillWidth: true; Layout.minimumWidth: 0; direct: false; label: input.caption; fields: ({name:input.name,value:input.value,minimum:input.minimum,maximum:input.maximum,step:input.step,decimals:input.decimals}); onCommitted: input.committed(value) }
}
