import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: landing
    property var backend
    readonly property var names: [qsTr("Niechlujne"),qsTr("Niestaranne"),qsTr("Poprawne"),qsTr("Eleganckie"),qsTr("Stylista")]
    spacing: Theme.dp(7)
    RowLayout {
        Label { text: qsTr("Lądowanie"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section }
        Item { Layout.fillWidth: true }
        Label { text: landing.names[landing.backend.profile.landing || 0]; color: Theme.muted; font.pointSize: Theme.caption }
    }
    GridLayout {
        Layout.fillWidth: true; columns: landing.width<Theme.dp(330) ? 3 : 5; columnSpacing: Theme.dp(5); rowSpacing: Theme.dp(5)
        Repeater {
            model: landing.names
            AbstractButton {
                id: choice
                required property int index
                required property string modelData
                objectName: "athleteLanding-"+index
                Layout.fillWidth: true; implicitHeight: contentItem.implicitHeight+Theme.dp(18); padding: Theme.dp(8)
                checked: landing.backend.profile.landing===index
                Accessible.name: modelData; Accessible.role: Accessible.RadioButton; Accessible.checked: checked
                background: Rectangle { color: choice.checked ? Theme.stadium : choice.hovered ? Theme.selection : Theme.surface; radius: Theme.dp(7); border.width: choice.activeFocus || choice.checked ? 2 : 1; border.color: choice.checked ? Theme.accent : Theme.divider }
                contentItem: ColumnLayout {
                    spacing: Theme.dp(6)
                    Row {
                        Layout.alignment: Qt.AlignHCenter; spacing: Theme.dp(2)
                        Repeater { model: 5; Rectangle { required property int index; width: Theme.dp(5); height: Theme.dp(5+index*2); radius: 1; color: index<=choice.index ? (choice.checked ? Theme.signal : Theme.accent) : (choice.checked ? "#456071" : Theme.divider) } }
                    }
                    Label { Layout.fillWidth: true; text: choice.modelData; horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap; color: choice.checked ? Theme.snow : Theme.ink; font.pointSize: Theme.caption }
                }
                onClicked: landing.backend.setValue("landing",index)
            }
        }
    }
}
