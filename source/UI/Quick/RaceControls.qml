import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

GridLayout {
    id: controls
    signal optionsRequested()
    columns: width >= 700 ? 3 : 2
    columnSpacing: 8; rowSpacing: 6
    Node { visible: !ui.context.complete && !ui.context.roundComplete; Layout.fillWidth: true; Layout.minimumWidth: 0; node: ui.find("pushButton_jump") }
    Node { Layout.fillWidth: true; Layout.minimumWidth: 0; node: ui.find("pushButton_goToNext") }
    RowLayout {
        Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.maximumWidth: controls.width >= 700 ? 220 : 9999
        Label { text: qsTr("Belka"); color: Theme.muted; font.pointSize: Theme.caption }
        Node { Layout.fillWidth: true; Layout.minimumWidth: 90; node: ui.find("spinBox_actualGate") }
    }
    SButton { objectName: "raceOptions"; text: qsTr("Opcje"); quiet: true; Layout.fillWidth: controls.width < 700; onClicked: controls.optionsRequested() }
}
