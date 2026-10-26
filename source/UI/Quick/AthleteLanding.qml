import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: landing
    property var backend
    property var design
    readonly property var names: [qsTr("Niechlujne"),qsTr("Niestaranne"),qsTr("Poprawne"),qsTr("Eleganckie"),qsTr("Stylista")]
    spacing: Theme.dp(6)
    Label { text: qsTr("Lądowanie"); color: landing.design.muted; font.family: landing.design.detailFont; font.pointSize: Theme.caption }
    Flow {
        Layout.fillWidth: true; spacing: Theme.dp(5)
        Repeater {
            model: landing.names
            AbstractButton {
                id: choice
                required property int index
                required property string modelData
                objectName: "athleteLanding-"+index
                checked: landing.backend.profile.landing===index
                implicitWidth: text.implicitWidth+Theme.dp(22); implicitHeight: Math.max(Theme.dp(34),text.implicitHeight+Theme.dp(12))
                padding: Theme.dp(7)
                Accessible.name: modelData; Accessible.role: Accessible.RadioButton; Accessible.checked: checked
                background: Rectangle {
                    color: choice.checked ? landing.design.accent : choice.hovered ? landing.design.surface : "transparent"
                    radius: landing.design.variant===2 || landing.design.variant===4 ? height/2 : landing.design.variant===1 ? 0 : Theme.dp(3)
                    border.width: choice.activeFocus ? 2 : choice.checked || landing.design.variant!==1 ? 1 : 0
                    border.color: choice.checked || choice.activeFocus ? landing.design.accent : landing.design.line
                }
                contentItem: Label { id: text; text: choice.modelData; color: choice.checked ? landing.design.accentInk : landing.design.ink; font.pointSize: Theme.body; font.family: landing.design.variant===1 ? Theme.bodyFamily : landing.design.headingFont; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: landing.backend.setValue("landing",index)
            }
        }
    }
}
