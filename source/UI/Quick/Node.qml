import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

FocusScope {
    id: control
    property bool showLabel: true
    property var node
    property real tableHeightLimit: 540
    property bool compactTable: false
    property bool tableSearchEnabled: false
    property bool plainTable: false
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
    RowLayout {
        width: parent.width
        visible: control.showLabel && !!control.fields.label
        Label {
            text: control.fields.label || ""
            Layout.fillWidth: true
            color: Theme.muted
            font.pointSize: Theme.caption
            wrapMode: Text.WordWrap
        }
        HelpDot {
            visible: control.kind!=="check" && control.kind!=="button" && (!!control.fields.help || control.fields.name==="comboBox_windMeasurementType")
            explanation: control.fields.help || control.fields.tip || ""
        }
    }
    Loader {
        id: body
        width: parent.width
        sourceComponent: control.kind === "simulationPreferences" ? simulationPreferencesBody : control.kind === "pointsPresetEditor" ? pointsPresetBody : control.kind === "seasonRosters" ? seasonRostersBody : control.kind === "seasonRosterChoice" ? seasonRosterChoiceBody : control.kind === "seasonAutosave" ? seasonAutosaveBody : control.kind === "seasonOverview" ? seasonOverviewBody : control.kind === "classificationEditor" ? classificationBody : control.kind === "empty" ? emptyBody : control.kind === "roster" ? rosterBody : control.kind === "rosterSummary" ? rosterSummary : control.kind === "calendar" ? calendarBody : control.kind === "wind" ? windBody : control.kind === "label" ? labelBody : control.kind === "button" ? buttonBody : control.kind === "check" ? checkBody : control.kind === "number" ? numberBody : control.kind === "text" || control.kind === "date" ? textBody : control.kind === "choice" ? choiceBody : control.kind === "slider" ? sliderBody : control.kind === "richtext" ? richBody : control.kind === "multiline" ? multilineBody : control.kind === "table" ? tableBody : control.kind === "chart" ? chartBody : control.kind === "progress" ? progressBody : groupBody
    }
    }
    Component { id: simulationPreferencesBody; SimulationPreferences { node: control.node } }
    Component { id: seasonRostersBody; SeasonRostersWorkspace { node: control.node } }
    Component { id: seasonOverviewBody; SeasonOverview { node: control.node } }
    Component { id: seasonAutosaveBody; SeasonAutosave { node: control.node } }
    Component { id: seasonRosterChoiceBody; SeasonRosterChoice { backend: control.node ? control.node.values.backend : null; navigateOnSelection: !!control.fields.navigateOnSelection } }
    Component { id: pointsPresetBody; PointsPresetEditor { node: control.node } }
    Component { id: classificationBody; ClassificationWorkspace { node: control.node } }
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
        RowLayout {
            spacing: Theme.dp(8)
        SButton {
            Layout.fillWidth: true; Layout.minimumWidth: 0
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
            HelpDot {
                visible: control.fields.name==="openBalanceLab"
                prominent: true
                explanation: control.fields.help || ""
            }
        }
    }

    Component {
        id: checkBody
        RowLayout {
            SCheck { objectName: "check-" + (control.fields.name || control.fields.id); Layout.fillWidth: true; Layout.minimumWidth: 0; text: control.fields.text || ""; checked: !!control.fields.value; onClicked: ui.activate(control.fields.id) }
            HelpDot { visible: !!control.fields.help; explanation: control.fields.help || "" }
        }
    }
    Component {
        id: numberBody
        Column {
            id: dsqControl
            width: control.width; spacing: Theme.dp(5)
            readonly property bool dsq: control.fields.name === "spinBox_dsqProbability"
            readonly property var levels: [0,400,190,130,60]
            readonly property var names: [qsTr("Wyłączone"),qsTr("Bardzo rzadkie"),qsTr("Rzadkie"),qsTr("Częste"),qsTr("Bardzo częste")]
            readonly property int current: Number(control.fields.value) <= 0 ? 0 : levels.indexOf(Number(control.fields.value))
            NumberField { visible: !parent.dsq; width: parent.width; fields: control.fields }
            SChoice {
                id: dsqChoice
                objectName: "dsqLevelChoice"
                visible: dsqControl.dsq; width: parent.width
                implicitHeight: Math.max(Theme.dp(50),Theme.bodyMetrics.height+Theme.dp(18))
                leftPadding: Theme.dp(8); rightPadding: Theme.dp(32)
                model: dsqControl.current<0 ? dsqControl.names.concat([qsTr("Własna częstość")]) : dsqControl.names
                currentIndex: dsqControl.current<0 ? 5 : dsqControl.current
                Accessible.name: qsTr("Częstość dyskwalifikacji")
                onActivated: function(index) { if(index<dsqControl.levels.length)ui.edit(control.fields.id,dsqControl.levels[index]) }
                contentItem: RowLayout {
                    spacing: Theme.dp(9)
                    Rectangle {
                        Layout.preferredWidth: Theme.dp(32); Layout.preferredHeight: Theme.dp(32)
                        radius: Theme.dp(7); color: Theme.selection
                        Row {
                            anchors.centerIn: parent; spacing: Theme.dp(2); visible: dsqControl.current>0
                            Repeater { model: 4; Rectangle { required property int index; width: Theme.dp(3); height: Theme.dp(5+index*3); anchors.bottom: parent.bottom; radius: Theme.dp(1); color: index<dsqControl.current ? Theme.accent : Theme.outline; opacity: index<dsqControl.current ? 1 : .35 } }
                        }
                        Text { anchors.centerIn: parent; visible: dsqControl.current<=0; text: dsqControl.current<0 ? "?" : "—"; color: Theme.accent; font.bold: true; font.pointSize: Theme.body }
                    }
                    Text { text: dsqChoice.displayText; color: Theme.ink; font.family: dsqChoice.font.family; font.pointSize: dsqChoice.font.pointSize; font.weight: Font.DemiBold; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight; verticalAlignment: Text.AlignVCenter }
                }
                background: Rectangle {
                    color: dsqChoice.hovered ? Theme.selection : Theme.raised; radius: Theme.dp(8)
                    border.width: dsqChoice.activeFocus ? 2 : 1; border.color: dsqChoice.activeFocus ? Theme.accent : Theme.outline
                    Row {
                        id: dsqScale
                        anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom
                        anchors.leftMargin: Theme.dp(8); anchors.rightMargin: Theme.dp(8); anchors.bottomMargin: Theme.dp(3)
                        spacing: Theme.dp(3); height: Theme.dp(2); Accessible.ignored: true
                        Repeater { model: 5; Rectangle { required property int index; width: Math.max(0,(dsqScale.width-dsqScale.spacing*4)/5); height: dsqScale.height; radius: Theme.dp(1); color: index===dsqControl.current ? Theme.accent : Theme.outline; opacity: index===dsqControl.current ? 1 : .2 } }
                    }
                }
                delegate: ItemDelegate {
                    id: dsqOption
                    required property int index
                    objectName: "dsqOption-"+index
                    width: dsqChoice.popup.availableWidth; height: Math.max(Theme.dp(42),Theme.bodyMetrics.height+Theme.dp(14))
                    text: dsqChoice.textAt(index); highlighted: dsqChoice.highlightedIndex===index
                    contentItem: RowLayout {
                        spacing: Theme.dp(9)
                        Rectangle {
                            Layout.preferredWidth: Theme.dp(28); Layout.preferredHeight: Theme.dp(28); radius: Theme.dp(6)
                            color: dsqOption.index===dsqChoice.currentIndex ? Theme.selection : Theme.raised
                            Row {
                                anchors.centerIn: parent; spacing: Theme.dp(2); visible: dsqOption.index>0 && dsqOption.index<5
                                Repeater { model: 4; Rectangle { required property int index; width: Theme.dp(3); height: Theme.dp(4+index*3); anchors.bottom: parent.bottom; radius: Theme.dp(1); color: index<dsqOption.index ? Theme.accent : Theme.outline; opacity: index<dsqOption.index ? 1 : .35 } }
                            }
                            Text { anchors.centerIn: parent; visible: dsqOption.index===0 || dsqOption.index===5; text: dsqOption.index===0 ? "—" : "?"; color: Theme.accent; font.bold: true; font.pointSize: Theme.caption }
                        }
                        Text { text: dsqOption.text; color: Theme.ink; font.family: dsqChoice.font.family; font.pointSize: dsqChoice.font.pointSize; font.weight: dsqOption.index===dsqChoice.currentIndex ? Font.DemiBold : Font.Normal; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight }
                        Text { visible: dsqOption.index===dsqChoice.currentIndex; text: "✓"; color: Theme.accent; font.bold: true; font.pointSize: Theme.body; Accessible.ignored: true }
                    }
                    background: Rectangle { color: dsqOption.highlighted ? Theme.selection : Theme.surface; radius: Theme.dp(5) }
                }
            }
            Label {
                visible: parent.dsq && parent.current<0; width: parent.width; wrapMode: Text.WordWrap
                color: Theme.muted; font.pointSize: Theme.caption
                text: qsTr("Zachowano wcześniejsze ustawienie. Wybierz poziom, aby je zmienić.")
            }
        }
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
                optionEnabled: control.fields.optionEnabled || []
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
    Component { id: tableBody; DataTable { showHeaders: control.fields.showHeaders!==false; ranking: !!control.fields.classificationRanking; teamSetup: !!control.fields.teamSetup; table: control.fields.table; ownerId: control.fields.id; actions: control.fields.tableActions || []; followCurrent: !!control.fields.follow; title: control.fields.label || ""; maximumHeight: control.tableHeightLimit; compact: control.compactTable; plain: control.plainTable || !!control.fields.plainTable; searchEnabled: control.tableSearchEnabled || !!control.fields.searchable; raceResults: control.raceResults } }
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
                        Behavior on width { enabled: !progress.indeterminate && Theme.motion>0; NumberAnimation { duration: Theme.motion; easing.type: Easing.OutCubic } }
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
