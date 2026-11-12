import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: preferences
    property var node
    spacing: Theme.dp(12)
    Layout.alignment: Qt.AlignTop
    RowLayout {
        Layout.fillWidth: true
        ColumnLayout {
            Layout.fillWidth: true; spacing: Theme.dp(3)
            Label { text: qsTr("Symulacja"); font.family: Theme.displayFamily; font.pointSize: Theme.section; font.bold: true; color: Theme.ink }
            Label { text: qsTr("Zmiany dotyczą kolejnych konkursów."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        }
        SToolButton {
            text: "↶"; hint: qsTr("Przywróć standardowe ustawienia symulacji"); Accessible.name: hint
            onClicked: {for(var i=0;i<preferences.node.children.length;++i)ui.edit(preferences.node.children[i].values.id,2)}
        }
    }
    Pane {
        Layout.fillWidth: true
        padding: Theme.dp(12)
        implicitHeight: rows.implicitHeight+topPadding+bottomPadding
        background: Rectangle { color: Theme.surface; radius: Theme.radius; border.color: Theme.outline }
        contentItem: ColumnLayout {
            id: rows
            spacing: Theme.dp(12)
            Repeater {
                model: preferences.node.children
                ColumnLayout {
                    id: row
                    required property var modelData
                    required property int index
                    readonly property var field: modelData.values
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignTop
                    spacing: Theme.dp(10)
                    GridLayout {
                        Layout.fillWidth: true
                        columns: preferences.width>=Theme.dp(1000) ? 2 : 1
                        columnSpacing: Theme.dp(24); rowSpacing: Theme.dp(6)
                        RowLayout {
                            Layout.fillWidth: true; Layout.preferredWidth: Theme.dp(230)
                            Label { text: row.field.label; font.pointSize: Theme.body; font.bold: true; color: Theme.ink; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                            HelpDot { explanation: row.field.tip || "" }
                        }
                        ColumnLayout {
                            Layout.fillWidth: true; Layout.preferredWidth: Theme.dp(630)
                            RowLayout {
                                visible: parent.width>=Theme.dp(580)
                                Layout.fillWidth: true; spacing: Theme.dp(3)
                                Repeater {
                                    model: row.field.options || []
                                    SButton {
                                        required property int index
                                        required property string modelData
                                        readonly property bool selected: index===row.field.value
                                        text: modelData; dense: true; font.pointSize: Theme.caption
                                        Layout.fillWidth: true; Layout.preferredWidth: 0
                                        foreground: selected ? Theme.snow : Theme.ink
                                        Accessible.name: row.field.label+": "+modelData
                                        background: Rectangle { color: parent.selected ? Theme.stadium : parent.hovered ? Theme.selection : Theme.raised; radius: Theme.dp(4); border.color: parent.selected ? Theme.stadium : Theme.divider }
                                        onClicked: ui.edit(row.field.id,index)
                                    }
                                }
                            }
                            SChoice { visible: parent.width<Theme.dp(580); Layout.fillWidth: true; model: row.field.options || []; currentIndex: row.field.value; Accessible.name: row.field.label; onActivated: function(index){ui.edit(row.field.id,index)} }
                        }
                    }
                    Rectangle { visible: row.index<4; Layout.fillWidth: true; implicitHeight: 1; color: Theme.divider }
                }
            }
        }
    }
}
