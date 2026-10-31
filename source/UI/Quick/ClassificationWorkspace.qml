import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: workspace
    ViewArrival { target: workspace; expressive: true; emphasis: arrivalHeading }
    objectName: "classificationWorkspace"
    property var node
    readonly property var fields: node ? node.values : ({})
    readonly property var controls: fields.controls || ({})
    readonly property string classificationId: fields.classificationId || ""
    readonly property int methodCount: (value("classificationScoringMethod").options || []).length
    onMethodCountChanged: if(methodCount>1)multipleMethods=true
    property bool multipleMethods: false
    property int pointsSource: 0
    function value(name) { return controls[name] ? controls[name].values : ({}) }
    function initialize() { multipleMethods=methodCount>1;pointsSource=0 }
    onClassificationIdChanged: initialize()
    Component.onCompleted: initialize()
    spacing: 12
    Pane {
        Layout.fillWidth: true; padding: 14
        background: Surface { fillColor: Theme.surface; sweep: 22 }
        contentItem: ColumnLayout {
            spacing: 8
            Label { id: arrivalHeading; text: qsTr("Klasyfikacja"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading }
            ColumnLayout {
                Layout.fillWidth: true; spacing: Theme.dp(4)
                Label { text: qsTr("Nazwa"); color: Theme.muted; font.pointSize: Theme.caption }
                ClassificationInput { node: workspace.controls.lineEdit_name || null; showLabel: false; Layout.fillWidth: true; Layout.minimumWidth: 0 }
                Label { text: qsTr("Rodzaj klasyfikacji"); color: Theme.muted; font.pointSize: Theme.caption; Layout.topMargin: Theme.dp(4) }
                ClassificationInput { node: workspace.controls.comboBox_type || null; showLabel: false; Layout.fillWidth: true; Layout.minimumWidth: 0 }
            }
        }
    }
    Pane {
        Layout.fillWidth: true; padding: 14
        background: Surface { fillColor: Theme.surface; sweep: 22 }
        contentItem: ColumnLayout {
            spacing: 10
            Label { text: qsTr("Punktacja"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading }
            CheckHelp {
                Layout.fillWidth: true
                controlName: "classificationMultipleMethods"
                text: qsTr("Licz punkty na różne sposoby")
                checked: workspace.multipleMethods
                explanation: qsTr("Różne konkursy mogą dodawać punkty do klasyfikacji w różny sposób. Na przykład konkursy duetów mogą dodawać mniej punktów do klasyfikacji drużynowej, niż \"czteroosobówki\". Za to kwalifikacje indywidualne mogą dodawać przykładowo 5 punktów do rankingu dla zwycięzcy.")
                onClicked: {workspace.multipleMethods=value;if(!value)ui.edit(workspace.value("classificationScoringMethod").id,0)}
            }
            ClassificationMethods { visible: workspace.multipleMethods; Layout.fillWidth: true; workspace: workspace }
            RowLayout {
                Layout.fillWidth: true
                ClassificationInput { node: workspace.controls.comboBox_points || null; showLabel: false; Layout.fillWidth: true; Layout.minimumWidth: 0 }
                HelpDot { explanation: qsTr("Dodaje notę z konkursu (np. 276,4 pkt), zamiast punktów za miejsce — jak w Turnieju Złotego Bażanta czy Turnieju Nordyckim.") }
            }
            ClassificationPoints { visible: workspace.value("comboBox_points").value===0; Layout.fillWidth: true; workspace: workspace }
        }
    }
    ClassificationFilters { Layout.fillWidth: true; workspace: workspace }
}
