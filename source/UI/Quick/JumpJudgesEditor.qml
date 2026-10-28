import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: judges
    property var backend
    property real availableWidth
    readonly property var values: backend.values
    spacing: Theme.dp(8)
    RowLayout {
        Layout.fillWidth: true
        Label { text: qsTr("Noty sędziów"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.bold: true; Layout.fillWidth: true }
        HelpDot { explanation: qsTr("Zaznacz tylko noty do ustawienia ręcznie. Pozostałe zostaną wylosowane.") }
    }
    GridLayout {
        Layout.fillWidth: true; columns: judges.availableWidth>Theme.dp(560) ? 5 : judges.availableWidth>Theme.dp(360) ? 3 : 2
        columnSpacing: Theme.dp(8); rowSpacing: Theme.dp(8)
        Repeater {
            model: 5
            Control {
                id: mark
                required property int index
                readonly property real value: Number(judges.values.judges[index])
                Layout.fillWidth: true; Layout.minimumWidth: 0
                padding: Theme.dp(7); implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
                background: Rectangle { color: mark.value>=0 ? Theme.selection : Theme.surface; radius: Theme.dp(4); border.width: mark.value>=0 ? 2 : 1; border.color: mark.value>=0 ? Theme.accent : Theme.divider }
                contentItem: ColumnLayout {
                    spacing: Theme.dp(4)
                    SCheck {
                        objectName: "overrideJudge-"+mark.index
                        text: String.fromCharCode(65+mark.index); checked: mark.value>=0
                        onClicked: {var selected=checked;judges.forceActiveFocus();judges.backend.setJudge(mark.index,selected?18:-1)}
                        Accessible.name: qsTr("Ustaw notę sędziego %1").arg(text)
                    }
                    NumberField {
                        Layout.fillWidth: true; Layout.minimumWidth: 0
                        direct: false; flexibleDecimal: true; enabled: mark.value>=0
                        label: qsTr("Nota sędziego %1").arg(mark.index+1); recordKey: mark.index+":"+judges.values.revision
                        fields: ({value:Math.max(0,mark.value),minimum:0,maximum:20,step:.5,decimals:1,name:"judge-"+mark.index})
                        onCommitted: function(value){judges.backend.setJudge(mark.index,value)}
                    }
                }
            }
        }
    }
}
