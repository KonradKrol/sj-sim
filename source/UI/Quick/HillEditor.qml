import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ScrollView {
    id: editor
    objectName: "hillEditorScroll"
    property var backend
    property bool scoring: false
    readonly property var profile: backend.profile
    contentWidth: availableWidth; clip: true
    AthleteDesign { id: conceptPalette; variant: 1 }
    HillPerkCatalog { id: perks }
    Control {
        width: editor.availableWidth
        padding: Theme.dp(10)
        background: Rectangle { color: Theme.background }
        contentItem: ColumnLayout {
            spacing: Theme.dp(10)
            RowLayout {
                Layout.fillWidth: true
                Label { text: qsTr("Skocznia"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.title; font.bold: true; Layout.fillWidth: true }
                Image { source: editor.profile.flag || ""; sourceSize.width: 70; sourceSize.height: 44; Layout.preferredWidth: Theme.dp(35); Layout.preferredHeight: Theme.dp(22); fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
            }
            GridLayout {
                Layout.fillWidth: true; columns: width>Theme.dp(440)?2:1; columnSpacing: Theme.dp(12); rowSpacing: Theme.dp(6)
                ColumnLayout {
                    Layout.fillWidth: true; Layout.minimumWidth: 0
                    Label { text: qsTr("Nazwa skoczni"); color: Theme.muted; font.pointSize: Theme.caption }
                    SField { objectName: "hillName"; Layout.fillWidth: true; text: editor.profile.name || ""; maximumLength: 100; placeholderText: qsTr("Nazwa skoczni"); Accessible.name: placeholderText; onEditingFinished: editor.backend.setValue("name",text) }
                }
                ColumnLayout {
                    Layout.fillWidth: true; Layout.minimumWidth: 0
                    Label { text: qsTr("Kraj"); color: Theme.muted; font.pointSize: Theme.caption }
                    SChoice { Layout.fillWidth: true; model: editor.backend.countries.map(function(c){return c.name}); optionImages: editor.backend.countries.map(function(c){return c.flag}); searchable: true; Accessible.name: qsTr("Kraj"); currentIndex: {var countries=editor.backend.countries;for(var i=0;i<countries.length;i++)if(countries[i].code===editor.profile.country)return i;return -1} onActivated: editor.backend.setValue("country",editor.backend.countries[index].code) }
                }
            }
            GridLayout {
                Layout.fillWidth: true; columns: width>Theme.dp(520)?3:2; columnSpacing: Theme.dp(10)
                HillNumber { backend: editor.backend; fieldKey: "k"; label: qsTr("Punkt K · m"); minimum: 1; maximum: Math.min(400,editor.profile.hs); Layout.fillWidth: true }
                HillNumber { backend: editor.backend; fieldKey: "hs"; label: "HS · m"; minimum: editor.profile.k; maximum: 500; Layout.fillWidth: true }
                HillNumber { backend: editor.backend; fieldKey: "record"; label: qsTr("Rekord skoczni · m"); maximum: 600; Layout.fillWidth: true }
            }
            RowLayout {
                Layout.fillWidth: true
                Label { text: qsTr("Perki"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true; Layout.fillWidth: true }
                HelpDot { explanation: qsTr("Jedna strona każdej pary lub brak. Wzmocnij przełącza poziomy I, II i III. Poziom III jest najsilniejszy. Kliknij wybrany perk, aby go usunąć.") }
            }
            Repeater {
                model: 3
                Control {
                    id: pair
                    required property int index
                    Layout.fillWidth: true
                    padding: Theme.dp(6)
                    background: Rectangle { color: Theme.surface; radius: Theme.dp(6); border.color: Theme.divider }
                    contentItem: ColumnLayout {
                        spacing: Theme.dp(4)
                        Label { text: perks.pairs[pair.index]; color: Theme.muted; font.family: Theme.displayFamily; font.pointSize: Theme.section }
                        GridLayout {
                            Layout.fillWidth: true; columns: width<Theme.dp(480)?1:2; columnSpacing: Theme.dp(8); rowSpacing: Theme.dp(4)
                            Repeater {
                                model: 2
                                AthleteBadge { required property int index; backend: editor.backend; design: conceptPalette; perk: perks.items[pair.index*2+index]; family: "patch"; ribbon: true; managedLevel: true; hill: true; Layout.fillWidth: true; Layout.minimumWidth: 0 }
                            }
                        }
                    }
                }
            }
            Label { text: qsTr("Próg trudnego lądowania: %1 m").arg(Number(editor.profile.realHs || 0).toLocaleString(Qt.locale(Theme.localeName),'f',1)); color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
            SButton { text: editor.scoring?qsTr("Ukryj punktację"):qsTr("Punktacja skoczni"); quiet: true; onClicked: editor.scoring=!editor.scoring }
            GridLayout {
                visible: editor.scoring; Layout.fillWidth: true; columns: width>Theme.dp(520)?3:2; columnSpacing: Theme.dp(10); rowSpacing: Theme.dp(6)
                HillNumber { backend: editor.backend; fieldKey: "pointsAtK"; label: qsTr("Punkty za punkt K"); maximum: 200; Layout.fillWidth: true }
                HillNumber { backend: editor.backend; fieldKey: "pointsPerMetre"; label: qsTr("Punkty za metr"); maximum: 20; Layout.fillWidth: true }
                HillNumber { backend: editor.backend; fieldKey: "gatePoints"; label: qsTr("Punkty za belkę"); Layout.fillWidth: true }
                HillNumber { backend: editor.backend; fieldKey: "headwindPoints"; label: qsTr("Punkty za wiatr przedni"); Layout.fillWidth: true }
                HillNumber { backend: editor.backend; fieldKey: "tailwindPoints"; label: qsTr("Punkty za wiatr tylni"); Layout.fillWidth: true }
            }
        }
    }
}
