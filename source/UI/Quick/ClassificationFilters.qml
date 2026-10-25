import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: filters
    property var workspace
    readonly property var age: workspace.value("classificationAgeFilter")
    readonly property var country: workspace.value("classificationCountrySelection")
    readonly property bool team: workspace.value("comboBox_type").value===1
    readonly property var exclude: workspace.value("classificationCountryExclude")
    readonly property var context: workspace.value("classificationContextParameters")
    readonly property bool hasFilters: !!age.value || (team && country.value>0) || !!context.value
    onHasFiltersChanged: if(hasFilters)expanded=true
    property bool expanded: false
    function initialize() {expanded=hasFilters}
    Connections { target: filters.workspace; function onClassificationIdChanged(){filters.initialize()} }
    Component.onCompleted: initialize()
    padding: 14
    background: Surface { fillColor: Theme.surface; sweep: 22 }
    contentItem: ColumnLayout {
        spacing: 10
        SButton { objectName: "classificationAdvancedToggle"; Layout.fillWidth: true; text: qsTr("Filtry zaawansowane")+(filters.expanded ? "  −" : "  +"); quiet: true; font.family: Theme.displayFamily; font.pointSize: Theme.section; onClicked: filters.expanded=!filters.expanded }
        ColumnLayout {
            visible: filters.expanded
            Layout.fillWidth: true; spacing: 12
            CheckHelp {
                Layout.fillWidth: true
                controlName: "classificationAgeToggle"
                text: qsTr("Filtr wieku")
                checked: !!filters.age.value
                editable: filters.age.enabled!==false
                explanation: qsTr("Wiek rocznikowy jest liczony według roku rozpoczęcia sezonu i pozostaje stały do jego końca, także po przejściu na styczeń. Filtr dotyczy punktów tej klasyfikacji, a nie prawa startu. W konkursie drużynowym cały skład musi spełnić filtr, aby kraj zdobył punkty za miejsce.")
                onClicked: ui.activate(filters.age.id)
            }
            GridLayout {
                visible: !!filters.age.value
                Layout.fillWidth: true; columns: filters.width<380 ? 1 : 2; columnSpacing: 10
                Repeater {
                    model: [{name:"classificationMinimumAge",title:qsTr("Wiek od")},{name:"classificationMaximumAge",title:qsTr("Wiek do")}]
                    ColumnLayout {
                        required property var modelData
                        Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: 3
                        Label { text: parent.modelData.title; color: Theme.muted; font.pointSize: Theme.caption }
                        NumberField { fields: filters.workspace.value(parent.modelData.name); label: parent.modelData.title; Layout.fillWidth: true; Layout.minimumWidth: 0; enabled: fields.enabled!==false }
                    }
                }
            }
            SCheck { visible: !!filters.age.value; readonly property var field: filters.workspace.value("classificationUnknownAge"); text: qsTr("Uwzględniaj nieznany wiek"); checked: !!field.value; enabled: field.enabled!==false; Layout.fillWidth: true; onClicked: ui.activate(field.id) }
            Rectangle { Layout.fillWidth: true; implicitHeight: 1; color: Theme.outline }
            CheckHelp {
                Layout.fillWidth: true
                visible: filters.team
                controlName: "classificationCountryLimitToggle"
                text: qsTr("Filtr wkładu z konkursów indywidualnych")
                checked: filters.country.value>0
                editable: filters.country.enabled!==false
                explanation: qsTr("Dotyczy tylko punktów kraju z indywidualnych wyników jego skoczków. W każdym konkursie indywidualnym osobno wybieramy najlepszych lub najsłabszych z kraju: uwzględniamy tylko ich albo ich wykluczamy. W konkursach drużynowych filtr nie działa. Raz zdobyte punkty pozostają w klasyfikacji; kolejny konkurs może mieć innych punktujących. Remis nie zwiększa liczby N.")
                onClicked: ui.edit(filters.country.id,value ? 1 : 0)
            }
            Label { visible: filters.team && filters.country.value>0; text: qsTr("Nie dotyczy konkursów drużynowych."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            GridLayout {
                visible: filters.team && filters.country.value>0
                Layout.fillWidth: true; columns: filters.width<500 ? 1 : 3; columnSpacing: 12
                ColumnLayout {
                    Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: 3
                    Label { text: qsTr("Działanie"); color: Theme.muted; font.pointSize: Theme.caption }
                    SChoice { objectName: "classificationCountryAction"; Layout.fillWidth: true; Layout.minimumWidth: 0; model: [qsTr("Uwzględniaj tylko"),qsTr("Wyklucz")]; currentIndex: filters.exclude.value ? 1 : 0; enabled: filters.exclude.enabled!==false; Accessible.name: qsTr("Działanie filtra wkładu"); onActivated: if((currentIndex===1)!==!!filters.exclude.value)ui.activate(filters.exclude.id) }
                }
                ColumnLayout {
                    Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: 3
                    Label { text: qsTr("Zawodnicy"); color: Theme.muted; font.pointSize: Theme.caption }
                    SChoice { objectName: "classificationCountryDirection"; Layout.fillWidth: true; Layout.minimumWidth: 0; model: [qsTr("Najlepszych"),qsTr("Najsłabszych")]; currentIndex: Math.max(0,filters.country.value-1); enabled: filters.country.enabled!==false; Accessible.name: qsTr("Których skoczków uwzględniać"); onActivated: ui.edit(filters.country.id,currentIndex+1) }
                }
                ColumnLayout {
                    Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: 3
                    Label { text: qsTr("Skoczków na kraj"); color: Theme.muted; font.pointSize: Theme.caption }
                    NumberField { objectName: "classificationCountryLimit"; Layout.fillWidth: true; Layout.minimumWidth: 0; fields: filters.workspace.value("classificationCountryCount"); label: qsTr("Liczba skoczków na kraj"); enabled: fields.enabled!==false }
                }
            }
            Rectangle { Layout.fillWidth: true; implicitHeight: 1; color: Theme.outline }
            CheckHelp {
                Layout.fillWidth: true
                controlName: ""
                text: qsTr("Pozwól zmieniać mnożnik punktów w konkursach")
                checked: !!filters.context.value
                editable: filters.context.enabled!==false
                explanation: qsTr("W powiązaniach konkursu ustawisz osobny mnożnik punktów. Na przykład 0,5 daje połowę punktów, a 2 podwaja punkty. Bez zmiany obowiązuje mnożnik 1.")
                onClicked: ui.activate(filters.context.id)
            }
        }
    }
}
