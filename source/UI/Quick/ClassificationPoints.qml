import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: points
    property var workspace
    readonly property bool alternative: workspace.pointsSource===1
    readonly property var tableNode: alternative ? workspace.controls.tableView_altPointsForPlaces : workspace.controls.tableView_pointsForPlaces
    readonly property var table: tableNode ? tableNode.values.table : null
    readonly property string prefix: alternative ? "classificationAlt" : "classification"
    spacing: 8
    RowLayout {
        Layout.fillWidth: true; spacing: 4
        SChoice {
            objectName: "classificationPointsSource"
            Layout.fillWidth: true; Layout.minimumWidth: 0
            model: [qsTr("Punkty za miejsce w konkursie"),points.workspace.fields.alternativeLabel || ""]
            currentIndex: points.workspace.pointsSource
            Accessible.name: qsTr("Źródło punktów")
            onActivated: points.workspace.pointsSource=currentIndex
        }
        HelpDot { explanation: points.workspace.fields.alternativeHelp || "" }
    }
    Label { visible: points.alternative && points.width<560; Layout.fillWidth: true; text: points.workspace.fields.alternativeLabel || ""; color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap }
    Flow {
        Layout.fillWidth: true; spacing: 6
        SChoice {
            id: preset
            objectName: "classificationPointsPreset"
            readonly property var field: points.workspace.value(points.alternative ? "comboBox_altPreset" : "comboBox_presets")
            width: Math.min(220,points.width)
            model: {var options=(field.options || []).slice();if(options.length)options[0]=qsTr("Wybierz preset");return options}
            currentIndex: field.value || 0
            displayText: qsTr("Wybierz preset")
            enabled: field.enabled!==false
            Accessible.name: qsTr("Preset punktacji")
            onActivated: if(currentIndex>0)ui.edit(field.id,currentIndex)
        }
        SButton { text: "+"; padding: 4; implicitHeight: 28; Accessible.name: qsTr("Dodaj punktowane miejsce"); enabled: points.workspace.value(points.prefix+"AddPlace").enabled!==false; onClicked: ui.activate(points.workspace.value(points.prefix+"AddPlace").id) }
        SButton { text: "−"; padding: 4; implicitHeight: 28; Accessible.name: qsTr("Usuń ostatnie miejsce"); enabled: points.table && points.table.count>0 && points.workspace.value(points.prefix+"RemovePlace").enabled!==false; onClicked: ui.activate(points.workspace.value(points.prefix+"RemovePlace").id) }
        SButton { text: qsTr("Wyczyść"); quiet: true; padding: 4; implicitHeight: 28; font.pointSize: Theme.caption; enabled: points.table && points.table.count>0 && preset.field.enabled!==false; onClicked: ui.edit(preset.field.id,0) }
    }
    Rectangle {
        Layout.fillWidth: true; implicitHeight: 26
        color: Theme.stadium; radius: 4
        RowLayout {
            anchors.fill: parent; anchors.leftMargin: 10; anchors.rightMargin: 10
            Label { text: qsTr("Miejsce"); color: Theme.snow; font.pointSize: Theme.caption; Layout.fillWidth: true }
            Label { text: qsTr("Punkty"); color: Theme.snow; font.pointSize: Theme.caption; Layout.preferredWidth: 110; horizontalAlignment: Text.AlignRight }
        }
    }
    ListView {
        id: rows
        objectName: "classificationPointsRows"
        Layout.fillWidth: true; Layout.preferredHeight: Math.min(300,Math.max(56,count*34))
        model: points.table; clip: true; spacing: 2
        ScrollBar.vertical: ScrollBar {}
        delegate: Rectangle {
            id: entry
            required property int index
            required property var cells
            width: rows.width; height: 32
            color: index%2 ? Theme.raised : "transparent"; radius: 4
            RowLayout {
                anchors.fill: parent; anchors.leftMargin: 10; anchors.rightMargin: 10
                Label { text: entry.index+1; color: Theme.ink; font.family: Theme.mono; font.pointSize: Theme.body; Layout.fillWidth: true }
                SField {
                    property bool dirty: false
                    Layout.preferredWidth: 110; Layout.preferredHeight: 28
                    text: entry.cells[1] ? entry.cells[1].text : ""
                    enabled: points.tableNode && points.tableNode.values.enabled!==false
                    font.family: Theme.mono; horizontalAlignment: TextInput.AlignRight
                    Accessible.name: qsTr("Punkty za miejsce %1").arg(entry.index+1)
                    validator: DoubleValidator { locale: Qt.locale().name }
                    onTextEdited: dirty=true
                    onEditingFinished: if(dirty){dirty=false;points.table.edit(entry.index,1,text)}
                }
            }
        }
        Label { anchors.centerIn: parent; visible: rows.count===0; text: qsTr("Wybierz preset lub dodaj punktowane miejsce."); color: Theme.muted; font.pointSize: Theme.caption; width: parent.width; wrapMode: Text.WordWrap; horizontalAlignment: Text.AlignHCenter }
    }
}
