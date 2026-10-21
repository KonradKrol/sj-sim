import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: library
    property var backend
    readonly property bool cramped: height<280
    spacing: cramped ? 4 : 8
    Label { visible: !library.cramped; text: qsTr("Zawodnicy"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section }
    GridLayout {
        columns: library.width >= 400 || library.cramped ? 2 : 1
        Layout.fillWidth: true; columnSpacing: 8; rowSpacing: 8
        SField { objectName: "rosterSearch"; Layout.fillWidth: true; Layout.minimumWidth: 0; text: library.backend.query; placeholderText: qsTr("Szukaj nazwiska lub kraju"); Accessible.name: placeholderText; onTextEdited: library.backend.query=text }
        SChoice { objectName: "rosterCountry"; Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.preferredWidth: 150; model: library.backend.countries; currentIndex: library.backend.country; Accessible.name: qsTr("Kraj"); onActivated: library.backend.country=currentIndex }
    }
    ListView {
        id: candidates
        objectName: "rosterLibrary"
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
        model: library.backend.library
        spacing: 4; clip: true; activeFocusOnTab: true
        ScrollBar.vertical: ScrollBar {}
        delegate: RosterRow { width: candidates.width; backend: library.backend; athleteId: model.athleteId; athleteName: model.athleteName; countryCode: model.countryCode; included: model.included }
        Label { visible: candidates.count === 0; anchors.centerIn: parent; width: parent.width; text: qsTr("Brak pasujących zawodników"); wrapMode: Text.WordWrap; color: Theme.muted; font.pointSize: Theme.body }
    }
    RowLayout {
        Layout.fillWidth: true
        SButton { objectName: "rosterAddVisible"; text: qsTr("Dodaj widocznych"); enabled: library.backend.library.count > 0; Layout.fillWidth: true; onClicked: library.backend.addVisible() }
        SButton {
            text: "⋯"; quiet: true; Accessible.name: qsTr("Wybór zawodników"); onClicked: options.open()
            ActionMenu { id: options; entries: [{id:0,text:qsTr("Usuń widocznych z listy")}]; onSelected: library.backend.removeVisible() }
        }
    }
}
