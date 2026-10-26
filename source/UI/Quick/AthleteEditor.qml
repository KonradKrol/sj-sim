import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: editor
    property var backend
    property int viewMode: 0
    spacing: Theme.dp(8)
    Flow {
        Layout.fillWidth: true; spacing: Theme.dp(5)
        Repeater {
            model: 5
            AthleteViewChoice { required property int index; objectName: "athleteView-"+index; view: index; selected: editor.viewMode===index; onClicked: editor.viewMode=index }
        }
    }
    ScrollView {
        id: scroll
        objectName: "athleteEditorScroll"
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
        contentWidth: availableWidth; clip: true
        onWidthChanged: contentItem.contentX=0
        Loader {
            width: scroll.availableWidth
            sourceComponent: [medals,patches,tokens,collector,character][editor.viewMode]
            onLoaded: scroll.contentItem.contentY=0
        }
    }
    Component { id: medals; AthleteMedals { backend: editor.backend } }
    Component { id: patches; AthletePatches { backend: editor.backend } }
    Component { id: tokens; AthleteTokens { backend: editor.backend } }
    Component { id: collector; AthleteCollector { backend: editor.backend } }
    Component { id: character; AthleteCharacter { backend: editor.backend } }
}
