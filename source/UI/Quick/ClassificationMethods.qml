import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: methods
    ViewArrival { target: methods; expressive: false }
    property var workspace
    readonly property var choice: workspace.value("classificationScoringMethod")
    function select(index) {
        var previous=entries.itemAt(choice.value)
        if(previous)previous.commitName()
        ui.edit(choice.id,index)
    }
    padding: 10
    background: Rectangle { color: Theme.selection; radius: 8; border.color: Theme.outline }
    contentItem: ColumnLayout {
        spacing: 6
        RowLayout {
            Layout.fillWidth: true
            Label { text: qsTr("Sposoby naliczania punktów"); color: Theme.ink; font.pointSize: Theme.body; font.weight: Font.DemiBold; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            HelpDot { explanation: qsTr("Wybierz sposób, aby edytować jego punktację. Nazwę wybranego sposobu zmienisz bezpośrednio w jego polu. Nowy sposób jest kopią aktualnego. Sposób domyślny ma stałą nazwę.") }
        }
        Flow {
            Layout.fillWidth: true; spacing: 6
            Repeater {
                id: entries
                model: methods.choice.options || []
                Rectangle {
                    id: method
                    required property int index
                    required property string modelData
                    readonly property bool selected: methods.choice.value===index
                    property bool dirty: false
                    function commitName(){if(dirty && selected && index>0){dirty=false;ui.edit(methods.workspace.value("classificationMethodName").id,nameInput.text)}}
                    width: Math.min(180,parent.width); height: Math.max(32,Theme.bodyMetrics.height+12)
                    color: selected ? Theme.surface : "transparent"
                    radius: 6; border.color: selected ? Theme.accent : Theme.outline
                    SField {
                        id: nameInput
                        objectName: "classificationMethod-"+method.index
                        anchors.fill: parent; anchors.rightMargin: method.selected && method.index>0 ? 28 : 0
                        text: method.modelData
                        readOnly: !method.selected || method.index===0 || methods.workspace.value("classificationMethodName").enabled===false
                        font.weight: method.selected ? Font.DemiBold : Font.Normal
                        Accessible.name: qsTr("Sposób: %1").arg(method.modelData)
                        background: Item {}
                        onTextEdited: method.dirty=true
                        onEditingFinished: method.commitName()
                    }
                    SButton {
                        visible: method.selected && method.index>0
                        anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter
                        width: 26; height: 26; padding: 0; text: "×"; quiet: true
                        Accessible.name: qsTr("Usuń sposób %1").arg(method.modelData)
                        enabled: methods.workspace.value("classificationRemoveMethod").enabled!==false
                        onClicked: {method.commitName();ui.activate(methods.workspace.value("classificationRemoveMethod").id)}
                    }
                    MouseArea {
                        anchors.fill: parent
                        visible: !method.selected || method.index===0
                        enabled: methods.choice.enabled!==false
                        onClicked: methods.select(method.index)
                    }
                    Keys.onReturnPressed: methods.select(index)
                    activeFocusOnTab: true
                }
            }
            SButton {
                objectName: "classificationNewMethod"
                text: "+"; padding: 4; implicitHeight: 32
                Accessible.name: qsTr("Dodaj sposób jako kopię bieżącego")
                enabled: methods.workspace.value("classificationAddMethod").enabled!==false
                onClicked: {var previous=entries.itemAt(methods.choice.value);if(previous)previous.commitName();ui.activate(methods.workspace.value("classificationAddMethod").id)}
            }
        }
    }
}
