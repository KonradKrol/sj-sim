import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

GridLayout {
    id: identity
    property var backend
    property var design
    readonly property var profile: backend.profile
    readonly property var countryOptions: backend.countries
    columns: width>=Theme.dp(690) ? 3 : width>=Theme.dp(360) ? 2 : 1
    columnSpacing: Theme.dp(15); rowSpacing: Theme.dp(8)
    ColumnLayout {
        Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: Theme.dp(260); spacing: Theme.dp(3)
        Label { text: qsTr("Imię"); color: identity.design.muted; font.family: identity.design.detailFont; font.pointSize: Theme.caption }
        AthleteText { objectName: "athleteName"; design: identity.design; Layout.fillWidth: true; text: identity.profile.name || ""; placeholderText: qsTr("Imię"); maximumLength: 100; Accessible.name: qsTr("Imię"); onEditingFinished: identity.backend.setValue("name",text) }
    }
    ColumnLayout {
        Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: Theme.dp(260); spacing: Theme.dp(3)
        Label { text: qsTr("Nazwisko"); color: identity.design.muted; font.family: identity.design.detailFont; font.pointSize: Theme.caption }
        AthleteText { objectName: "athleteSurname"; design: identity.design; Layout.fillWidth: true; text: identity.profile.surname || ""; placeholderText: qsTr("Nazwisko"); maximumLength: 100; Accessible.name: qsTr("Nazwisko"); onEditingFinished: identity.backend.setValue("surname",text) }
    }
    ColumnLayout {
        Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: Theme.dp(260); spacing: Theme.dp(3)
        Label { text: qsTr("Kraj"); color: identity.design.muted; font.family: identity.design.detailFont; font.pointSize: Theme.caption }
        SChoice {
            id: country
            objectName: "athleteCountry"; Layout.fillWidth: true; searchable: true
            model: identity.countryOptions.map(function(c){return c.name})
            optionImages: identity.countryOptions.map(function(c){return c.flag})
            currentIndex: {for(var i=0;i<identity.countryOptions.length;i++)if(identity.countryOptions[i].code===identity.profile.country)return i;return -1}
            Accessible.name: qsTr("Kraj")
            background: Rectangle { color: identity.design.variant===1 ? "transparent" : identity.design.surface; radius: identity.design.variant===2 ? Theme.dp(18) : 0; border.width: identity.design.variant===1 ? 0 : country.activeFocus ? 2 : 1; border.color: country.activeFocus ? identity.design.accent : identity.design.line; Rectangle { visible: identity.design.variant===1; anchors.bottom: parent.bottom; width: parent.width; height: 1; color: identity.design.line } }
            contentItem: RowLayout {
                spacing: Theme.dp(7)
                Image { source: identity.profile.flag || ""; Layout.preferredWidth: Theme.dp(25); Layout.preferredHeight: Theme.dp(17); sourceSize.width: 50; sourceSize.height: 34; fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
                Label { text: country.displayText; color: identity.design.ink; font.pointSize: Theme.body; Layout.fillWidth: true; elide: Text.ElideRight }
            }
            indicator: Label { anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter; anchors.rightMargin: Theme.dp(8); text: "⌄"; color: identity.design.muted; font.pointSize: Theme.body }
            onActivated: identity.backend.setValue("country",identity.countryOptions[index].code)
        }
    }
    RowLayout {
        Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: Theme.dp(260); spacing: Theme.dp(10)
        ColumnLayout {
            Layout.fillWidth: true; spacing: Theme.dp(3)
            Label { text: qsTr("Rekord · m"); color: identity.design.muted; font.family: identity.design.detailFont; font.pointSize: Theme.caption }
            AthleteText {
                objectName: "athletePersonalBest"; design: identity.design; Layout.fillWidth: true
                text: identity.profile.personalBest>0 ? Number(identity.profile.personalBest).toLocaleString(Qt.locale(),'f',1) : ""; placeholderText: "—"
                Accessible.name: qsTr("Rekord życiowy w metrach"); validator: DoubleValidator { bottom: 0; top: 500; decimals: 1; locale: Qt.locale().name }
                onEditingFinished: if(!text.length || acceptableInput)identity.backend.setValue("personalBest",text.length?Number.fromLocaleString(Qt.locale(),text):0)
            }
        }
        ColumnLayout {
            Layout.fillWidth: true; spacing: Theme.dp(3)
            Label { text: qsTr("Rok urodzenia"); color: identity.design.muted; font.family: identity.design.detailFont; font.pointSize: Theme.caption }
            AthleteText {
                design: identity.design; Layout.fillWidth: true; text: identity.profile.birthYear>0 ? String(identity.profile.birthYear) : ""; placeholderText: "—"
                Accessible.name: qsTr("Rok urodzenia"); validator: IntValidator { bottom: 1; top: 9999 }
                inputMethodHints: Qt.ImhDigitsOnly
                onEditingFinished: if(!text.length || acceptableInput)identity.backend.setValue("birthYear",text.length?Number(text):0)
            }
        }
    }
    Flow {
        Layout.fillWidth: true; Layout.columnSpan: identity.columns===3 ? 2 : 1
        spacing: Theme.dp(8)
        AbstractButton {
            id: photo
            implicitWidth: label.implicitWidth+Theme.dp(4); implicitHeight: Math.max(Theme.dp(35),label.implicitHeight+Theme.dp(8))
            contentItem: Label { id: label; text: identity.profile.photo ? qsTr("Zmień zdjęcie") : qsTr("+ Zdjęcie"); color: identity.design.accent; font.pointSize: Theme.body; verticalAlignment: Text.AlignVCenter }
            Accessible.name: identity.profile.photo ? qsTr("Zmień zdjęcie") : qsTr("Dodaj zdjęcie")
            background: Rectangle { color: "transparent"; border.width: photo.activeFocus ? 2 : 0; border.color: identity.design.accent }
            onClicked: identity.backend.choosePhoto()
        }
        AbstractButton {
            id: remove
            visible: !!identity.profile.photo
            implicitWidth: Theme.dp(35); implicitHeight: implicitWidth
            contentItem: Label { text: "×"; font.pointSize: Theme.section; color: identity.design.muted; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
            Accessible.name: qsTr("Usuń zdjęcie")
            background: Rectangle { color: "transparent"; border.width: remove.activeFocus ? 2 : 0; border.color: identity.design.accent }
            onClicked: identity.backend.clearPhoto()
        }
    }
}
