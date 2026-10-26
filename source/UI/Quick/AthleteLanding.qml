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
        RowLayout {
            Layout.fillWidth: true
            Label { text: qsTr("Lądowanie"); color: landing.design.ink; font.family: landing.design.headingFont; font.pointSize: Theme.section; font.bold: true; Layout.fillWidth: true }
            Label { text: steps.columns===1 ? qsTr("Jakość rośnie ↓") : qsTr("Jakość rośnie →"); color: landing.design.muted; font.pointSize: Theme.caption }
        }
        GridLayout {
            id: steps
            Layout.fillWidth: true
            columns: width>=Theme.dp(640) ? 5 : 1
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
                    checked: landing.backend.profile.landing===index
                    hoverEnabled: true
                    implicitWidth: contentItem.implicitWidth+leftPadding+rightPadding
                    implicitHeight: Math.max(Theme.dp(39),contentItem.implicitHeight+topPadding+bottomPadding)
                    padding: Theme.dp(9)
                    Accessible.name: qsTr("%1 · poziom %2 z 5").arg(modelData).arg(index+1); Accessible.role: Accessible.RadioButton; Accessible.checked: checked
                    background: Item {
                        Rectangle {
                            visible: steps.columns===1 && choice.index<4
                            x: choice.leftPadding+Theme.dp(11); y: parent.height/2
                            width: 1; height: choice.height+steps.rowSpacing
                            color: landing.design.line
                        }
                        Rectangle {
                            anchors.fill: parent
                            anchors.leftMargin: steps.columns===1 ? Theme.dp(37) : 0
                            color: choice.checked ? landing.design.accent : choice.hovered ? landing.design.paper : "transparent"
                            radius: Theme.dp(3)
                            border.width: choice.activeFocus ? 2 : 1
                            border.color: choice.checked || choice.activeFocus ? landing.design.accent : landing.design.line
                        }
                    }
                    contentItem: RowLayout {
                        spacing: Theme.dp(12)
                        Rectangle {
                            Layout.preferredWidth: Theme.dp(22); Layout.preferredHeight: Theme.dp(22)
                            radius: width/2; color: choice.checked ? landing.design.accent : landing.design.paper; border.width: 1
                            border.color: choice.checked ? landing.design.accent : landing.design.line
                            Label { anchors.centerIn: parent; text: choice.index+1; font.family: Theme.mono; font.pointSize: Theme.caption; font.bold: true; color: choice.checked ? landing.design.accentInk : landing.design.muted }
                        }
                        Label { text: choice.modelData; Layout.fillWidth: true; Layout.minimumWidth: 0; wrapMode: Text.WordWrap; color: choice.checked ? landing.design.accentInk : landing.design.ink; font.pointSize: Theme.body; font.family: Theme.bodyFamily; font.weight: choice.checked ? Font.Bold : Font.Medium }
                    }
                    onClicked: landing.backend.setValue("landing",index)
                }
            }
        }
    }
}
