import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: control
    property var fields
    property bool direct: true
    property string label: ""
    property real currentValue: fields ? Number(fields.value) : 0
    property string draft: ""
    property string recordKey: ""
    property bool flexibleDecimal: false
    property real quantum: 0
    onRecordKeyChanged: { input.dirty=false; synchronize() }
    readonly property string localeName: Theme.localeName
    onLocaleNameChanged: synchronize()
    function synchronize() {
        if(!fields)return
        currentValue=Number(fields.value)
        if(!input.dirty)draft=fields.mixed ? "" : formattedValue()
    }
    onFieldsChanged: synchronize()
    Component.onCompleted: synchronize()
    signal committed(real value)
    ChangeFeedback { id: invalidFeedback; target: control; duration: Theme.motion*3 }
    implicitHeight: Math.max(Theme.compact ? 28 : 48,Theme.bodyMetrics.height+(Theme.compact ? 4 : 18))
    implicitWidth: 140
    function setValue(value) { if(quantum>0)value=Math.round(value/quantum)*quantum;currentValue=Math.max(fields.minimum,Math.min(fields.maximum,value));input.dirty=false;draft=formattedValue();if(direct)ui.edit(fields.id,currentValue);committed(currentValue) }
    function formattedValue() {return currentValue===fields.minimum && fields.specialText ? fields.specialText : currentValue.toLocaleString(Qt.locale(Theme.localeName),'f',fields.decimals).split(Qt.locale(Theme.localeName).groupSeparator).join("")}
    function commitInput() {
        if(!input.dirty && draft===formattedValue())return
        input.dirty=false
        if(fields.mixed && draft.length===0)return
        if(currentValue===fields.minimum && draft===fields.specialText)return
        var value=NaN
        try {value=control.flexibleDecimal ? Number(draft.trim().replace(",",".")) : Number.fromLocaleString(Qt.locale(Theme.localeName),draft)} catch(error) {}
        if(input.acceptableInput && draft.trim().length>0 && isFinite(value) && value>=fields.minimum && value<=fields.maximum){if(value!==currentValue)setValue(value);else draft=formattedValue()}
        else {draft=fields.mixed ? "" : formattedValue();invalidFeedback.restart()}
    }
    function step(direction) {if(input.dirty)commitInput();setValue(currentValue+direction*Number(fields.step))}
    SField {
        id: input
        property bool dirty: false
        objectName: "input-" + (control.fields.name || control.fields.id)
        anchors.fill: parent
        rightPadding: 48 + (suffix.visible ? suffix.implicitWidth + 6 : 0)
        text: control.draft
        placeholderText: control.fields.mixed ? qsTr("Różne wartości") : ""
        horizontalAlignment: TextInput.AlignRight
        font.family: Theme.mono
        Accessible.name: control.label || control.fields.accessibleName || control.fields.label || control.fields.tip || control.fields.name || ""
        validator: control.flexibleDecimal ? decimalValidator : localeValidator
        onTextEdited: {dirty=true;control.draft=text}
        onEditingFinished: control.commitInput()
        Keys.onUpPressed: control.step(1)
        Keys.onDownPressed: control.step(-1)
    }
    DoubleValidator { id: localeValidator; bottom: control.fields.minimum; top: control.fields.maximum; decimals: control.fields.decimals; locale: Qt.locale(Theme.localeName).name }
    RegularExpressionValidator { id: decimalValidator; regularExpression: new RegExp("[+-]?[0-9]*"+(control.fields.decimals>0?"([.,][0-9]{0,"+control.fields.decimals+"})?":"")) }
    Label { id: suffix; text: control.fields.suffix || ""; visible: text.length>0; anchors.right: parent.right; anchors.rightMargin: 44; anchors.verticalCenter: parent.verticalCenter; color: Theme.muted; font.pointSize: Theme.caption }
    Column {
        anchors.right: parent.right; anchors.top: parent.top; anchors.bottom: parent.bottom
        width: 38
        SButton { objectName: "numberIncrease-"+(control.fields.name || control.fields.id); autoRepeat: true; autoRepeatDelay: 250; autoRepeatInterval: 40; width: parent.width; height: control.height/2; text: "+"; padding: 0; quiet: true; enabled: control.currentValue < control.fields.maximum; Accessible.name: qsTr("Zwiększ wartość"); onPressed: control.step(1) }
        SButton { objectName: "numberDecrease-"+(control.fields.name || control.fields.id); autoRepeat: true; autoRepeatDelay: 250; autoRepeatInterval: 40; width: parent.width; height: control.height/2; text: "−"; padding: 0; quiet: true; enabled: control.currentValue > control.fields.minimum; Accessible.name: qsTr("Zmniejsz wartość"); onPressed: control.step(-1) }
    }
    Rectangle {
        anchors.fill: parent; color: "transparent"; radius: Theme.dp(6)
        visible: invalidFeedback.progress<1; opacity: 1-invalidFeedback.progress
        border.width: 2; border.color: Theme.danger
        Accessible.ignored: true
    }
}
