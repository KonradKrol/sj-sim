import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: editor
    objectName: "hillEditorScroll"
    property var backend
    readonly property var profile: backend.profile
    readonly property var countryOptions: backend.countries.map(function(country){return country})
    contentWidth: availableWidth; clip: true
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
                    SChoice { Layout.fillWidth: true; model: editor.countryOptions.map(function(c){return c.name}); optionImages: editor.countryOptions.map(function(c){return c.flag}); searchable: true; Accessible.name: qsTr("Kraj"); currentIndex: {var countries=editor.countryOptions;var code=editor.profile.country;for(var i=0;i<countries.length;i++)if(countries[i].code===code)return i;return -1} onActivated: editor.backend.setValue("country",editor.countryOptions[index].code) }
                }
            }
            GridLayout {
                Layout.fillWidth: true; columns: width>Theme.dp(520)?3:2; columnSpacing: Theme.dp(10)
                HillNumber { backend: editor.backend; fieldKey: "k"; label: qsTr("Punkt K · m"); minimum: 1; maximum: Math.min(400,editor.profile.hs); Layout.fillWidth: true }
                HillNumber { backend: editor.backend; fieldKey: "hs"; label: "HS · m"; minimum: editor.profile.k; maximum: 500; Layout.fillWidth: true }
                HillNumber { backend: editor.backend; fieldKey: "record"; label: qsTr("Rekord skoczni · m"); maximum: 1000; Layout.fillWidth: true }
            }
            Label { text: qsTr("Punktacja skoczni"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true }
            GridLayout {
                objectName: "hillScoring"
                Layout.fillWidth: true; columns: width>Theme.dp(520)?3:2; columnSpacing: Theme.dp(10); rowSpacing: Theme.dp(6)
                HillNumber { backend: editor.backend; fieldKey: "pointsAtK"; label: qsTr("Punkty za punkt K"); maximum: 200; Layout.fillWidth: true }
                HillNumber { backend: editor.backend; fieldKey: "pointsPerMetre"; label: qsTr("Punkty za metr"); maximum: 20; Layout.fillWidth: true }
                HillNumber { backend: editor.backend; fieldKey: "gatePoints"; label: qsTr("Punkty za belkę"); Layout.fillWidth: true }
                HillNumber { backend: editor.backend; fieldKey: "headwindPoints"; label: qsTr("Punkty za wiatr przedni"); Layout.fillWidth: true }
                HillNumber { backend: editor.backend; fieldKey: "tailwindPoints"; label: qsTr("Punkty za wiatr tylni"); Layout.fillWidth: true }
            }
            RowLayout {
                Layout.fillWidth: true
                Label { text: qsTr("Perki"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true }
                Label { text: qsTr("opcjonalne"); color: Theme.muted; font.pointSize: Theme.caption }
                Item { Layout.fillWidth: true }
                Label { text: qsTr("Wybrane: %1").arg((editor.backend.selectedPerks || []).filter(function(v){return v!==0}).length); color: Theme.muted; font.pointSize: Theme.caption }
                HelpDot { explanation: qsTr("Wybierz jeden perk z każdej pary lub pozostaw ją pustą. Siła: lekko, wyraźnie lub skrajnie. Kliknij zaznaczoną opcję ponownie, aby usunąć perk. Opis: najedź lub naciśnij F1.") }
            }
            Repeater {
                model: perks.pairs.length
                Control {
                    id: pair
                    required property int index
                    Layout.fillWidth: true
                    padding: Theme.dp(6)
                    background: Rectangle { color: Theme.surface; radius: Theme.dp(6); border.width: 1; border.color: Theme.divider }
                    contentItem: ColumnLayout {
                        spacing: Theme.dp(5)
                        RowLayout {
                            Layout.fillWidth: true
                            Rectangle { Layout.preferredWidth: Theme.dp(3); Layout.preferredHeight: Theme.dp(13); color: Theme.accent; radius: 1 }
                            Label { text: perks.pairs[pair.index]; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.weight: Font.Bold }
                            Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: Theme.divider }
                        }
                        GridLayout {
                            Layout.fillWidth: true; columns: width<Theme.fontSize(640)?1:2; columnSpacing: Theme.dp(8); rowSpacing: Theme.dp(5)
                            Repeater {
                                model: 2
                                HillPerkChoice { required property int index; backend: editor.backend; perk: perks.items[pair.index*2+index]; Layout.fillWidth: true; Layout.minimumWidth: 0 }
                            }
                        }
                        Label {
                            objectName: "hillLandingThreshold"
                            visible: pair.index===1
                            text: qsTr("Próg trudnego lądowania: %1 m.").arg(Number(editor.backend.realHs || 0).toLocaleString(Qt.locale(Theme.localeName),'f',1))
                            color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true
                            Layout.topMargin: Theme.dp(4)
                        }
                    }
                }
            }
        }
    }
}
