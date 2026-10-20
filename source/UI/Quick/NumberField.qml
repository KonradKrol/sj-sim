import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: control
    property var fields
    property bool direct: true
    property string label: ""
    signal committed(real value)
    implicitHeight: 48
    implicitWidth: 140
    function setValue(value) { if (direct) ui.edit(fields.id,value); committed(value) }
    SField {
        id: input
        objectName: "input-" + (control.fields.name || control.fields.id)
        anchors.fill: parent
        rightPadding: 48 + (suffix.visible ? suffix.implicitWidth + 6 : 0)
        text: control.fields.mixed ? "" : Number(control.fields.value).toLocaleString(Qt.locale(), 'f', control.fields.decimals).split(Qt.locale().groupSeparator).join("")
        placeholderText: control.fields.mixed ? qsTr("Różne wartości") : ""
        horizontalAlignment: TextInput.AlignRight
        font.family: Theme.mono
        Accessible.name: control.label || control.fields.accessibleName || control.fields.label || control.fields.tip || control.fields.name || ""
        validator: DoubleValidator { bottom: control.fields.minimum; top: control.fields.maximum; decimals: control.fields.decimals; locale: Qt.locale().name }
        onEditingFinished: {
            if(control.fields.mixed && text.length === 0)return
            var value = Number.fromLocaleString(Qt.locale(),text)
            if (acceptableInput && isFinite(value)) { if (value !== control.fields.value) control.setValue(value) }
            else text = Number(control.fields.value).toLocaleString(Qt.locale(),'f',control.fields.decimals).split(Qt.locale().groupSeparator).join("")
        }
        Keys.onUpPressed: control.setValue(Math.min(control.fields.maximum, Number(control.fields.value) + Number(control.fields.step)))
        Keys.onDownPressed: control.setValue(Math.max(control.fields.minimum, Number(control.fields.value) - Number(control.fields.step)))
    }
    Label { id: suffix; text: control.fields.suffix || ""; visible: text.length>0; anchors.right: parent.right; anchors.rightMargin: 44; anchors.verticalCenter: parent.verticalCenter; color: Theme.muted; font.pointSize: Theme.caption }
    Column {
        anchors.right: parent.right; anchors.top: parent.top; anchors.bottom: parent.bottom
        width: 38
        SButton { width: parent.width; height: control.height/2; text: "+"; padding: 0; quiet: true; enabled: control.fields.value < control.fields.maximum; Accessible.name: qsTr("Zwiększ wartość"); onClicked: control.setValue(Math.min(control.fields.maximum, Number(control.fields.value) + Number(control.fields.step))) }
        SButton { width: parent.width; height: control.height/2; text: "−"; padding: 0; quiet: true; enabled: control.fields.value > control.fields.minimum; Accessible.name: qsTr("Zmniejsz wartość"); onClicked: control.setValue(Math.max(control.fields.minimum, Number(control.fields.value) - Number(control.fields.step))) }
    }
}
