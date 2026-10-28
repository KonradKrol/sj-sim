import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: workspace
    objectName: "jumpManipulationWorkspace"
    property var node
    readonly property var fields: node.values
    readonly property var backend: fields.backend
    readonly property var values: backend.values
    property int section: 0
    spacing: Theme.dp(8)
    Control {
        Layout.fillWidth: true
        padding: Theme.dp(12); implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
        background: Surface { fillColor: Theme.stadium; sweep: Theme.dp(24) }
        contentItem: ColumnLayout {
            spacing: Theme.dp(5)
            RowLayout {
                Layout.fillWidth: true
                Image { source: workspace.fields.flag || ""; sourceSize.width: 48; sourceSize.height: 32; Layout.preferredWidth: Theme.dp(24); Layout.preferredHeight: Theme.dp(18); fillMode: Image.PreserveAspectFit }
                Label { text: workspace.fields.athlete || qsTr("Następny skok"); color: Theme.snow; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                SButton { text: qsTr("Wyczyść"); inverse: true; quiet: true; enabled: workspace.backend.changesCount>0; onClicked: {workspace.forceActiveFocus();workspace.backend.reset()} }
            }
            Label { text: qsTr("Zmiany dotyczą wyłącznie następnego skoku."); color: Theme.headerMuted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        }
    }
    SectionTabs {
        Layout.fillWidth: true
        options: [qsTr("Lot"),qsTr("Lądowanie i noty"),qsTr("Warunki")]; currentIndex: workspace.section
        onSelected: {workspace.forceActiveFocus();workspace.section=index}
    }
    ScrollView {
        id: scroll
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0; Layout.minimumWidth: 0
        contentWidth: availableWidth; clip: true
        Column {
            width: scroll.availableWidth; spacing: Theme.dp(10)
            JumpAdjustmentFields { visible: workspace.section===0; width: parent.width; backend: workspace.backend; fieldKey: "quality"; title: qsTr("Jakość skoku"); unit: qsTr("pkt jakości"); upper: 1000; help: qsTr("Koryguje jakość tego skoku na skali OVR i formy. Nie zmienia parametrów zawodnika.") }
            JumpAdjustmentFields { visible: workspace.section===0; width: parent.width; backend: workspace.backend; fieldKey: "distance"; title: qsTr("Odległość"); unit: "m"; upper: 1000; help: qsTr("Korekta jest dodawana przed ograniczeniem zakresu. Jednakowe minimum i maksimum oznaczają dokładną odległość. Granice skoczni nadal obowiązują.") }
            ColumnLayout {
                visible: workspace.section===1; width: parent.width; spacing: Theme.dp(12)
                RowLayout {
                    Layout.fillWidth: true
                    Label { text: qsTr("Lądowanie"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.bold: true; Layout.fillWidth: true }
                    SChoice {
                        objectName: "manipulatedLanding"
                        Layout.preferredWidth: Theme.dp(180); Layout.minimumWidth: 0
                        model: [qsTr("Losuj"),qsTr("Telemark"),qsTr("Na dwie nogi"),qsTr("Podparty"),qsTr("Upadek")]
                        currentIndex: Number(workspace.values.landing)+1
                        onActivated: function(index){workspace.forceActiveFocus();workspace.backend.setValue("landing",index-1)}
                    }
                }
                JumpJudgesEditor { Layout.fillWidth: true; backend: workspace.backend; availableWidth: scroll.availableWidth }
                RowLayout {
                    visible: !!workspace.values.dsqAllowed; Layout.fillWidth: true
                    SCheck { text: qsTr("Ustaw ryzyko DSQ"); checked: Number(workspace.values.dsq)>=0; Layout.fillWidth: true; onClicked: {var selected=checked;workspace.forceActiveFocus();workspace.backend.setValue("dsq",selected?1:-1)} }
                    NumberField {
                        Layout.preferredWidth: Theme.dp(150); Layout.minimumWidth: Theme.dp(110)
                        direct: false; flexibleDecimal: true; enabled: Number(workspace.values.dsq)>=0; label: qsTr("Ryzyko dyskwalifikacji · %")
                        recordKey: "dsq:"+workspace.values.revision
                        fields: ({value:Math.max(0,Number(workspace.values.dsq)),minimum:0,maximum:100,step:1,decimals:2,name:"dsqPercent"})
                        onCommitted: function(value){workspace.backend.setValue("dsq",value)}
                    }
                }
                Label { visible: !workspace.values.dsqAllowed; text: qsTr("DSQ wyłączone w zasadach konkursu"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            }
            ColumnLayout {
                visible: workspace.section===2; width: parent.width; spacing: Theme.dp(10)
                SCheck { objectName: "manipulatedWind"; text: qsTr("Ustaw wiatr dla tego skoku"); checked: !!workspace.values.windActive; Layout.fillWidth: true; onClicked: {var selected=checked;workspace.forceActiveFocus();workspace.backend.setValue("windActive",selected)} }
                Loader {
                    active: workspace.section===2 && !!workspace.values.windActive; visible: active; Layout.fillWidth: true
                    Layout.preferredHeight: status===Loader.Ready && item ? item.implicitHeight : 0
                    sourceComponent: Component { WindSlopePreview { heading: qsTr("Wiatr tego skoku"); backend: workspace.backend.wind; samples: workspace.backend.wind.preview } }
                }
                JumpAdjustmentFields { Layout.fillWidth: true; backend: workspace.backend; fieldKey: "precipitation"; title: qsTr("Natężenie opadów"); unit: qsTr("poziom 0–10"); upper: 10; help: qsTr("Zmienia natężenie opadów tylko podczas tego skoku. Rodzaj opadów pozostaje zgodny z warunkami konkursu.") }
                JumpAdjustmentFields { Layout.fillWidth: true; backend: workspace.backend; fieldKey: "measuredWind"; title: qsTr("Pomiar do punktacji"); unit: "m/s"; lower: -100; upper: 100; help: qsTr("Zmienia pomiar do rekompensaty, nie fizyczny wiatr. Kierunek dodatni: pod narty; ujemny: w plecy.") }
            }
        }
    }
    Label { visible: workspace.backend.message.length>0; text: workspace.backend.message; color: Theme.danger; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    RowLayout {
        Layout.fillWidth: true
        Label { text: workspace.backend.changesCount>0 ? qsTr("Zmiany: %1").arg(workspace.backend.changesCount) : qsTr("Brak zmian"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        SButton { text: qsTr("Zastosuj"); primary: true; enabled: workspace.backend.canApply; onClicked: {workspace.forceActiveFocus();if(workspace.backend.canApply)ui.activate(workspace.fields.submitId)} }
    }
}
