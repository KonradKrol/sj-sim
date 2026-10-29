import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: points
    ViewArrival { target: points; expressive: false }
    property var workspace
    readonly property bool alternative: workspace.pointsSource===1
    readonly property var tableNode: alternative ? workspace.controls.tableView_altPointsForPlaces : workspace.controls.tableView_pointsForPlaces
    readonly property var table: tableNode ? tableNode.values.table : null
    readonly property string prefix: alternative ? "classificationAlt" : "classification"
    spacing: 8
    ClassificationSources { Layout.fillWidth: true; workspace: points.workspace }
    Flow {
        Layout.fillWidth: true; spacing: 6
        SChoice {
            id: preset
            objectName: "classificationPointsPreset"
            readonly property var field: points.workspace.value(points.alternative ? "comboBox_altPreset" : "comboBox_presets")
            width: Math.min(220,points.width)
            model: field.options || []
            optionEnabled: (field.options || []).map(function(_,index){return index>0})
            currentIndex: field.value || 0
            displayText: currentText || qsTr("Własne")
            enabled: field.enabled!==false
            Accessible.name: qsTr("Preset punktacji")
            onActivated: function(index){if(index===0){currentIndex=Qt.binding(function(){return preset.field.value || 0});return}points.rememberScroll();ui.edit(field.id,index)}
        }
        SButton { objectName: "classificationPointAdd"; text: "+"; padding: 4; implicitHeight: 28; Accessible.name: qsTr("Dodaj punktowane miejsce"); enabled: points.workspace.value(points.prefix+"AddPlace").enabled!==false; onClicked: {points.rememberScroll();ui.activate(points.workspace.value(points.prefix+"AddPlace").id)} }
        SButton { objectName: "classificationPointRemove"; text: "−"; padding: 4; implicitHeight: 28; Accessible.name: qsTr("Usuń ostatnie miejsce"); enabled: points.table && points.table.count>0 && points.workspace.value(points.prefix+"RemovePlace").enabled!==false; onClicked: {points.rememberScroll();ui.activate(points.workspace.value(points.prefix+"RemovePlace").id)} }
        SButton { objectName: "classificationPointClear"; text: qsTr("Wyczyść"); quiet: true; padding: 4; implicitHeight: 28; font.pointSize: Theme.caption; enabled: points.table && points.table.count>0 && preset.field.enabled!==false; onClicked: {points.rememberScroll();ui.edit(preset.field.id,0)} }
    }
    RowLayout {
        Layout.fillWidth: true
        Label { text: qsTr("Miejsce → punkty"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true }
        Label { text: qsTr("Miejsca: %1").arg(rows.count); color: Theme.muted; font.pointSize: Theme.caption }
    }
    GridView {
        id: rows
        objectName: "classificationPointsRows"
        Layout.fillWidth: true
        readonly property int columns: Math.max(1,Math.floor(width/Math.max(110,Theme.bodyMetrics.height*7)))
        cellWidth: width/columns
        cellHeight: Math.max(32,Theme.bodyMetrics.height+10)
        Layout.preferredHeight: Math.min(240,Math.max(cellHeight*2,Math.ceil(count/columns)*cellHeight))
        model: points.table; clip: true
        currentIndex: -1
        highlightFollowsCurrentItem: false
        keyNavigationEnabled: false
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar {}
        delegate: Item {
            id: entry
            required property int index
            required property var cells
            width: rows.cellWidth; height: rows.cellHeight
            RowLayout {
                anchors.fill: parent; anchors.rightMargin: 8
                spacing: 4
                Label { text: (entry.index+1)+"."; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption; Layout.preferredWidth: 26 }
                SField {
                    property bool dirty: false
                    Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredHeight: rows.cellHeight-4
                    text: entry.cells[1] ? entry.cells[1].text : ""
                    enabled: points.tableNode && points.tableNode.values.enabled!==false
                    font.family: Theme.mono; horizontalAlignment: TextInput.AlignRight
                    Accessible.name: qsTr("Punkty za miejsce %1").arg(entry.index+1)
                    validator: DoubleValidator { locale: Qt.locale(Theme.localeName).name }
                    onTextEdited: dirty=true
                    onEditingFinished: if(dirty){dirty=false;points.rememberScroll();points.table.edit(entry.index,1,text)}
                }
            }
        }
        Label { anchors.centerIn: parent; visible: rows.count===0; text: qsTr("Wybierz preset lub dodaj punktowane miejsce."); color: Theme.muted; font.pointSize: Theme.caption; width: parent.width; wrapMode: Text.WordWrap; horizontalAlignment: Text.AlignHCenter }
    }
    property real savedY: 0
    property real savedOuterY: 0
    property bool preserving: false
    function outerViewport(){for(var item=parent;item;item=item.parent)if(item.contentY!==undefined && item.contentHeight!==undefined)return item;return null}
    function rememberScroll(){if(preserving)return;savedY=rows.contentY;var outer=outerViewport();savedOuterY=outer ? outer.contentY : 0;preserving=true;restoreTimer.restart()}
    function restoreScroll(){rows.contentY=Math.max(0,Math.min(savedY,rows.contentHeight-rows.height));var outer=outerViewport();if(outer)outer.contentY=Math.max(0,Math.min(savedOuterY,outer.contentHeight-outer.height));preserving=false}
    onTableChanged: {restoreTimer.stop();preserving=false;rows.contentY=0}
    Timer { id: restoreTimer; interval: 32; onTriggered: points.restoreScroll() }
    Connections {
        target: points.table
        function onModelAboutToBeReset(){points.rememberScroll()}
        function onModelReset(){rows.currentIndex=-1;restoreTimer.restart()}
        function onRowsAboutToBeInserted(){points.rememberScroll()}
        function onRowsAboutToBeRemoved(){points.rememberScroll()}
        function onRowsInserted(){restoreTimer.restart()}
        function onRowsRemoved(){restoreTimer.restart()}
        function onDataChanged(){if(points.preserving)restoreTimer.restart()}
    }
}
