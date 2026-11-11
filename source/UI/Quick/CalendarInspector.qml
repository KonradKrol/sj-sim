import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Pane {
    id: inspector
    ViewArrival { target: inspector; expressive: false }
    objectName: "calendarInspector"
    property var backend
    property bool compact: false
    property bool planningOpen: false
    signal planningRequested()
    signal editRequested()
    property var rulesNode
    property int section: 0
    implicitWidth: 0
    property real planningHeaderHeight: Theme.dp(90)
    implicitHeight: planningOpen ? planningHeaderHeight : 520
    Timer { id: measureHeader; interval: 0; onTriggered: inspector.planningHeaderHeight=heading.implicitHeight+inspector.topPadding+inspector.bottomPadding }
    Connections { target: heading; function onImplicitHeightChanged(){measureHeader.restart()} }
    onPlanningOpenChanged: measureHeader.restart()
    Component.onCompleted: measureHeader.start()
    padding: compact ? 8 : 16
    background: Surface { fillColor: Theme.surface; sweep: 26 }
    contentItem: Item {
        ColumnLayout {
            id: heading
            objectName: "calendarInspectorHeading"
            anchors.top: parent.top; anchors.left: parent.left; anchors.right: parent.right
            spacing: inspector.compact ? 8 : 12
            RowLayout {
                Layout.fillWidth: true; spacing: 8
                Image { objectName: "calendarInspectorFlag"; visible: !inspector.planningOpen && !!inspector.backend.selection.flag; source: inspector.backend.selection.flag || ""; sourceSize.width: 48; sourceSize.height: 32; Layout.preferredWidth: 28; Layout.preferredHeight: 20; fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
                Label { text: inspector.planningOpen ? qsTr("Cały kalendarz") : inspector.backend.selection.title || qsTr("Wybierz konkurs"); color: Theme.ink; font.family: inspector.compact ? Theme.bodyFamily : Theme.displayFamily; font.pointSize: inspector.compact ? Theme.body : Theme.heading; Layout.fillWidth: true; Layout.minimumWidth: 0; wrapMode: inspector.compact ? Text.NoWrap : Text.WordWrap; elide: Text.ElideRight }
            }
            Label { visible: !inspector.planningOpen && inspector.backend.selection.count > 1; text: qsTr("Zmiany dotyczą %1 konkursów").arg(inspector.backend.selection.count); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            Label { visible: !inspector.planningOpen && inspector.backend.selection.count > 0 && inspector.backend.selection.locked; text: inspector.backend.selection.rulesEditable ? qsTr("Rozegrane przygotowania są zablokowane.") : qsTr("Rozegrane wydarzenia są zablokowane."); color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            Label { visible: !inspector.planningOpen && inspector.backend.selection.pendingSource>0; text: qsTr("Zachowano kwalifikację z konkursu %1. Sprawdź kolejność i rodzaj konkursów, aby przywrócić powiązanie.").arg(inspector.backend.selection.pendingSource); color: Theme.warning; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            RowLayout {
                Layout.fillWidth: true; spacing: Theme.dp(10)
                SectionTabs {
                    objectName: "calendarEventTabs"
                    enabled: inspector.backend.selection.count>0
                    dense: true; options: [qsTr("Konkurs"),qsTr("Zasady"),qsTr("Powiązania")]
                    currentIndex: inspector.planningOpen ? -1 : inspector.section
                    Layout.fillWidth: true; Layout.minimumWidth: 0
                    onSelected: {inspector.section=index;inspector.editRequested()}
                }
                Rectangle { Layout.preferredWidth: 1; Layout.preferredHeight: Theme.dp(22); color: Theme.divider; Accessible.ignored: true }
                SButton {
                    objectName: "calendarPlanDates"
                    text: qsTr("Szybkie planowanie"); dense: true; checkable: true; checked: inspector.planningOpen
                    foreground: checked ? Theme.accent : Theme.ink
                    onClicked: if(inspector.planningOpen)inspector.editRequested();else inspector.planningRequested()
                }
            }
        }
        ScrollView {
            id: fieldsScroll
            visible: !inspector.planningOpen
            objectName: "calendarInspectorFields"
            anchors.top: heading.bottom; anchors.bottom: parent.bottom
            anchors.left: parent.left; anchors.right: parent.right
            anchors.topMargin: inspector.planningOpen ? 0 : inspector.compact ? 8 : 12
            contentWidth: availableWidth; clip: true
            ColumnLayout {
                width: fieldsScroll.availableWidth
                enabled: inspector.backend.selection.count > 0 && (!inspector.backend.selection.locked || inspector.section>0 && inspector.backend.selection.rulesEditable)
                CalendarFields { visible: inspector.section === 0; backend: inspector.backend; Layout.fillWidth: true; onEditRules: {inspector.backend.rulesTarget=target;inspector.section=1;inspector.editRequested()} }
                ColumnLayout {
                    visible: inspector.section === 1
                    Layout.fillWidth: true; spacing: 12
                    SChoice { Layout.fillWidth: true; model: [qsTr("Konkurs"),qsTr("Treningi"),qsTr("Seria próbna")]; currentIndex: inspector.backend.rulesTarget; Accessible.name: qsTr("Zasady wydarzenia"); onActivated: inspector.backend.rulesTarget=currentIndex }
                    Loader {
                        Layout.fillWidth: true; Layout.preferredHeight: item ? item.implicitHeight : 0
                        enabled: !inspector.backend.selection.locked || inspector.backend.rulesTarget===0
                        active: inspector.section === 1
                        source: Qt.resolvedUrl("Node.qml")
                        onLoaded: item.node = Qt.binding(function(){return inspector.rulesNode})
                    }
                }
                ColumnLayout {
                    visible: inspector.section === 2
                    Layout.fillWidth: true; spacing: 12
                    RowLayout { Layout.fillWidth: true; Label { text: qsTr("Klasyfikacje"); color: Theme.ink; font.pointSize: Theme.body; font.weight: Font.DemiBold; Layout.fillWidth: true } HelpDot { explanation: qsTr("Zaznacz klasyfikacje, do których te zawody mają dodawać punkty.") } }
                    Label { visible: inspector.backend.classifications.length === 0; text: qsTr("Brak klasyfikacji w kalendarzu"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                    Repeater {
                        model: inspector.backend.classifications
                        CalendarClassificationScoring { enabled: !inspector.backend.selection.locked; Layout.fillWidth: true; backend: inspector.backend; entry: modelData }
                    }
                    RowLayout { Layout.fillWidth: true; Label { text: qsTr("Kwalifikacja na podstawie konkursu"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap } HelpDot { explanation: qsTr("Uczestników wyłoni wskazany wcześniejszy konkurs lub kwalifikacje.") } }
                    RowLayout {
                        Layout.fillWidth: true; spacing: Theme.dp(5)
                        SChoice { Layout.minimumWidth: 0; Layout.fillWidth: true; model: inspector.backend.advancementCompetitions; optionImages: inspector.backend.selection.qualificationFlags || []; searchable: true; popup.width: Math.min(Theme.dp(480), inspector.Window.window ? inspector.Window.window.width-24 : width); currentIndex: inspector.backend.selection.advancement === undefined ? -1 : inspector.backend.selection.advancement; Accessible.name: qsTr("Kwalifikacja na podstawie konkursu"); onActivated: inspector.backend.update({advancement:currentIndex}) }
                        SToolButton {
                            text: "×"; Accessible.name: qsTr("Usuń źródło kwalifikacji")
                            hint: qsTr("Ustaw brak źródła kwalifikacji")
                            enabled: inspector.backend.selection.advancement !== 0 || inspector.backend.selection.pendingSource>0
                            onClicked: inspector.backend.update({advancement:0})
                        }
                    }
                    RowLayout { Layout.fillWidth: true; Label { text: qsTr("Kwalifikacja według klasyfikacji"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap } HelpDot { explanation: qsTr("Uczestników wyłoni aktualna kolejność we wskazanej klasyfikacji.") } }
                    RowLayout {
                        Layout.fillWidth: true; spacing: Theme.dp(5)
                        SChoice { Layout.minimumWidth: 0; Layout.fillWidth: true; model: inspector.backend.advancementClassifications; currentIndex: inspector.backend.selection.advancementClass === undefined ? -1 : inspector.backend.selection.advancementClass; Accessible.name: qsTr("Kwalifikacja według klasyfikacji"); onActivated: inspector.backend.update({advancementClass:currentIndex}) }
                        SToolButton {
                            text: "×"; Accessible.name: qsTr("Usuń źródło kwalifikacji")
                            hint: qsTr("Ustaw brak źródła kwalifikacji")
                            enabled: inspector.backend.selection.advancementClass !== 0
                            onClicked: inspector.backend.update({advancementClass:0})
                        }
                    }
                    Rectangle { Layout.fillWidth: true; implicitHeight: 1; color: Theme.outline; Layout.topMargin: Theme.dp(8); Layout.bottomMargin: Theme.dp(8) }
                    RowLayout {
                        Layout.fillWidth: true
                        Label { text: qsTr("Sortowanie listy startowej"); color: Theme.ink; font.weight: Font.DemiBold; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                        HelpDot { explanation: qsTr("Ustawia początkową kolejność od najsłabszego do najlepszego. Domyślnie korzysta z pierwszej pasującej klasyfikacji. Przed startem możesz ręcznie zmienić kolejność.") }
                    }
                    SChoice {
                        Layout.fillWidth: true; Layout.minimumWidth: 0
                        popup.width: Math.min(Theme.dp(420), inspector.Window.window ? inspector.Window.window.width-24 : width)
                        model: [qsTr("Domyślnie — pierwsza pasująca klasyfikacja"),qsTr("Według wybranego źródła kwalifikacji"),qsTr("Konkretny konkurs"),qsTr("Konkretna klasyfikacja")]
                        optionEnabled: [true, !!inspector.backend.selection.hasQualificationSource, (inspector.backend.selection.sortCompetitionIds || []).length>0, true]
                        currentIndex: inspector.backend.selection.sortMode === undefined ? 0 : inspector.backend.selection.sortMode
                        onActivated: inspector.backend.update({sortMode:currentIndex})
                    }
                    SChoice {
                        visible: inspector.backend.selection.sortMode === 2
                        Layout.fillWidth: true; Layout.minimumWidth: 0
                        searchable: true
                        popup.width: Math.min(Theme.dp(480), inspector.Window.window ? inspector.Window.window.width-24 : width)
                        popup.height: Math.min(Theme.dp(360), Math.max(popup.above,popup.below))
                        displayText: currentIndex < 0 ? qsTr("Różne konkursy") : textAt(currentIndex)
                        optionImages: inspector.backend.selection.sortCompetitionFlags || []
                        model: inspector.backend.selection.sortCompetitionNames || []
                        currentIndex: (inspector.backend.selection.sortCompetitionIds || []).indexOf(inspector.backend.selection.sortCompetition)
                        onActivated: inspector.backend.update({sortCompetition:inspector.backend.selection.sortCompetitionIds[currentIndex]})
                    }
                    SChoice {
                        visible: inspector.backend.selection.sortMode === 3
                        Layout.fillWidth: true; Layout.minimumWidth: 0
                        searchable: true
                        displayText: currentIndex < 0 ? qsTr("Wybierz klasyfikację") : textAt(currentIndex)
                        model: inspector.backend.selection.sortClassNames || []
                        currentIndex: (inspector.backend.selection.sortClassIds || []).indexOf(inspector.backend.selection.sortClassification)
                        onActivated: inspector.backend.update({sortClassification:inspector.backend.selection.sortClassIds[currentIndex]})
                    }
                    Label {
                        Layout.fillWidth: true
                        text: qsTr("Od najsłabszego do najlepszego. Kolejność możesz zmienić przed startem.")
                        color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap
                    }
                }
            }
        }
    }
}
