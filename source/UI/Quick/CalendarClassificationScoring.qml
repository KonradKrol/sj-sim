import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: scoring
    property var backend
    property var entry
    spacing: 8
    SCheck {
        Layout.fillWidth: true
        text: scoring.entry.name
        tristate: true
        checkState: scoring.entry.mixed ? Qt.PartiallyChecked : scoring.entry.checked ? Qt.Checked : Qt.Unchecked
        onClicked: scoring.backend.setClassification(scoring.entry.index, checked)
    }
    ColumnLayout {
        visible: scoring.entry.checked
        Layout.fillWidth: true
        Layout.leftMargin: 12
        spacing: 6
        Label { text: qsTr("Sposób naliczania punktów"); color: Theme.muted; font.pointSize: Theme.caption }
        SChoice {
            objectName: "calendar-scoring-method-" + scoring.entry.index
            Layout.fillWidth: true
            model: scoring.entry.methodNames
            currentIndex: scoring.entry.methodIndex
            displayText: currentIndex < 0 ? qsTr("Różne sposoby") : currentText
            Accessible.name: qsTr("Sposób naliczania punktów: %1").arg(scoring.entry.name)
            onActivated: if (currentIndex >= 0) scoring.backend.setClassificationScoring(scoring.entry.index, {method: scoring.entry.methodIds[currentIndex]})
        }
        Label {
            visible: scoring.entry.contextAllowed
            Layout.fillWidth: true
            text: qsTr("Zaawansowane parametry konkursu — wyłączone pola dziedziczą ustawienia klasyfikacji.")
            color: Theme.muted
            font.pointSize: Theme.caption
            wrapMode: Text.WordWrap
        }
        Repeater {
            model: scoring.entry.contextAllowed ? [
                {key:"multiplier",title:qsTr("Mnożnik punktów"),minimum:0,maximum:1000000,step:0.1,decimals:4},
                {key:"points-limit",title:qsTr("Limit punktowanych miejsc (0 = bez limitu)"),minimum:0,maximum:10000,step:1,decimals:0},
                {key:"reference-year",title:qsTr("Rok odniesienia dla wieku"),minimum:1,maximum:9999,step:1,decimals:0},
                {key:"minimum-age",title:qsTr("Minimalny wiek"),minimum:0,maximum:9998,step:1,decimals:0},
                {key:"maximum-age",title:qsTr("Maksymalny wiek"),minimum:0,maximum:9998,step:1,decimals:0},
                {key:"country-count",title:qsTr("N wyników indywidualnych kraju"),minimum:1,maximum:10000,step:1,decimals:0}
            ] : []
            ColumnLayout {
                id: parameter
                required property var modelData
                readonly property bool overridden: scoring.entry.parameters[modelData.key] !== undefined
                readonly property bool mixed: !!scoring.entry.mixedParameters[modelData.key]
                Layout.fillWidth: true
                spacing: 4
                SCheck {
                    objectName: "calendar-parameter-" + parameter.modelData.key + "-" + scoring.entry.index
                    Layout.fillWidth: true
                    text: parameter.modelData.title
                    tristate: true
                    checkState: parameter.mixed ? Qt.PartiallyChecked : parameter.overridden ? Qt.Checked : Qt.Unchecked
                    onClicked: {
                        var patch = {}
                        patch[parameter.modelData.key] = checked ? scoring.entry.defaults[parameter.modelData.key] : null
                        scoring.backend.setClassificationScoring(scoring.entry.index, patch)
                    }
                }
                NumberField {
                    visible: parameter.overridden || parameter.mixed
                    Layout.fillWidth: true
                    direct: false
                    label: parameter.modelData.title
                    fields: ({name:"classification-parameter-"+parameter.modelData.key+"-"+scoring.entry.index,
                        value:parameter.overridden ? scoring.entry.parameters[parameter.modelData.key] : scoring.entry.defaults[parameter.modelData.key],
                        mixed:parameter.mixed, minimum:parameter.modelData.minimum, maximum:parameter.modelData.maximum,
                        step:parameter.modelData.step, decimals:parameter.modelData.decimals})
                    onCommitted: {
                        var patch = {}
                        patch[parameter.modelData.key] = value
                        scoring.backend.setClassificationScoring(scoring.entry.index, patch)
                    }
                }
            }
        }
    }
}
