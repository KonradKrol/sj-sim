import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: roster
    property var backend
    spacing: Theme.dp(8)
    SChoice {
        objectName: "formLabRosterPack"
        Layout.fillWidth: true; model: roster.backend.packs; currentIndex: roster.backend.options.pack
        enabled: !roster.backend.busy; searchable: true
        Accessible.name: qsTr("Paczka skoczków")
        onActivated: roster.backend.setOption("pack",index)
    }
    RowLayout {
        Layout.fillWidth: true
        Label { text: qsTr("Wybrano %1 / %2").arg(roster.backend.selectedAthletes).arg(roster.backend.athleteCount); color: Theme.ink; font.pointSize: Theme.body; font.bold: true; Layout.fillWidth: true }
        SButton { text: qsTr("Wszyscy"); dense: true; quiet: true; enabled: !roster.backend.busy; onClicked: roster.backend.selectAthletes(true) }
        SButton { text: qsTr("Wyczyść"); dense: true; quiet: true; enabled: !roster.backend.busy; onClicked: roster.backend.selectAthletes(false) }
    }
    SField { objectName: "formLabAthleteSearch"; Layout.fillWidth: true; placeholderText: qsTr("Nazwisko lub kraj"); onTextChanged: roster.backend.filterAthletes(text) }
    Label { text: qsTr("Zmiany dotyczą tylko próby. Paczka pozostaje bez zmian."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    ListView {
        id: list
        objectName: "formLabAthletes"
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
        clip: true; spacing: Theme.dp(5); model: roster.backend.athleteRows
        ScrollBar.vertical: ScrollBar {}
        delegate: Pane {
            id: athlete
            required property string athleteId
            required property string athleteName
            required property string country
            required property string flag
            required property bool chosen
            required property int initialForm
            required property int formPerk
            objectName: "formLabAthlete-"+athleteId
            width: ListView.view.width
            padding: Theme.dp(6)
            implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
            background: Rectangle { color: athlete.chosen ? Theme.selection : Theme.surface; radius: Theme.dp(5); border.width: 1; border.color: athlete.chosen ? Theme.accent : Theme.divider }
            contentItem: ColumnLayout {
                spacing: Theme.dp(5)
                ItemDelegate {
                    Layout.fillWidth: true; padding: 0
                    implicitHeight: Math.max(Theme.dp(22),Theme.bodyMetrics.height)
                    enabled: !roster.backend.busy
                    Accessible.name: athlete.athleteName+" · "+athlete.country; Accessible.role: Accessible.CheckBox; Accessible.checked: athlete.chosen
                    onClicked: roster.backend.setAthlete(athlete.athleteId,"selected",!athlete.chosen)
                    background: Item {}
                    contentItem: RowLayout {
                        spacing: Theme.dp(6)
                        Rectangle { implicitWidth: Theme.dp(17); implicitHeight: implicitWidth; radius: Theme.dp(3); color: athlete.chosen ? Theme.accent : Theme.surface; border.width: 1; border.color: Theme.outline; Label { anchors.centerIn: parent; text: athlete.chosen ? "✓" : ""; color: Theme.accentText; font.pointSize: Theme.caption } }
                        Image { source: athlete.flag; Layout.preferredWidth: Theme.dp(20); Layout.preferredHeight: Theme.dp(14); fillMode: Image.PreserveAspectFit; sourceSize.width: 40; sourceSize.height: 28 }
                        Label { text: athlete.athleteName; color: Theme.ink; font.pointSize: Theme.body; font.bold: athlete.chosen; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight }
                    }
                }
                RowLayout {
                    Layout.fillWidth: true; spacing: Theme.dp(5)
                    enabled: athlete.chosen && !roster.backend.busy
                    Label { text: qsTr("Forma"); color: Theme.muted; font.pointSize: Theme.caption }
                    SField {
                        objectName: "formLabInitialForm-"+athlete.athleteId
                        Layout.preferredWidth: Theme.dp(58); leftPadding: Theme.dp(4); rightPadding: Theme.dp(4); text: String(athlete.initialForm)
                        horizontalAlignment: Text.AlignHCenter; Accessible.name: qsTr("Forma początkowa: %1").arg(athlete.athleteName)
                        validator: IntValidator { bottom: 1; top: 100 }
                        onEditingFinished: {if(acceptableInput)roster.backend.setAthlete(athlete.athleteId,"form",Number(text));text=Qt.binding(function(){return String(athlete.initialForm)})}
                    }
                    SChoice {
                        objectName: "formLabPerk-"+athlete.athleteId
                        Layout.fillWidth: true; Layout.minimumWidth: 0
                        model: [qsTr("Równa IV"),qsTr("Równa III"),qsTr("Równa II"),qsTr("Równa I"),qsTr("Bez perka"),qsTr("Chwiejna I"),qsTr("Chwiejna II"),qsTr("Chwiejna III"),qsTr("Chwiejna IV")]
                        currentIndex: athlete.formPerk+4; Accessible.name: qsTr("Perk formy: %1").arg(athlete.athleteName)
                        onActivated: roster.backend.setAthlete(athlete.athleteId,"perk",index-4)
                    }
                    SToolButton { text: "↶"; Accessible.name: qsTr("Przywróć formę i perk z paczki"); hint: Accessible.name; onClicked: roster.backend.resetAthlete(athlete.athleteId) }
                }
            }
        }
        Label { visible: list.count===0; anchors.centerIn: parent; width: parent.width; horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap; text: roster.backend.athleteCount ? qsTr("Brak wyników wyszukiwania") : qsTr("Brak skoczków w paczce"); color: Theme.muted; font.pointSize: Theme.body }
    }
}
