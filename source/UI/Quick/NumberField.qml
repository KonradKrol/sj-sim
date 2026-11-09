import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: control
    property var fields
    property bool direct: true
    property string label: ""
    property real currentValue: fields ? Number(fields.value) : 0
    property alias draft: input.text
    property string recordKey: ""
    property bool flexibleDecimal: false
    property real quantum: 0
    property bool committing: false
    onRecordKeyChanged: { input.dirty=false; synchronize(true) }
    readonly property string localeName: Theme.localeName
    onLocaleNameChanged: synchronize()
    function synchronize(force) {
        if(!fields || committing)return
        if(!force && (input.dirty || input.activeFocus))return
        currentValue=Number(fields.value)
        draft=fields.mixed ? "" : formattedValue()
    }
    onFieldsChanged: synchronize()
    Component.onCompleted: synchronize()
    signal committed(real value)
    ChangeFeedback { id: invalidFeedback; target: control; duration: Theme.motion*3 }
    implicitHeight: Math.max(Theme.compact ? 28 : 48,Theme.bodyMetrics.height+(Theme.compact ? 4 : 18))
    implicitWidth: 140
    function setValue(value) {
        if(!isFinite(value))return
        if(quantum>0)value=Math.round(value/quantum)*quantum
        var precision=Math.pow(10,Number(fields.decimals || 0))
        value=Math.round(value*precision)/precision
        currentValue=Math.max(fields.minimum,Math.min(fields.maximum,value))
        input.dirty=false
        draft=formattedValue()
        committing=true
        try { if(direct)ui.edit(fields.id,currentValue);committed(currentValue) }
        finally { committing=false }
    }
    function formattedValue() {return currentValue===fields.minimum && fields.specialText ? fields.specialText : currentValue.toLocaleString(Qt.locale(Theme.localeName),'f',fields.decimals).split(Qt.locale(Theme.localeName).groupSeparator).join("")}
    function commitInput() {
        if(!input.dirty && draft===formattedValue())return
        input.dirty=false
        if(fields.mixed && draft.length===0)return
        if(currentValue===fields.minimum && draft===fields.specialText)return
        var value=NaN
        try {
            var text=draft.trim()
            value=/^[+-]?(?:[0-9]+(?:[.,][0-9]*)?|[.,][0-9]+)$/.test(text)
                ? Number(text.replace(",",".")) : Number.fromLocaleString(Qt.locale(Theme.localeName),text)
        } catch(error) {}
        if(draft.trim().length>0 && isFinite(value) && value>=fields.minimum && value<=fields.maximum){if(value!==currentValue)setValue(value);else draft=formattedValue()}
        else {draft=fields.mixed ? "" : formattedValue();invalidFeedback.restart()}
    }
    function step(direction) {if(input.dirty)commitInput();setValue(currentValue+direction*Number(fields.step))}
    SField {
        id: input
        property bool dirty: false
        objectName: "input-" + (control.fields.name || control.fields.id)
        anchors.fill: parent
        rightPadding: 48 + (suffix.visible ? suffix.implicitWidth + 6 : 0)
        placeholderText: control.fields.mixed ? qsTr("Różne wartości") : ""
        horizontalAlignment: TextInput.AlignRight
        font.family: Theme.mono
        Accessible.name: control.label || control.fields.accessibleName || control.fields.label || control.fields.tip || control.fields.name || ""
        onTextEdited: dirty=true
        onEditingFinished: control.commitInput()
        Keys.onUpPressed: control.step(1)
        Keys.onDownPressed: control.step(-1)
    }
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
