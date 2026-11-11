import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
Flow {
    id: tools
    property var backend
    spacing: Theme.dp(6)
    Repeater {
        model: [
            {op:"copy-order",icon:"⇥",tip:qsTr("Kolejność startowa z konkursu"),group:0},
            {op:"copy-exact",icon:"⧉",tip:qsTr("Zastąp listę listą z konkursu"),group:0},
            {op:"results",icon:"▤",tip:qsTr("Sortuj wynikami konkursu"),group:0},
            {op:"classification",icon:"🏆",tip:qsTr("Sortuj klasyfikacją"),group:0},
            {op:"reverse",icon:"⇅",tip:qsTr("Odwróć kolejność"),group:1},
            {op:"random",icon:"⚄",tip:qsTr("Losowa kolejność"),group:1},
            {op:"ovr",icon:"OVR",tip:qsTr("Sortuj według OVR"),group:1},
            {op:"strength",icon:"⚡",tip:qsTr("Sortuj według aktualnego potencjału"),group:1},
            {op:"name",icon:"A–Z",tip:qsTr("Sortuj alfabetycznie"),group:2},
            {op:"country",icon:"⚑",tip:qsTr("Kraje alfabetycznie"),group:2},
            {op:"countries-manual",icon:"⚑↕",tip:qsTr("Ustal kolejność krajów"),group:2}
        ]
        delegate: Row {
            required property var modelData
            required property int index
            visible: !tools.backend.teams || modelData.op!=="country" && modelData.op!=="countries-manual"
            spacing: Theme.dp(6)
            Rectangle { visible: index===4 || index===8 && !tools.backend.teams; width: 1; height: Theme.dp(24); anchors.verticalCenter: parent.verticalCenter; color: Theme.outline }
            SToolButton { text: modelData.icon; hint: modelData.tip; Accessible.name: hint; onClicked: tools.backend.tool(modelData.op) }
        }
    }
}
