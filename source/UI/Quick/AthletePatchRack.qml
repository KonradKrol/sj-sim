import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: rack
    property var backend
    property var design
    AthletePerkCatalog { id: catalog }
    spacing: Theme.dp(5)
    RowLayout {
        Layout.fillWidth: true
        Label { text: qsTr("Perki"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true }
        Label { text: qsTr("opcjonalne"); color: Theme.muted; font.pointSize: Theme.caption }
        Item { Layout.fillWidth: true }
        Label { text: qsTr("Wybrane: %1").arg((rack.backend.selectedPerks || []).filter(function(v){return v!==0}).length); color: Theme.muted; font.pointSize: Theme.caption }
        HelpDot { explanation: qsTr("Wybierz jeden perk z każdej pary lub pozostaw ją pustą. Trzy opcje siły są widoczne obok siebie. Kliknij zaznaczoną opcję ponownie, aby usunąć perk. Opis: najedź lub naciśnij F1.") }
    }
    Repeater {
        model: 3
        Control {
            id: category
            required property int index
            Layout.fillWidth: true
            implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
            padding: Theme.dp(6)
            background: Rectangle { color: Theme.surface; border.width: 1; border.color: Theme.divider; radius: Theme.dp(6) }
            contentItem: ColumnLayout {
                spacing: Theme.dp(5)
                RowLayout {
                    Layout.fillWidth: true
                    Rectangle { Layout.preferredWidth: Theme.dp(3); Layout.preferredHeight: Theme.dp(13); color: Theme.accent; radius: 1 }
                    Label { text: catalog.pairs[category.index]; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.weight: Font.Bold }
                    Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }
                }
                GridLayout {
                    Layout.fillWidth: true; columns: width<Theme.fontSize(640) ? 1 : 2
                    columnSpacing: Theme.dp(8); rowSpacing: Theme.dp(5)
                    Repeater {
                        model: 2
                        AthleteBadge {
                            required property int index
                            Layout.fillWidth: true; Layout.minimumWidth: 0
                            backend: rack.backend; design: rack.design; family: "patch"; ribbon: true; managedLevel: true
                            perk: catalog.items[category.index*2+index]
                        }
                    }
                }
            }
        }
    }
}
