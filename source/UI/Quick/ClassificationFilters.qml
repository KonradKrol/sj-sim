import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: filters
    property var workspace
    readonly property var age: workspace.value("classificationAgeFilter")
    readonly property var country: workspace.value("classificationCountrySelection")
    readonly property var context: workspace.value("classificationContextParameters")
    readonly property bool hasFilters: !!age.value || country.value>0 || !!context.value
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
                explanation: qsTr("Wiek rocznikowy to rok odniesienia minus rok urodzenia. Granice są włączne. W konkursie drużynowym cały skład musi spełnić filtr, aby drużyna zdobyła punkty za miejsce.")
                onClicked: ui.activate(filters.age.id)
            }
            GridLayout {
                visible: !!filters.age.value
                Layout.fillWidth: true; columns: filters.width<380 ? 1 : 3; columnSpacing: 10
                Repeater {
                    model: [{name:"classificationMinimumAge",title:qsTr("Wiek od")},{name:"classificationMaximumAge",title:qsTr("Wiek do")},{name:"classificationReferenceYear",title:qsTr("Rok odniesienia")}]
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
                controlName: "classificationCountryLimitToggle"
                text: qsTr("Ogranicz liczbę skoczków z kraju")
                checked: filters.country.value>0
                editable: filters.country.enabled!==false
                explanation: qsTr("W klasyfikacji indywidualnej limit wybiera skoczków z każdego kraju według sumy punktów rankingu. W drużynowej ogranicza liczbę indywidualnych wyników kraju uwzględnianych w każdym konkursie. Remis nie zwiększa limitu.")
                onClicked: ui.edit(filters.country.id,value ? 1 : 0)
            }
            GridLayout {
                visible: filters.country.value>0
                Layout.fillWidth: true; columns: filters.width<380 ? 1 : 2; columnSpacing: 12
                ColumnLayout {
                    Layout.fillWidth: true; Layout.minimumWidth: 0; spacing: 3
                    Label { text: qsTr("Uwzględniaj"); color: Theme.muted; font.pointSize: Theme.caption }
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
                text: qsTr("Osobne parametry dla konkursów")
                checked: !!filters.context.value
                editable: filters.context.enabled!==false
                explanation: qsTr("Pozwala ustawić w powiązaniach konkursu mnożnik punktów, limit punktowanych miejsc, rok i granice wieku oraz liczbę indywidualnych wyników kraju. Bez nadpisania obowiązują ustawienia klasyfikacji.")
                onClicked: ui.activate(filters.context.id)
            }
        }
    }
}
