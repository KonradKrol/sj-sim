import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: sources
    objectName: "classificationPointsSource"
    property var workspace
    readonly property bool individual: workspace.value("comboBox_type").value===0
    readonly property var names: individual ? [qsTr("Konkurs indywidualny"),qsTr("Konkurs drużynowy")] : [qsTr("Konkurs drużynowy"),qsTr("Konkurs indywidualny")]
    FontMetrics { id: metrics; font.family: Theme.bodyFamily; font.pointSize: Theme.body; font.weight: Font.DemiBold }
    spacing: 5
    RowLayout {
        Layout.fillWidth: true; spacing: 6
        Rectangle {
            Layout.fillWidth: true; Layout.minimumWidth: 0
            implicitHeight: metrics.height*Math.max(1,Math.ceil(Math.max(metrics.advanceWidth(sources.names[0]),metrics.advanceWidth(sources.names[1]))/Math.max(80,(sources.width-37)/2-8)))+16
            radius: 8; color: Theme.raised
            RowLayout {
                id: buttons
                anchors.fill: parent; anchors.margins: 3
                spacing: 3
                Repeater {
                    model: sources.names
                    SButton {
                        objectName: "classificationSource-"+index
                        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: 0; Layout.preferredWidth: 1
                        text: modelData; quiet: true; padding: 4
                        checkable: true; checked: sources.workspace.pointsSource===index
                        foreground: checked ? Theme.snow : Theme.muted
                        font.pointSize: Theme.body
                        background: Rectangle { color: parent.checked ? Theme.stadium : "transparent"; radius: 6; border.color: parent.activeFocus ? Theme.accent : "transparent" }
                        onClicked: sources.workspace.pointsSource=index
                    }
                }
            }
        }
        HelpDot { explanation: sources.workspace.fields.alternativeHelp || "" }
    }
    Label {
        Layout.fillWidth: true; wrapMode: Text.WordWrap
        text: sources.workspace.pointsSource===1 ? sources.workspace.fields.alternativeLabel || "" : sources.individual ? qsTr("Punkty zawodnika za miejsce w konkursie indywidualnym") : qsTr("Punkty drużyny za miejsce w konkursie drużynowym")
        color: Theme.muted; font.pointSize: Theme.caption
    }
}
