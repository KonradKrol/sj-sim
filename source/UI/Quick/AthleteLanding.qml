import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: landing
    property var backend
    property var design
    readonly property var names: [qsTr("Niechlujne"),qsTr("Niestaranne"),qsTr("Poprawne"),qsTr("Eleganckie"),qsTr("Stylista")]
    padding: Theme.dp(10)
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    background: Rectangle {
        color: landing.design.surface
        radius: Theme.dp(4)
        border.width: 1
        border.color: landing.design.line
    }
    contentItem: ColumnLayout {
        spacing: Theme.dp(9)
        Label { text: qsTr("Lądowanie"); color: landing.design.ink; font.family: landing.design.headingFont; font.pointSize: Theme.section; font.bold: true }
        GridLayout {
            Layout.fillWidth: true
            columns: width>=Theme.dp(640) ? 5 : 2
            columnSpacing: Theme.dp(5)
            rowSpacing: Theme.dp(5)
            Repeater {
                model: landing.names
                AbstractButton {
                    id: choice
                    required property int index
                    required property string modelData
                    objectName: "athleteLanding-"+index
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    Layout.columnSpan: index===4 && parent.columns===2 ? 2 : 1
                    checked: landing.backend.profile.landing===index
                    hoverEnabled: true
                    implicitWidth: contentItem.implicitWidth+leftPadding+rightPadding
                    implicitHeight: Math.max(Theme.dp(39),contentItem.implicitHeight+topPadding+bottomPadding)
                    padding: Theme.dp(9)
                    Accessible.name: modelData; Accessible.role: Accessible.RadioButton; Accessible.checked: checked
                    background: Rectangle {
                        color: choice.checked ? landing.design.accent : choice.hovered ? landing.design.paper : "transparent"
                        radius: Theme.dp(3)
                        border.width: choice.activeFocus ? 2 : 1
                        border.color: choice.checked || choice.activeFocus ? landing.design.accent : landing.design.line
                    }
                    contentItem: RowLayout {
                        spacing: Theme.dp(7)
                        Rectangle {
                            Layout.preferredWidth: Theme.dp(12); Layout.preferredHeight: Theme.dp(12)
                            radius: width/2; color: "transparent"; border.width: 1
                            border.color: choice.checked ? landing.design.accentInk : landing.design.muted
                            Rectangle { anchors.centerIn: parent; width: Theme.dp(6); height: width; radius: width/2; visible: choice.checked; color: landing.design.accentInk }
                        }
                        Label { text: choice.modelData; Layout.fillWidth: true; Layout.minimumWidth: 0; wrapMode: Text.WordWrap; color: choice.checked ? landing.design.accentInk : landing.design.ink; font.pointSize: Theme.body; font.family: Theme.bodyFamily; font.weight: choice.checked ? Font.Bold : Font.Medium }
                    }
                    onClicked: landing.backend.setValue("landing",index)
                }
            }
        }
    }
}
