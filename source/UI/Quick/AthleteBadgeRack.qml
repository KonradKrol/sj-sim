import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: rack
    property var backend
    property var design
    property string family: "medal"
    property bool ribbons: false
    property bool vertical: false
    AthletePerkCatalog { id: catalog }
    spacing: Theme.dp(10)
    RowLayout {
        Layout.fillWidth: true
        Label { text: qsTr("Perki"); color: rack.design.ink; font.family: rack.design.headingFont; font.pointSize: Theme.heading; font.bold: true }
        Label { text: qsTr("%1 / 3").arg((rack.backend.profile.perks || []).filter(function(v){return v!==0}).length); color: rack.design.muted; font.family: Theme.mono; font.pointSize: Theme.caption }
        Item { Layout.fillWidth: true }
        HelpDot { explanation: qsTr("Wybierz jeden perk z każdej pary lub pozostaw ją pustą. Kliknij wybrany ponownie, aby go usunąć. Przycisk I ↑ wzmacnia perk do II. Opis: najedź na odznakę lub naciśnij F1.") }
    }
    GridLayout {
        Layout.fillWidth: true
        columns: rack.vertical || rack.width<Theme.dp(rack.ribbons ? 900 : 690) ? 1 : 3
        columnSpacing: Theme.dp(22); rowSpacing: Theme.dp(12)
        Repeater {
            model: 3
            ColumnLayout {
                id: pair
                required property int index
                Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.alignment: Qt.AlignTop
                spacing: Theme.dp(7)
                RowLayout {
                    Layout.fillWidth: true
                    Label { text: catalog.pairs[pair.index]; color: rack.design.muted; font.family: rack.design.detailFont; font.pointSize: Theme.caption }
                    Rectangle { Layout.fillWidth: true; height: 1; color: rack.design.line }
                }
                GridLayout {
                    Layout.fillWidth: true
                    columns: rack.ribbons && rack.width<Theme.dp(500) ? 1 : 2
                    columnSpacing: Theme.dp(8); rowSpacing: Theme.dp(5)
                    Repeater {
                        model: 2
                        AthleteBadge {
                            required property int index
                            Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.alignment: Qt.AlignTop
                            backend: rack.backend; design: rack.design; family: rack.family; ribbon: rack.ribbons
                            perk: catalog.items[pair.index*2+index]
                        }
                    }
                }
            }
        }
    }
}
