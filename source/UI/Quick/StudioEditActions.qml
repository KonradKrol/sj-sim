import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

RowLayout {
    id: actions
    property var backend
    property bool athletes: false
    readonly property string prefix: athletes ? "athlete" : "hill"
    signal added()
    signal duplicated()
    spacing: Theme.dp(4)
    SIconButton {
        objectName: actions.prefix+"Add"
        text: ""; iconName: "add"; primary: true
        implicitWidth: Theme.dp(56); implicitHeight: Math.max(Theme.dp(44),Theme.controlHeight)
        actionName: actions.athletes ? qsTr("Dodaj skoczka") : qsTr("Dodaj skocznię")
        onClicked: {actions.backend.add();actions.added()}
    }
    Item { Layout.fillWidth: true }
    SIconButton { objectName: actions.prefix+"Undo"; text: "↶"; quiet: false; actionName: qsTr("Cofnij"); enabled: actions.backend.canUndo; onClicked: actions.backend.undo() }
    SIconButton { objectName: actions.prefix+"Redo"; text: "↷"; quiet: false; actionName: qsTr("Ponów"); enabled: actions.backend.canRedo; onClicked: actions.backend.redo() }
    Rectangle { Layout.preferredWidth: 1; Layout.preferredHeight: Theme.dp(22); Layout.leftMargin: Theme.dp(4); Layout.rightMargin: Theme.dp(4); color: Theme.outline }
    SIconButton { objectName: actions.prefix+"Duplicate"; text: ""; iconName: "copy"; quiet: false; actionName: actions.athletes ? qsTr("Duplikuj skoczka") : qsTr("Duplikuj skocznię"); enabled: actions.backend.selectedIndex>=0; onClicked: {actions.backend.duplicate();actions.duplicated()} }
    SIconButton { objectName: actions.prefix+"Remove"; text: ""; iconName: "delete"; quiet: false; actionName: actions.athletes ? qsTr("Usuń wybranego skoczka") : qsTr("Usuń skocznię"); foreground: Theme.danger; enabled: actions.backend.selectedIndex>=0; onClicked: {actions.forceActiveFocus();actions.backend.remove()} }
}
