import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: field
    property var backend
    property string fieldKey
    property string label
    property real minimum: 0
    property real maximum: 100
    property real step: 0.1
    property bool readOnly: false
    opacity: readOnly ? 0.55 : 1
    readonly property var explanations: ({
        k: qsTr("Odległość punktu konstrukcyjnego. Od niej zależą punkty za odległość."),
        hs: qsTr("Rozmiar skoczni. Wpływa na ocenę trudności dalekiego lądowania."),
        record: qsTr("Najdłuższy zapisany skok na tej skoczni; pozostaw 0, jeśli rekord nie jest znany."),
        gatePoints: qsTr("Liczba punktów rekompensaty za zmianę belki o jeden stopień."),
        headwindPoints: qsTr("Punkty odejmowane za 1 m/s wiatru pod narty, gdy włączono rekompensatę."),
        tailwindPoints: qsTr("Wartość wynosi 1,5 razy punkty za wiatr pod narty, maksymalnie 100.")
    })
    spacing: Theme.dp(3)
    RowLayout { Layout.fillWidth: true; Label { text: field.label; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap } HelpDot { visible: !!field.explanations[field.fieldKey]; explanation: field.explanations[field.fieldKey] || "" } }
    NumberField {
        objectName: "hill-"+field.fieldKey
        Layout.fillWidth: true
        direct: false; label: field.label
        enabled: !field.readOnly
        recordKey: field.backend.profile.id || ""
        fields: ({value:Number(field.backend.profile[field.fieldKey] || 0),minimum:field.minimum,maximum:field.maximum,step:field.step,decimals:field.step<1?1:0,name:"hill-"+field.fieldKey})
        onCommitted: function(value){field.backend.setValue(field.fieldKey,value)}
    }
}
