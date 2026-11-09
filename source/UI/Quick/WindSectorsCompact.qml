import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: wind
    property var recorded: ({})
    property var forecastSamples: []
    property string recordedKey: ""
    property bool forecast: false
    property bool framed: true
    property real forecastWind: 0
    readonly property var beforeSamples: forecastSamples.length ? forecastSamples : recorded.before || []
    readonly property var samples: forecast ? beforeSamples : recorded.during || []
    readonly property real axisLimit: {
        var maximum=1
        var groups=[beforeSamples,recorded.during || []]
        for(var g=0;g<groups.length;++g)for(var i=0;i<groups[g].length;++i)maximum=Math.max(maximum,Math.abs(Number(groups[g][i].head || 0)))
        return Math.ceil(maximum)
    }
    signal dismissed()
    onRecordedKeyChanged: if(recordedKey.length)forecast=false
    Component.onCompleted: if(!(recorded.during || []).length)forecast=true
    implicitWidth: 0
    implicitHeight: details.implicitHeight+topPadding+bottomPadding
    padding: framed ? Theme.dp(10) : 0
    background: Rectangle { visible: wind.framed; color: Theme.surface; radius: Theme.radius; border.color: Theme.outline }
    function number(value){return Number(value).toLocaleString(Qt.locale(Theme.localeName),'f',2)}
    contentItem: Item {
    ColumnLayout {
        id: details
        width: parent.width
        spacing: Theme.dp(6)
        RowLayout {
            Layout.fillWidth: true; spacing: Theme.dp(4)
            Label { text: qsTr("Wiatr"); font.pointSize: Theme.body; font.bold: true; color: Theme.ink; Layout.fillWidth: true }
            Repeater {
                model: [qsTr("Przed"),qsTr("Po")]
                SButton {
                    required property int index
                    required property string modelData
                    readonly property bool selected: wind.forecast ? index===0 : index===1
                    text: modelData; dense: true
                    enabled: index===0 ? wind.beforeSamples.length>0 : (wind.recorded.during || []).length>0
                    foreground: selected ? Theme.snow : Theme.ink
                    background: Rectangle { color: parent.selected ? Theme.stadium : parent.hovered ? Theme.selection : Theme.raised; radius: Theme.dp(4) }
                    onClicked: wind.forecast=index===0
                }
            }
            SButton { text: "×"; dense: true; quiet: true; Accessible.name: qsTr("Zamknij podgląd wiatru"); onClicked: wind.dismissed() }
        }
        RowLayout {
            Layout.fillWidth: true
            Label { text: wind.forecast ? qsTr("Przed następnym skokiem") : qsTr("Zapis skoku"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true }
            Label { text: wind.forecast ? wind.number(wind.forecastWind)+" m/s" : wind.recorded.measuredWind!==undefined ? wind.number(wind.recorded.measuredWind)+" m/s" : ""; color: Theme.ink; font.family: Theme.mono; font.pointSize: Theme.body; font.bold: true }
        }
        RowLayout {
            Layout.fillWidth: true; spacing: Theme.dp(2)
            Repeater {
                model: wind.samples
                ColumnLayout {
                    id: sector
                    required property var modelData
                    readonly property bool available: modelData.available!==false
                    readonly property real head: Number(modelData.head || 0)
                    readonly property color tint: !available || Math.abs(head)<.005 ? Theme.muted : head>0 ? Theme.success : Theme.danger
                    Layout.fillWidth: true; Layout.preferredWidth: 0; Layout.minimumWidth: 0
                    spacing: Theme.dp(3)
                    Item {
                        Layout.fillWidth: true; Layout.preferredHeight: Theme.dp(64)
                        Rectangle { anchors.fill: parent; color: Theme.raised; radius: Theme.dp(3); opacity: .6 }
                        Rectangle { width: parent.width; height: 1; y: parent.height/2; color: Theme.outline }
                        Rectangle { width: Math.max(2,parent.width-Theme.dp(12)); anchors.horizontalCenter: parent.horizontalCenter; height: sector.available ? Math.max(1,Math.abs(sector.head)/wind.axisLimit*(parent.height/2-Theme.dp(4))) : 0; y: sector.head>=0 ? parent.height/2-height : parent.height/2; color: sector.tint; radius: Theme.dp(2) }
                        MouseArea { id: hover; anchors.fill: parent; hoverEnabled: true; acceptedButtons: Qt.NoButton }
                        ToolTip.visible: hover.containsMouse
                        ToolTip.delay: 500
                        ToolTip.text: sector.available ? qsTr("Wiatr boczny: %1 m/s").arg(wind.number(Math.abs(Number(sector.modelData.cross || 0)))) : qsTr("Poza skokiem")
                    }
                    Label { text: sector.available ? (sector.head>0 ? "+" : "")+Number(sector.head).toLocaleString(Qt.locale(Theme.localeName),'f',1) : "—"; color: sector.tint; font.bold: true; font.family: Theme.mono; font.pointSize: Theme.body; Layout.alignment: Qt.AlignHCenter }
                    Label { text: Math.round(modelData.distance)+" m"; color: Theme.muted; font.pointSize: Theme.caption; Layout.alignment: Qt.AlignHCenter }
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            Label { text: qsTr("+ pod narty"); color: Theme.success; font.pointSize: Theme.caption }
            Item { Layout.fillWidth: true }
            Label { text: qsTr("− w plecy"); color: Theme.danger; font.pointSize: Theme.caption }
        }
        WindMeasurementComparison { recorded: wind.forecast ? ({}) : wind.recorded; Layout.fillWidth: true }
    }
    }
}
