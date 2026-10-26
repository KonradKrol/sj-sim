import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: landing
    property var backend
    property var design
    readonly property var names: [qsTr("Niechlujne"),qsTr("Niestaranne"),qsTr("Poprawne"),qsTr("Eleganckie"),qsTr("Stylista")]
    padding: Theme.dp(8)
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    background: Rectangle {
        color: landing.design.surface
        radius: Theme.dp(4)
        border.width: 1
        border.color: landing.design.line
    }
    contentItem: ColumnLayout {
        spacing: Theme.dp(6)
        RowLayout {
            Layout.fillWidth: true
            Label { text: qsTr("Lądowanie"); color: landing.design.muted; font.family: Theme.bodyFamily; font.pointSize: Theme.caption; Layout.fillWidth: true }
            Label { text: landing.names[landing.backend.profile.landing]; color: landing.design.ink; font.family: Theme.bodyFamily; font.pointSize: Theme.body; font.bold: true }
        }
        RowLayout {
            id: steps
            Layout.fillWidth: true
            readonly property bool showNames: width>=Theme.dp(660)
            spacing: Theme.dp(4)
            Repeater {
                model: landing.names
                AbstractButton {
                    id: choice
                    required property int index
                    required property string modelData
                    objectName: "athleteLanding-"+index
                    Layout.fillWidth: true
                    Layout.preferredWidth: 0
                    Layout.minimumWidth: 0
                    checked: landing.backend.profile.landing===index
                    hoverEnabled: true
                    activeFocusOnTab: true
                    implicitHeight: Math.max(Theme.dp(32),contentItem.implicitHeight+topPadding+bottomPadding)
                    padding: Theme.dp(5)
                    Accessible.name: qsTr("%1 · poziom %2 z 5").arg(modelData).arg(index+1)
                    Accessible.role: Accessible.RadioButton
                    Accessible.checked: checked
                    background: Rectangle {
                        color: choice.checked ? landing.design.accent : choice.hovered ? landing.design.paper : "transparent"
                        radius: Theme.dp(3)
                        border.width: choice.activeFocus ? 2 : 1
                        border.color: choice.checked || choice.activeFocus ? landing.design.accent : landing.design.line
                    }
                    contentItem: RowLayout {
                        spacing: Theme.dp(5)
                        Label { text: choice.index+1; Layout.fillWidth: !steps.showNames; horizontalAlignment: Text.AlignHCenter; color: choice.checked ? landing.design.accentInk : landing.design.muted; font.family: Theme.mono; font.pointSize: Theme.body; font.bold: true }
                        Label { visible: steps.showNames; text: choice.modelData; Layout.fillWidth: true; Layout.minimumWidth: 0; wrapMode: Text.Wrap; color: choice.checked ? landing.design.accentInk : landing.design.ink; font.pointSize: Theme.body; font.family: Theme.bodyFamily; font.weight: choice.checked ? Font.Bold : Font.Medium }
                    }
                    onClicked: landing.backend.setValue("landing",index)
                    ToolTip.visible: !steps.showNames && (hovered || activeFocus) && !down
                    ToolTip.delay: hovered ? 450 : 0
                    ToolTip.text: modelData
                }
            }
        }
    }
}
