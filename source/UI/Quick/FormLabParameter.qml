import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

RowLayout {
    id: row
    property var backend
    property var definition
    property bool option: false
    readonly property real value: Number(option ? backend.options[definition.key] : backend.config.values[definition.key])
    function display(){return String(value).replace(".",Qt.locale(Theme.localeName).decimalPoint)}
    function apply(value){if(option)backend.setOption(definition.key,value);else backend.setParameter(definition.key,value)}
    spacing: Theme.dp(6)
    Label { text: row.definition.label; color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; Layout.minimumWidth: 0; wrapMode: Text.WordWrap }
    HelpDot { visible: !!row.definition.help; explanation: row.definition.help || "" }
    SField {
        id: field
        objectName: "formParameter-"+row.definition.key
        Layout.preferredWidth: Theme.dp(85)
        enabled: !row.backend.busy && row.definition.editable!==false
        text: row.display(); horizontalAlignment: Text.AlignRight
        Accessible.name: row.definition.label
        validator: RegularExpressionValidator { regularExpression: row.option ? /^-?\d{0,8}$/ : /^-?\d{0,8}([.,]\d{0,6})?$/ }
        onEditingFinished: {var value=Number(text.replace(",","."));if(text.length && text!=="-" && isFinite(value) && value>=row.definition.minimum && value<=row.definition.maximum)row.apply(value);text=Qt.binding(function(){return row.display()})}
    }
}
