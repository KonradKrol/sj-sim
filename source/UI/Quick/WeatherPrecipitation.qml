import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: precipitation
    objectName: "weatherPrecipitation"
    property var backend
    readonly property var values: backend.values
    padding: Theme.dp(12)
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    background: Surface { fillColor: Theme.surface; edgeColor: Theme.divider; strokeWidth: 1; sweep: Theme.dp(20) }
    contentItem: ColumnLayout {
        spacing: Theme.dp(8)
        Label { text: qsTr("Opady"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true }
        GridLayout {
            Layout.fillWidth: true; columns: width>Theme.dp(520)?3:width>Theme.dp(320)?2:1
            columnSpacing: Theme.dp(10); rowSpacing: Theme.dp(6)
            SChoice {
                objectName: "weatherPrecipitationKind"
                Layout.fillWidth: true; Layout.minimumWidth: 0
                model: [qsTr("Śnieg"),qsTr("Deszcz")]
                currentIndex: precipitation.values.precipitationKind
                Accessible.name: qsTr("Rodzaj opadów")
                onActivated: precipitation.backend.setValue("precipitationKind",index)
            }
            SChoice {
                objectName: "weatherPrecipitationLevel"
                Layout.fillWidth: true; Layout.minimumWidth: 0
                model: [qsTr("Bez opadów"),qsTr("Słabe"),qsTr("Umiarkowane"),qsTr("Silne")]
                currentIndex: precipitation.values.precipitationLevel
                Accessible.name: qsTr("Natężenie opadów")
                onActivated: precipitation.backend.setValue("precipitationLevel",index)
            }
            SChoice {
                objectName: "weatherPrecipitationPattern"
                visible: precipitation.values.precipitationLevel>0
                Layout.fillWidth: true; Layout.minimumWidth: 0
                model: [qsTr("Stałe"),qsTr("Zmienne"),qsTr("Nasilające się")]
                currentIndex: precipitation.values.precipitationPattern
                Accessible.name: qsTr("Przebieg opadów")
                onActivated: precipitation.backend.setValue("precipitationPattern",index)
            }
        }
        Label {
            text: precipitation.values.precipitationKind===0 ? qsTr("Śnieg w torach spowalnia rozbieg.") : qsTr("Lekki deszcz nie musi hamować. Ulewa pogarsza ślizg.")
            color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true
        }
    }
}
