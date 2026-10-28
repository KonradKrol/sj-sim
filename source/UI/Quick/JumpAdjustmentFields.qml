import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: adjustment
    property var backend
    property string fieldKey
    property string title
    property string unit
    property string help
    property real lower: 0
    property real upper: 1000
    readonly property var values: backend.values
    readonly property bool active: !!values[fieldKey+"Active"]
    padding: Theme.dp(10)
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    background: Rectangle { color: adjustment.active ? Theme.selection : Theme.surface; radius: Theme.dp(5); border.width: adjustment.active ? 2 : 1; border.color: adjustment.active ? Theme.accent : Theme.divider }
    contentItem: ColumnLayout {
        spacing: Theme.dp(8)
        RowLayout {
            Layout.fillWidth: true
            SCheck {
                objectName: "adjustment-"+adjustment.fieldKey
                text: adjustment.title; checked: adjustment.active; Layout.fillWidth: true
                font.family: Theme.displayFamily; font.pointSize: Theme.section; font.bold: true
                onClicked: {var selected=checked;adjustment.forceActiveFocus();adjustment.backend.setValue(adjustment.fieldKey+"Active",selected)}
            }
            HelpDot { visible: adjustment.help.length>0; explanation: adjustment.help }
        }
        ColumnLayout {
            visible: adjustment.active; Layout.fillWidth: true; spacing: Theme.dp(6)
            RowLayout {
                Layout.fillWidth: true
                Label { text: qsTr("Korekta · %1").arg(adjustment.unit); color: Theme.ink; font.pointSize: Theme.caption; Layout.fillWidth: true }
                NumberField {
                    objectName: "adjustmentBonus-"+adjustment.fieldKey
                    direct: false; flexibleDecimal: true; label: adjustment.title
                    Layout.preferredWidth: Theme.dp(150); Layout.minimumWidth: Theme.dp(110)
                    recordKey: adjustment.fieldKey+":"+adjustment.values.revision
                    fields: ({value:adjustment.values[adjustment.fieldKey+"Bonus"],minimum:-adjustment.upper,maximum:adjustment.upper,decimals:1,step:.5,name:adjustment.fieldKey+"Bonus"})
                    onCommitted: function(value){adjustment.backend.setValue(adjustment.fieldKey+"Bonus",value)}
                }
            }
            SCheck {
                text: qsTr("Ogranicz zakres"); checked: !!adjustment.values[adjustment.fieldKey+"Bounded"]
                onClicked: {var selected=checked;adjustment.forceActiveFocus();adjustment.backend.setValue(adjustment.fieldKey+"Bounded",selected)}
            }
            GridLayout {
                visible: !!adjustment.values[adjustment.fieldKey+"Bounded"]
                columns: 2; columnSpacing: Theme.dp(8); Layout.fillWidth: true
                Repeater {
                    model: [qsTr("Minimum"),qsTr("Maksimum")]
                    ColumnLayout {
                        id: rangeField
                        required property int index
                        required property string modelData
                        readonly property string key: adjustment.fieldKey+(index===0 ? "Min" : "Max")
                        Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: 2
                        Label { text: modelData; color: Theme.muted; font.pointSize: Theme.caption }
                        NumberField {
                            Layout.fillWidth: true; Layout.minimumWidth: 0; direct: false; flexibleDecimal: true
                            label: rangeField.modelData+": "+adjustment.title; recordKey: rangeField.key+":"+adjustment.values.revision
                            fields: ({value:adjustment.values[rangeField.key],minimum:adjustment.lower,maximum:adjustment.upper,decimals:1,step:.5,name:rangeField.key})
                            onCommitted: function(value){adjustment.backend.setValue(rangeField.key,value)}
                        }
                    }
                }
            }
        }
    }
}
