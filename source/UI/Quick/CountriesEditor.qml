import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// Countries: a searchable list on the left, an explained detail panel or a guided "new country"
// form on the right, and one save bar. Changes stay in a draft until "Zapisz zmiany".
Item {
    id: editor
    objectName: "countriesEditorScreen"
    readonly property var backend: ui.root.values.backend
    readonly property var current: backend ? backend.current : ({})
    readonly property bool narrow: width < Theme.dp(760)
    property bool adding: false
    property bool confirmRemove: false
    property string renameProblem: ""
    onCurrentChanged: { confirmRemove=false; renameProblem="" }

    ColumnLayout {
        anchors.fill: parent; anchors.margins: Theme.dp(16)
        spacing: Theme.dp(12)

        // Header: what this screen is for, before any interaction.
        RowLayout {
            Layout.fillWidth: true; spacing: Theme.dp(12)
            ColumnLayout {
                Layout.fillWidth: true; spacing: Theme.dp(2)
                Label { text: qsTr("Kraje"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading }
                Label {
                    text: qsTr("Nazwa kraju pojawia się przy zawodnikach, skoczniach i w wynikach. Kod (np. POL) wybiera flagę i łączy kraj z zawodnikami.")
                    color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true
                }
            }
            SButton {
                objectName: "countriesAddCountry"
                text: qsTr("Dodaj kraj"); primary: !editor.adding
                enabled: !editor.adding
                onClicked: { editor.adding=true; editor.confirmRemove=false }
            }
        }

        GridLayout {
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
            columns: editor.narrow ? 1 : 2
            columnSpacing: Theme.dp(16); rowSpacing: Theme.dp(12)

            // List of countries.
            ColumnLayout {
                Layout.preferredWidth: editor.narrow ? -1 : Theme.dp(380)
                Layout.fillWidth: editor.narrow; Layout.fillHeight: true; Layout.minimumHeight: Theme.dp(160)
                spacing: Theme.dp(6)
                SField {
                    objectName: "countriesSearch"
                    Layout.fillWidth: true
                    placeholderText: qsTr("Szukaj: nazwa lub kod, np. Norwegia albo NOR")
                    text: editor.backend ? editor.backend.search : ""
                    onTextChanged: if(editor.backend && editor.backend.search!==text) editor.backend.search=text
                    Accessible.name: qsTr("Szukaj kraju")
                }
                Label {
                    text: list.count===editor.backend.total ? qsTr("Krajów: %1").arg(editor.backend.total) : qsTr("Znaleziono %1 z %2").arg(list.count).arg(editor.backend.total)
                    color: Theme.muted; font.pointSize: Theme.caption
                }
                ListView {
                    id: list
                    objectName: "countriesList"
                    Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
                    clip: true; spacing: Theme.dp(2)
                    model: editor.backend ? editor.backend.countries : []
                    ScrollBar.vertical: ScrollBar {}
                    delegate: ItemDelegate {
                        id: row
                        objectName: "country-"+modelData.code
                        width: list.width - Theme.dp(8)
                        readonly property bool selected: modelData.selected && !editor.adding
                        padding: Theme.dp(8)
                        background: Rectangle { radius: Theme.dp(6); color: row.selected ? Theme.selection : row.hovered ? Theme.raised : "transparent"; border.color: row.selected ? Theme.accent : "transparent" }
                        onClicked: { editor.adding=false; editor.backend.select(modelData.index) }
                        Accessible.name: modelData.name+" "+modelData.code
                        contentItem: RowLayout {
                            spacing: Theme.dp(10)
                            Image { source: modelData.hasFlag ? modelData.flag : ""; sourceSize.width: 48; sourceSize.height: 32; Layout.preferredWidth: Theme.dp(30); Layout.preferredHeight: Theme.dp(20); fillMode: Image.PreserveAspectFit; cache: false; visible: modelData.hasFlag }
                            Rectangle { visible: !modelData.hasFlag; Layout.preferredWidth: Theme.dp(30); Layout.preferredHeight: Theme.dp(20); radius: 3; color: Theme.raised; border.color: Theme.outline }
                            ColumnLayout {
                                Layout.fillWidth: true; spacing: 0
                                Label { text: modelData.name; color: Theme.ink; font.pointSize: Theme.body; font.weight: row.selected ? Font.DemiBold : Font.Normal; elide: Text.ElideRight; Layout.fillWidth: true }
                                Label { text: modelData.code+" · "+modelData.usage; color: Theme.muted; font.pointSize: Theme.caption; elide: Text.ElideRight; Layout.fillWidth: true }
                            }
                        }
                    }
                    Label {
                        anchors.centerIn: parent; width: parent.width - Theme.dp(24)
                        visible: list.count===0
                        horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap
                        text: qsTr("Nie ma kraju pasującego do wyszukiwania. Możesz go dodać przyciskiem „Dodaj kraj”.")
                        color: Theme.muted; font.pointSize: Theme.body
                    }
                }
            }

            // Right panel: new country form or details of the selected one.
            Pane {
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: Theme.dp(200)
                padding: Theme.dp(18)
                background: Surface {}
                ScrollView {
                    id: panelScroll
                    anchors.fill: parent; contentWidth: availableWidth; clip: true
                    Loader {
                        width: panelScroll.availableWidth
                        sourceComponent: editor.adding ? addForm : editor.current && editor.current.code ? details : nothing
                    }
                }
            }
        }

        // Save bar: one clear place where the draft is kept or discarded.
        Rectangle {
            Layout.fillWidth: true; implicitHeight: saveRow.implicitHeight + Theme.dp(16)
            radius: Theme.dp(8)
            color: editor.backend && editor.backend.dirty ? Theme.selection : Theme.surface
            border.color: editor.backend && editor.backend.dirty ? Theme.accent : Theme.outline
            Behavior on color { enabled: Theme.motion>0; ColorAnimation { duration: Theme.motion } }
            RowLayout {
                id: saveRow
                anchors.fill: parent; anchors.margins: Theme.dp(8); anchors.leftMargin: Theme.dp(14)
                spacing: Theme.dp(10)
                Label {
                    Layout.fillWidth: true; wrapMode: Text.WordWrap
                    text: editor.backend && editor.backend.dirty ? qsTr("Masz niezapisane zmiany. Trafią do gry po kliknięciu „Zapisz zmiany”.") : qsTr("Brak zmian do zapisania.")
                    color: editor.backend && editor.backend.dirty ? Theme.ink : Theme.muted; font.pointSize: Theme.caption
                }
                SButton { objectName: "countriesCancel"; text: editor.backend && editor.backend.dirty ? qsTr("Odrzuć i zamknij") : qsTr("Zamknij"); quiet: true; onClicked: editor.backend.cancel() }
                SButton { objectName: "countriesSave"; text: qsTr("Zapisz zmiany"); primary: true; enabled: editor.backend && editor.backend.dirty; onClicked: editor.backend.save() }
            }
        }
    }

    Component { id: nothing; Label { text: qsTr("Wybierz kraj z listy."); color: Theme.muted; font.pointSize: Theme.body } }

    Component {
        id: details
        ColumnLayout {
            spacing: Theme.dp(14)
            RowLayout {
                spacing: Theme.dp(14)
                Image { visible: editor.current.hasFlag; source: editor.current.hasFlag ? editor.current.flag : ""; sourceSize.width: 160; sourceSize.height: 106; Layout.preferredWidth: Theme.dp(72); Layout.preferredHeight: Theme.dp(48); fillMode: Image.PreserveAspectFit; cache: false }
                ColumnLayout {
                    spacing: 0; Layout.fillWidth: true
                    Label { text: editor.current.name; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                    Label { text: editor.current.code; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.body }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true; spacing: Theme.dp(4)
                Label { text: qsTr("Nazwa w grze"); color: Theme.ink; font.weight: Font.DemiBold; font.pointSize: Theme.body }
                SField {
                    id: nameField
                    objectName: "countryName"
                    Layout.fillWidth: true
                    text: editor.current.name
                    Accessible.name: qsTr("Nazwa kraju")
                    onEditingFinished: editor.renameProblem=editor.backend.rename(editor.current.index,text)
                }
                Label {
                    text: editor.renameProblem.length ? editor.renameProblem : qsTr("Zmiana nazwy nie zmienia flagi ani przypisania zawodników.")
                    color: editor.renameProblem.length ? Theme.danger : Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true
                }
            }

            GridLayout {
                Layout.fillWidth: true; columns: 2; columnSpacing: Theme.dp(16); rowSpacing: Theme.dp(6)
                Label { text: qsTr("Kod kraju"); color: Theme.muted; font.pointSize: Theme.caption }
                Label { text: editor.current.code+"  —  "+qsTr("skrót przy zawodnikach; wybiera flagę. Stały, bo łączy kraj z zawodnikami."); color: Theme.ink; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                Label { text: qsTr("Kod ISO"); color: Theme.muted; font.pointSize: Theme.caption }
                Label { text: (editor.current.iso || "—")+"  —  "+qsTr("dwuliterowy kod do flag na Discordzie."); color: Theme.ink; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
                Label { text: qsTr("Używany przez"); color: Theme.muted; font.pointSize: Theme.caption }
                Label {
                    text: editor.current.athletes===0 && editor.current.hills===0 ? qsTr("nikogo – żaden zawodnik ani skocznia z bazy nie ma tego kraju.")
                        : qsTr("zawodnicy: %1, skocznie: %2").arg(editor.current.athletes).arg(editor.current.hills)
                    color: Theme.ink; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true
                }
            }

            // Flag: where it comes from and how to add or replace it.
            ColumnLayout {
                Layout.fillWidth: true; spacing: Theme.dp(6)
                Label { text: qsTr("Flaga"); color: Theme.ink; font.weight: Font.DemiBold; font.pointSize: Theme.body }
                Label {
                    text: editor.current.hasFlag
                        ? (editor.current.ownFlag ? qsTr("Używana jest Twoja flaga z pliku %1.").arg(editor.current.flagFileName)
                                                  : qsTr("Używana jest flaga wbudowana w grę (%1).").arg(editor.current.flagFileName))
                        : qsTr("Brak flagi. Gra szuka pliku %1.").arg(editor.current.flagFileName)
                    color: editor.current.hasFlag ? Theme.ink : Theme.warning; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true
                }
                FlagHelp { fileName: editor.current.flagFileName; backend: editor.backend }
            }

            Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }

            ColumnLayout {
                Layout.fillWidth: true; spacing: Theme.dp(6)
                Label {
                    text: editor.current.removable ? qsTr("Kraj nie jest używany, więc możesz go usunąć.")
                        : qsTr("Nie można usunąć tego kraju, bo mają go zawodnicy lub skocznie. Najpierw zmień im kraj w edytorze bazy.")
                    color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true
                }
                RowLayout {
                    spacing: Theme.dp(8)
                    SButton { visible: !editor.confirmRemove; objectName: "countryRemove"; text: qsTr("Usuń kraj"); quiet: true; foreground: Theme.danger; enabled: editor.current.removable; onClicked: editor.confirmRemove=true }
                    Label { visible: editor.confirmRemove; text: qsTr("Usunąć „%1”?").arg(editor.current.name); color: Theme.ink; font.pointSize: Theme.body }
                    SButton { visible: editor.confirmRemove; objectName: "countryRemoveConfirm"; text: qsTr("Tak, usuń"); foreground: Theme.danger; onClicked: { editor.confirmRemove=false; editor.backend.removeSelected() } }
                    SButton { visible: editor.confirmRemove; text: qsTr("Nie"); quiet: true; onClicked: editor.confirmRemove=false }
                }
            }
        }
    }

    Component {
        id: addForm
        ColumnLayout {
            id: form
            spacing: Theme.dp(12)
            readonly property var check: editor.backend.checkNew(nameInput.text,codeInput.text,isoInput.text)
            readonly property var problems: check.problems || ({})
            property bool touched: false

            Label { text: qsTr("Nowy kraj"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section }
            Label { text: qsTr("Uzupełnij trzy pola. Podgląd poniżej pokazuje, jak kraj będzie wyglądał w grze."); color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }

            ColumnLayout {
                Layout.fillWidth: true; spacing: Theme.dp(4)
                Label { text: qsTr("1. Nazwa"); color: Theme.ink; font.weight: Font.DemiBold; font.pointSize: Theme.body }
                SField { id: nameInput; objectName: "newCountryName"; Layout.fillWidth: true; placeholderText: qsTr("np. Polska"); onTextEdited: form.touched=true; Component.onCompleted: forceActiveFocus() }
                Label { text: form.touched && form.problems.name ? form.problems.name : qsTr("Tak kraj będzie nazywał się w listach i wynikach."); color: form.touched && form.problems.name ? Theme.danger : Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
            }
            ColumnLayout {
                Layout.fillWidth: true; spacing: Theme.dp(4)
                Label { text: qsTr("2. Kod kraju (3 litery)"); color: Theme.ink; font.weight: Font.DemiBold; font.pointSize: Theme.body }
                SField { id: codeInput; objectName: "newCountryCode"; Layout.fillWidth: true; placeholderText: qsTr("np. POL"); maximumLength: 3; font.capitalization: Font.AllUppercase; onTextEdited: form.touched=true }
                Label { text: form.touched && form.problems.code ? form.problems.code : qsTr("Skrót widoczny przy zawodnikach. Wybiera flagę i nie da się go później zmienić."); color: form.touched && form.problems.code ? Theme.danger : Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
            }
            ColumnLayout {
                Layout.fillWidth: true; spacing: Theme.dp(4)
                Label { text: qsTr("3. Kod ISO (2 litery)"); color: Theme.ink; font.weight: Font.DemiBold; font.pointSize: Theme.body }
                SField { id: isoInput; objectName: "newCountryIso"; Layout.fillWidth: true; placeholderText: qsTr("np. PL"); maximumLength: 2; font.capitalization: Font.AllUppercase; onTextEdited: form.touched=true }
                Label { text: form.touched && form.problems.iso ? form.problems.iso : qsTr("Potrzebny do flagi w wiadomościach na Discordzie."); color: form.touched && form.problems.iso ? Theme.danger : Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
            }

            // Preview of the result.
            Rectangle {
                Layout.fillWidth: true; implicitHeight: preview.implicitHeight + Theme.dp(20)
                radius: Theme.dp(8); color: Theme.raised; border.color: Theme.outline
                RowLayout {
                    id: preview
                    anchors.fill: parent; anchors.margins: Theme.dp(10); spacing: Theme.dp(10)
                    Image { visible: form.check.hasFlag; source: form.check.hasFlag ? form.check.flag : ""; sourceSize.width: 60; sourceSize.height: 40; Layout.preferredWidth: Theme.dp(36); Layout.preferredHeight: Theme.dp(24); fillMode: Image.PreserveAspectFit; cache: false }
                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 0
                        Label { text: (form.check.name || qsTr("Nazwa kraju"))+(form.check.code ? "  ·  "+form.check.code : ""); color: form.check.name ? Theme.ink : Theme.muted; font.pointSize: Theme.body; font.weight: Font.DemiBold; elide: Text.ElideRight; Layout.fillWidth: true }
                        Label {
                            text: form.check.code && form.check.code.length===3 && !form.check.hasFlag ? qsTr("Brak flagi dla tego kodu. Możesz dodać plik %1 do folderu flag (opis niżej).").arg(form.check.flagFileName) : form.check.hasFlag ? qsTr("Flaga znaleziona.") : qsTr("Flaga pojawi się po wpisaniu kodu.")
                            color: form.check.code && form.check.code.length===3 && !form.check.hasFlag ? Theme.warning : Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true
                        }
                    }
                }
            }

            RowLayout {
                spacing: Theme.dp(8)
                SButton { objectName: "newCountryAdd"; text: qsTr("Dodaj do listy"); primary: true; enabled: form.check.ready; onClicked: { if(editor.backend.add(nameInput.text,codeInput.text,isoInput.text)) editor.adding=false } }
                SButton { text: qsTr("Anuluj"); quiet: true; onClicked: editor.adding=false }
            }
            Label { text: qsTr("Nowy kraj trafi do gry po kliknięciu „Zapisz zmiany” na dole."); color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
            FlagHelp { visible: form.check.code && form.check.code.length===3 && !form.check.hasFlag; fileName: form.check.flagFileName || ""; backend: editor.backend }
        }
    }

    // How to add a flag: file name, format, folder and the button that opens it.
    component FlagHelp: Rectangle {
        id: flagHelp
        property string fileName
        property var backend
        Layout.fillWidth: true; implicitHeight: helpColumn.implicitHeight + Theme.dp(20)
        radius: Theme.dp(8); color: Theme.raised; border.color: Theme.outline
        ColumnLayout {
            id: helpColumn
            anchors.fill: parent; anchors.margins: Theme.dp(10); spacing: Theme.dp(6)
            Label {
                text: qsTr("Jak dodać lub podmienić flagę: zapisz obraz SVG albo PNG pod nazwą %1 w folderze własnych flag. Twoja flaga ma pierwszeństwo przed wbudowaną i działa we wszystkich miejscach gry, także w zapisach obrazów wyników.").arg(flagHelp.fileName)
                color: Theme.ink; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true
            }
            Label { text: qsTr("Folder własnych flag:"); color: Theme.muted; font.pointSize: Theme.caption }
            TextEdit {
                objectName: "countriesFlagsFolder"
                text: flagHelp.backend ? flagHelp.backend.flagsFolder : ""
                readOnly: true; selectByMouse: true; wrapMode: TextEdit.WrapAnywhere
                color: Theme.ink; selectionColor: Theme.selection; font.family: Theme.mono; font.pointSize: Theme.caption
                Layout.fillWidth: true
            }
            Flow {
                Layout.fillWidth: true; spacing: Theme.dp(8)
                SButton { objectName: "countriesOpenFlagsFolder"; text: qsTr("Otwórz folder flag"); dense: true; onClicked: flagHelp.backend.openFlagsFolder() }
                SButton { objectName: "countriesRefreshFlags"; text: qsTr("Sprawdź ponownie"); dense: true; quiet: true; hint: qsTr("Użyj po skopiowaniu nowej flagi do folderu."); onClicked: flagHelp.backend.refresh() }
            }
            Label { text: qsTr("W listach poza tym ekranem nowa flaga pojawi się po ponownym uruchomieniu gry."); color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        }
    }
}
