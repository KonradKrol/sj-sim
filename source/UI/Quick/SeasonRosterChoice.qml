import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: choice
    property var backend
    property string controlName: "seasonActiveRoster"
    readonly property var entries: backend ? backend.rosterChoices : []
    readonly property int selectedIndex: {
        if (!backend) return 0
        for (var i=0; i<entries.length; ++i) if (entries[i].id === backend.activeRosterId) return i
        return 0
    }
    spacing: Theme.dp(4)
    Label { text: qsTr("Aktywna lista zawodników"); color: Theme.ink; font.pointSize: Theme.body; font.bold: true; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    SChoice {
        objectName: choice.controlName
        Layout.fillWidth: true
        model: choice.entries; textRole: "label"; currentIndex: choice.selectedIndex
        Accessible.name: qsTr("Aktywna lista zawodników")
        onActivated: if (choice.backend && index >= 0 && index < choice.entries.length) choice.backend.selectActiveRoster(choice.entries[index].id)
    }
    Label { text: qsTr("Przed konkursem skład zacznie się od tej listy. Możesz go zmienić w przygotowaniu."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
}
