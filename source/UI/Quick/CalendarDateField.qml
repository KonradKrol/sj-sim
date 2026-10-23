import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: field
    property var backend
    property string value: ""
    property string name: "date"
    property string label: qsTr("Data")
    property bool mixed: false
    property string error: ""
    signal committed(string iso)
    spacing: 4
    Label { text: field.label; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    RowLayout {
        Layout.fillWidth: true
        SField {
            id: input
            objectName: "date-input-" + field.name
            Layout.fillWidth: true; Layout.minimumWidth: 0
            text: field.backend.dateText(field.value)
            placeholderText: field.mixed ? qsTr("Różne daty") : qsTr("Wybierz datę lub wpisz RRRR-MM-DD")
            Accessible.name: field.label
            onEditingFinished: {
                if(text === field.backend.dateText(field.value))return
                var iso=field.backend.normalizeDate(text)
                if(iso.length){field.error="";field.committed(iso)}
                else field.error=qsTr("Nieprawidłowa data. Użyj kalendarza lub formatu RRRR-MM-DD.")
            }
        }
        SButton { objectName: "date-picker-"+field.name; text: "▦"; Accessible.name: qsTr("Otwórz kalendarz: %1").arg(field.label); onClicked: picker.open() }
    }
    Label { visible: !!field.error; text: field.error; color: Theme.danger; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    Popup {
        id: picker
        parent: Overlay.overlay
        x: parent ? (parent.width-width)/2 : 0
        y: parent ? Math.max(0,(parent.height-height)/2) : 0
        focus: true
        property string month: field.value || field.backend.suggestedDate
        width: Math.min(360,Overlay.overlay ? Overlay.overlay.width-20 : 320)
        padding: 12
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        onOpened: month=field.value || field.backend.suggestedDate
        background: Rectangle { color: Theme.surface; radius: 12; border.color: Theme.outline }
        contentItem: Loader {
            active: picker.visible
            sourceComponent: ColumnLayout {
                id: pickerPage
                readonly property var page: field.backend.monthData(picker.month)
                spacing: 6
                RowLayout {
                    Layout.fillWidth: true
                    SButton { text: "‹"; Accessible.name: qsTr("Poprzedni miesiąc"); onClicked: picker.month=field.backend.offsetDate(picker.month,0,-1) }
                    Label { text: pickerPage.page.title; color: Theme.ink; font.pointSize: Theme.body; Layout.fillWidth: true; horizontalAlignment: Text.AlignHCenter }
                    SButton { text: "›"; Accessible.name: qsTr("Następny miesiąc"); onClicked: picker.month=field.backend.offsetDate(picker.month,0,1) }
                }
                GridLayout {
                    columns: 7; columnSpacing: 2; rowSpacing: 2; Layout.fillWidth: true
                    Repeater {
                        model: pickerPage.page.weekdays
                        Label { required property string modelData; text: modelData; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; horizontalAlignment: Text.AlignHCenter }
                    }
                    Repeater {
                        model: pickerPage.page.days
                        SButton {
                            required property var modelData
                            objectName: "date-day-"+modelData.iso
                            Layout.fillWidth: true; Layout.preferredWidth: 36; Layout.preferredHeight: 36
                            text: modelData.day; quiet: !modelData.today; primary: modelData.iso===field.value
                            opacity: modelData.current ? 1 : .5
                            Accessible.name: field.backend.dateText(modelData.iso)
                            onClicked: {field.error="";field.committed(modelData.iso);picker.close()}
                        }
                    }
                }
            }
        }
    }
}
