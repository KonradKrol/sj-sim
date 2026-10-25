import QtQuick 2.15
import QtQuick.Controls 2.15

Column {
    id: input
    property var node
    property bool showLabel: false
    readonly property var fields: node ? node.values : ({})
    enabled: fields.enabled!==false
    spacing: 3
    Label { visible: input.showLabel; width: parent.width; text: input.fields.label || qsTr("Nazwa sposobu"); color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap }
    Loader {
        width: parent.width
        sourceComponent: input.fields.kind==="choice" ? choice : text
    }
    Component {
        id: choice
        SChoice {
            objectName: "choice-"+(input.fields.name || input.fields.id)
            model: input.fields.options || []
            currentIndex: input.fields.value || 0
            Accessible.name: input.fields.label || input.fields.name || ""
            onActivated: ui.edit(input.fields.id,currentIndex)
        }
    }
    Component {
        id: text
        SField {
            objectName: "input-"+(input.fields.name || input.fields.id)
            text: input.fields.value || ""
            readOnly: !!input.fields.readOnly
            Accessible.name: input.fields.label || input.fields.name || ""
            onTextEdited: ui.inputText(input.fields.id,text)
            onEditingFinished: ui.finishText(input.fields.id)
        }
    }
}
