import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: profiles
    property var backend
    signal closeRequested()
    property int tab: 0
    contentWidth: availableWidth; clip: true
    ColumnLayout {
    width: profiles.availableWidth
    spacing: Theme.dp(10)
    RowLayout { Layout.fillWidth: true; Label { text: qsTr("Profile i historia balansu"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; Layout.fillWidth: true; wrapMode: Text.WordWrap } SButton { text: qsTr("Zamknij"); quiet: true; onClicked: profiles.closeRequested() } }
    Label { text: qsTr("Aktywny w grze: %1").arg(profiles.backend.activeProfile); color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    SField { id: name; Layout.fillWidth: true; text: profiles.backend.config.name; placeholderText: qsTr("Nazwa profilu"); Accessible.name: qsTr("Nazwa profilu"); onEditingFinished: profiles.backend.setContext("name",text) }
    Flow {
        Layout.fillWidth: true; spacing: 6
        SButton { text: qsTr("Zapisz profil"); enabled: !profiles.backend.busy && name.text.trim().length>0; onClicked: profiles.backend.saveProfile(name.text) }
        SButton { text: qsTr("Zastosuj bieżący balans w grze"); primary: true; enabled: !profiles.backend.busy; onClicked: profiles.backend.applyProfile() }
        SButton { text: qsTr("Standardowy balans"); quiet: true; enabled: !profiles.backend.busy; onClicked: profiles.backend.standardProfile() }
    }
    Label { text: qsTr("Zastosowanie dotyczy następnego konkursu lub sesji skoków, także w istniejącym sezonie. Rozpoczęty konkurs zachowuje swoją kopię ustawień. Dane paczki nie są zmieniane."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    Label { text: qsTr("Zmiany danych skoczków i skoczni służą eksperymentom. W grze stosowane są współczynniki silnika, pomiaru i generatora pogody."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    SectionTabs { Layout.fillWidth: true; options: [qsTr("Profile"),qsTr("Historia"),qsTr("Różnice")]; currentIndex: profiles.tab; onSelected: profiles.tab=index }
    ListView {
        Layout.fillWidth: true; Layout.preferredHeight: Theme.dp(220); clip: true
        model: profiles.tab===0 ? profiles.backend.profiles : profiles.tab===1 ? profiles.backend.history : profiles.backend.diff.changes || []
        ScrollBar.vertical: ScrollBar {}
        delegate: ItemDelegate {
            required property var modelData
            required property int index
            width: ListView.view.width
            text: profiles.tab===2 ? modelData.key+": "+modelData.before+" → "+modelData.after : modelData.name+" · "+modelData.time
            enabled: !profiles.backend.busy
            hoverEnabled: profiles.tab!==2
            onClicked: {if(profiles.tab===2)return;if(profiles.tab===0)profiles.backend.loadProfile(index);else profiles.backend.loadHistory(index);profiles.closeRequested()}
        }
    }
    Flow {
        Layout.fillWidth: true; spacing: 6
        SButton { text: qsTr("Zapisz eksperyment"); quiet: true; onClicked: profiles.backend.saveExperiment() }
        SButton { text: qsTr("Wczytaj eksperyment"); quiet: true; enabled: !profiles.backend.busy; onClicked: profiles.backend.loadExperiment() }
        SButton { text: qsTr("Eksportuj raport i CSV"); quiet: true; enabled: !!profiles.backend.fullResult.report; onClicked: profiles.backend.exportResults() }
    }
    Label { text: profiles.backend.message; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
}
}
