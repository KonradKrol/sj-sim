import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: identity
    property var backend
    readonly property var profile: backend.profile
    readonly property var countryOptions: backend.countries
    spacing: Theme.dp(10)
    Label { text: qsTr("Dane skoczka"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section }
    GridLayout {
        columns: identity.width>Theme.dp(390) ? 2 : 1
        columnSpacing: Theme.dp(12); rowSpacing: Theme.dp(9); Layout.fillWidth: true
        ColumnLayout {
            Layout.fillWidth: true; spacing: 3
            Label { text: qsTr("Imię"); color: Theme.muted; font.pointSize: Theme.caption }
            SField { objectName: "athleteName"; Layout.fillWidth: true; text: identity.profile.name || ""; maximumLength: 100; Accessible.name: qsTr("Imię"); onEditingFinished: identity.backend.setValue("name",text) }
        }
        ColumnLayout {
            Layout.fillWidth: true; spacing: 3
            Label { text: qsTr("Nazwisko"); color: Theme.muted; font.pointSize: Theme.caption }
            SField { objectName: "athleteSurname"; Layout.fillWidth: true; text: identity.profile.surname || ""; maximumLength: 100; Accessible.name: qsTr("Nazwisko"); onEditingFinished: identity.backend.setValue("surname",text) }
        }
        ColumnLayout {
            Layout.fillWidth: true; spacing: 3
            Label { text: qsTr("Kraj"); color: Theme.muted; font.pointSize: Theme.caption }
            SChoice {
                objectName: "athleteCountry"; Layout.fillWidth: true; searchable: true
                model: identity.countryOptions.map(function(c){return c.name})
                optionImages: identity.countryOptions.map(function(c){return c.flag})
                currentIndex: {for(var i=0;i<identity.countryOptions.length;i++)if(identity.countryOptions[i].code===identity.profile.country)return i;return -1}
                Accessible.name: qsTr("Kraj")
                onActivated: identity.backend.setValue("country",identity.countryOptions[index].code)
            }
        }
        ColumnLayout {
            Layout.fillWidth: true; spacing: 3
            Label { text: qsTr("Rok urodzenia · opcjonalnie"); color: Theme.muted; font.pointSize: Theme.caption }
            SField {
                Layout.fillWidth: true; text: identity.profile.birthYear>0 ? String(identity.profile.birthYear) : ""; placeholderText: "—"
                Accessible.name: qsTr("Rok urodzenia"); validator: IntValidator { bottom: 1; top: 9999 } inputMethodHints: Qt.ImhDigitsOnly
                onEditingFinished: if(text.length===0 || acceptableInput)identity.backend.setValue("birthYear",text.length?Number(text):0)
            }
        }
        ColumnLayout {
            Layout.fillWidth: true; spacing: 3
            Label { text: qsTr("Rekord życiowy · m"); color: Theme.muted; font.pointSize: Theme.caption }
            SField {
                objectName: "athletePersonalBest"; Layout.fillWidth: true
                text: identity.profile.personalBest>0 ? Number(identity.profile.personalBest).toLocaleString(Qt.locale(),'f',1) : ""; placeholderText: qsTr("Brak")
                Accessible.name: qsTr("Rekord życiowy w metrach"); validator: DoubleValidator { bottom: 0; top: 500; decimals: 1; locale: Qt.locale().name }
                onEditingFinished: if(!text.length || acceptableInput)identity.backend.setValue("personalBest",text.length?Number.fromLocaleString(Qt.locale(),text):0)
            }
        }
        Flow {
            Layout.fillWidth: true; spacing: 3
            SButton { text: identity.profile.photo ? qsTr("Zmień zdjęcie") : qsTr("Dodaj zdjęcie"); quiet: true; onClicked: identity.backend.choosePhoto() }
            SButton { visible: !!identity.profile.photo; text: qsTr("Usuń zdjęcie"); quiet: true; onClicked: identity.backend.clearPhoto() }
        }
    }
}
