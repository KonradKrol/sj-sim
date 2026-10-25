import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: workspace
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
            Label { text: qsTr("Klasyfikacja"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading }
            GridLayout {
                columns: workspace.width<440 ? 1 : 2
                Layout.fillWidth: true; columnSpacing: 16; rowSpacing: 4
                Label { text: qsTr("Nazwa"); color: Theme.muted; font.pointSize: Theme.caption; Layout.row: 0; Layout.column: 0 }
                ClassificationInput { node: workspace.controls.lineEdit_name || null; showLabel: false; Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.row: 1; Layout.column: 0 }
                Label { text: qsTr("Rodzaj klasyfikacji"); color: Theme.muted; font.pointSize: Theme.caption; Layout.row: workspace.width<440 ? 2 : 0; Layout.column: workspace.width<440 ? 0 : 1 }
                ClassificationInput { node: workspace.controls.comboBox_type || null; showLabel: false; Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.row: workspace.width<440 ? 3 : 1; Layout.column: workspace.width<440 ? 0 : 1 }
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
            Pane {
                visible: workspace.multipleMethods
                Layout.fillWidth: true; padding: 10
                background: Rectangle { color: Theme.selection; radius: 8; border.color: Theme.outline }
                contentItem: ColumnLayout {
                    spacing: 6
                    Label { text: qsTr("Sposób naliczania punktów"); color: Theme.ink; font.pointSize: Theme.body; font.weight: Font.DemiBold }
                    RowLayout {
                        Layout.fillWidth: true
                        ClassificationInput { node: workspace.controls.classificationScoringMethod || null; showLabel: false; Layout.fillWidth: true; Layout.minimumWidth: 0 }
                        SButton { objectName: "classificationNewMethod"; text: "+"; padding: 4; implicitHeight: 28; Accessible.name: qsTr("Dodaj sposób jako kopię bieżącego"); enabled: workspace.value("classificationAddMethod").enabled!==false; onClicked: ui.activate(workspace.value("classificationAddMethod").id) }
                        SButton { text: "−"; padding: 4; implicitHeight: 28; Accessible.name: qsTr("Usuń sposób"); enabled: workspace.value("classificationRemoveMethod").enabled!==false; onClicked: ui.activate(workspace.value("classificationRemoveMethod").id) }
                    }
                    ClassificationInput { visible: workspace.value("classificationScoringMethod").value>0; node: workspace.controls.classificationMethodName || null; showLabel: true; Layout.fillWidth: true }
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Label { text: qsTr("Naliczanie punktów"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true }
                ClassificationInput { node: workspace.controls.comboBox_points || null; showLabel: false; Layout.preferredWidth: 200; Layout.maximumWidth: workspace.width*.6; Layout.minimumWidth: 0 }
            }
            ClassificationPoints { visible: workspace.value("comboBox_points").value===0; Layout.fillWidth: true; workspace: workspace }
        }
    }
    ClassificationFilters { Layout.fillWidth: true; workspace: workspace }
}
