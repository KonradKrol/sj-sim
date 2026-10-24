import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

FocusScope {
    id: control
    property bool showLabel: true
    property var node
    property real tableHeightLimit: 540
    property bool compactTable: false
    property bool tableSearchEnabled: true
    property bool raceResults: false
    readonly property var fields: node ? node.values : ({kind: "column"})
    readonly property string kind: fields.kind || "column"
    objectName: "quick-" + (fields.name || fields.id || "root")
    visible: !!node && fields.available !== false
    implicitWidth: 240
    implicitHeight: contents.implicitHeight
    enabled: fields.enabled === undefined || fields.enabled
    Column {
    id: contents
    width: parent.width
    spacing: Theme.compact ? 3 : 6
    Label {
        visible: control.showLabel && !!control.fields.label
        text: control.fields.label || ""
        width: parent.width
        color: Theme.muted
        font.pointSize: Theme.caption
        wrapMode: Text.WordWrap
    }
    Loader {
        id: body
        width: parent.width
        sourceComponent: control.kind === "empty" ? emptyBody : control.kind === "roster" ? rosterBody : control.kind === "rosterSummary" ? rosterSummary : control.kind === "calendar" ? calendarBody : control.kind === "wind" ? windBody : control.kind === "label" ? labelBody : control.kind === "button" ? buttonBody : control.kind === "check" ? checkBody : control.kind === "number" ? numberBody : control.kind === "text" || control.kind === "date" ? textBody : control.kind === "choice" ? choiceBody : control.kind === "slider" ? sliderBody : control.kind === "richtext" ? richBody : control.kind === "multiline" ? multilineBody : control.kind === "table" ? tableBody : control.kind === "chart" ? chartBody : control.kind === "progress" ? progressBody : groupBody
    }
    }
    Component { id: emptyBody; EmptyState { title: control.fields.title || ""; description: control.fields.description || ""; actionText: control.fields.actionText || ""; onTriggered: ui.activate(control.fields.actionId) } }
    Component { id: rosterBody; RosterWorkspace { node: control.node } }
    Component { id: rosterSummary; RosterSummary { node: control.node } }
    Component { id: windBody; WindEditor { node: control.node } }
    Component { id: calendarBody; CalendarWorkspace { node: control.node } }
    Component {
        id: groupBody
        NodeGroup { node: control.node; focused: control.activeFocus }
    }
    Component {
        id: labelBody
        RowLayout {
            spacing: Theme.compact ? 5 : 10
            Image {
                visible: !!control.fields.image
                source: control.fields.image || ""
                sourceSize.width: 160; sourceSize.height: 160
                Layout.preferredWidth: control.fields.name && /img/i.test(control.fields.name) ? 72 : 36
                Layout.preferredHeight: control.fields.name && /img/i.test(control.fields.name) ? 72 : 26
                fillMode: Image.PreserveAspectFit
                Accessible.name: control.fields.accessibleName || control.fields.label || control.fields.tip || ""
            }
            Label {
                text: control.fields.text || ""
                visible: text.length > 0
                textFormat: Text.AutoText
                color: Theme.ink
                font.pointSize: control.fields.heading ? Theme.heading : Theme.body
                font.weight: control.fields.heading ? Font.DemiBold : Font.Normal
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                onLinkActivated: Qt.openUrlExternally(link)
            }
        }
    }
    Component {
        id: buttonBody
        SButton {
            objectName: "action-" + (control.fields.name || control.fields.text || control.fields.id)
            text: control.fields.text || ""
            primary: !!control.fields.primary
            hint: control.fields.tip || ""
            onClicked: {
                if (control.fields.actions && control.fields.actions.length) menu.open()
                else ui.activate(control.fields.id)
            }
            ActionMenu {
                id: menu
                entries: control.fields.actions || []
                onSelected: entry.button ? ui.activate(entry.id) : ui.triggerAction(entry.id)
            }
        }
    }
    Component {
        id: checkBody
        SCheck { objectName: "check-" + (control.fields.name || control.fields.id); text: control.fields.text || ""; checked: !!control.fields.value; onClicked: ui.activate(control.fields.id) }
    }
    Component {
        id: numberBody
        NumberField { fields: control.fields }
    }
    Component {
        id: textBody
        SField {
            objectName: "input-" + (control.fields.name || control.fields.id)
            text: control.fields.kind === "date" ? control.fields.value : control.fields.value || ""
            placeholderText: control.fields.placeholder || ""
            readOnly: !!control.fields.readOnly
            maximumLength: control.fields.maxLength || 32767
            echoMode: control.fields.password ? TextInput.Password : TextInput.Normal
            Accessible.name: control.fields.accessibleName || control.fields.label || control.fields.tip || control.fields.name
            onTextEdited: ui.inputText(control.fields.id,text)
            onEditingFinished: ui.finishText(control.fields.id)
        }
    }
    Component {
        id: choiceBody
        Column {
            spacing: Theme.compact ? 3 : 6
            SChoice {
                width: parent.width
                searchable: !!control.fields.searchable
                optionImages: control.fields.optionImages || []
                objectName: "choice-" + (control.fields.name || control.fields.id)
                model: control.fields.options || []
                currentIndex: control.fields.value
                Accessible.name: control.fields.accessibleName || control.fields.label || control.fields.tip || control.fields.name
                onActivated: function(index) { ui.edit(control.fields.id,index) }
            }
            SField {
                visible: !!control.fields.editable
                width: parent.width
                text: control.fields.text || ""
                Accessible.name: control.fields.accessibleName || control.fields.label || qsTr("Własna wartość")
                onEditingFinished: ui.edit(control.fields.id,text)
            }
        }
    }
    Component {
        id: sliderBody
        Slider {
            id: slider
            implicitHeight: Theme.compact ? 28 : 40
            from: control.fields.minimum; to: control.fields.maximum; value: control.fields.value; stepSize: control.fields.step || 1
            Accessible.name: control.fields.accessibleName || control.fields.label || control.fields.tip || control.fields.name
            onMoved: ui.edit(control.fields.id,value)
            background: Rectangle {
                x: slider.leftPadding; y: slider.topPadding + slider.availableHeight/2-height/2
                width: slider.availableWidth; height: 6; radius: 3; color: Theme.raised
                Rectangle { width: slider.visualPosition * parent.width; height: parent.height; radius: 3; color: Theme.accent }
            }
            handle: Rectangle {
                x: slider.leftPadding + slider.visualPosition*(slider.availableWidth-width)
                y: slider.topPadding + slider.availableHeight/2-height/2
                width: 24; height: 24; radius: 6; color: Theme.accent
                border.width: slider.activeFocus ? 3 : 0; border.color: Theme.ink
            }
        }
    }
    Component {
        id: richBody
        TextEdit {
            text: control.fields.text || ""
            textFormat: TextEdit.AutoText
            readOnly: true
            selectByMouse: true
            activeFocusOnTab: true
            selectionColor: Theme.selection
            selectedTextColor: Theme.ink
            color: Theme.ink
            font.pointSize: Theme.body
            wrapMode: TextEdit.Wrap
            onLinkActivated: Qt.openUrlExternally(link)
        }
    }
    Component {
        id: multilineBody
        ScrollView {
            implicitHeight: 140
            TextArea {
                text: control.fields.text || ""
                color: Theme.ink; font.pointSize: Theme.body
                selectByMouse: true
                wrapMode: TextEdit.Wrap
                Accessible.name: control.fields.accessibleName || control.fields.label || control.fields.tip || control.fields.name
                background: Rectangle { color: Theme.raised; radius: Theme.radius; border.color: parent.activeFocus ? Theme.accent : Theme.outline }
                onActiveFocusChanged: if (!activeFocus && text !== control.fields.text) ui.edit(control.fields.id,text)
            }
        }
    }
    Component { id: tableBody; DataTable { table: control.fields.table; ownerId: control.fields.id; actions: control.fields.tableActions || []; followCurrent: !!control.fields.follow; title: control.fields.label || ""; maximumHeight: control.tableHeightLimit; compact: control.compactTable; searchEnabled: control.tableSearchEnabled; raceResults: control.raceResults } }
    Component { id: chartBody; PlotPanel { chart: control.fields.chart } }
    Component {
        id: progressBody
        Column {
            Label { text: control.fields.text || ""; color: Theme.ink; font.pointSize: Theme.caption }
            ProgressBar {
                id: progress
                width: parent.width; implicitHeight: 12
                from: control.fields.minimum; to: control.fields.maximum; value: control.fields.value; indeterminate: from === to
                background: Rectangle { implicitHeight: 12; radius: 4; color: Theme.raised }
                contentItem: Item {
                    Rectangle {
                        width: progress.indeterminate ? parent.width*.25 : progress.visualPosition*parent.width
                        height: parent.height; radius: 4; color: Theme.accent
                        SequentialAnimation on x {
                            running: progress.visible && progress.indeterminate && Theme.motion > 0
                            loops: Animation.Infinite
                            NumberAnimation { to: progress.availableWidth*.75; duration: 750; easing.type: Easing.InOutSine }
                            NumberAnimation { to: 0; duration: 750; easing.type: Easing.InOutSine }
                        }
                    }
                }
            }
        }
    }
}
