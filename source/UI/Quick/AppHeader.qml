import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: header
    property bool compactHeight: false
    readonly property var page: navigation ? navigation.current : null
    readonly property bool home: !navigation || !navigation.canGoBack
    implicitHeight: home ? Math.max(Theme.compact ? 32 : 42,Theme.bodyMetrics.height)+(Theme.compact ? 16 : 24) : width < 600 || compactHeight || Theme.compact ? Math.max(40,Theme.titleMetrics.height)+16 : Theme.headerHeight
    color: Theme.background
    RowLayout {
        anchors.fill: parent; anchors.margins: header.compactHeight || header.width < 600 ? 8 : 12
        spacing: 12
        SButton { objectName: "navigateBack"; visible: !header.home; text: qsTr("Wstecz"); Accessible.description: "Alt+←"; quiet: true; onClicked: navigation.back() }
        Image { visible: header.home; source: "qrc:/sj-sim.png"; sourceSize.width: 80; sourceSize.height: 80; Layout.preferredWidth: 42; Layout.preferredHeight: 42; fillMode: Image.PreserveAspectFit; Accessible.name: qsTr("Logo Sj.Sim") }
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2
            Label { visible: !header.home && !header.compactHeight && !Theme.compact && header.width >= 600; text: navigation ? navigation.path.slice(0,-1).join(" / ") : ""; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; elide: Text.ElideRight }
            Label { text: header.home ? "SJ.SIM" : header.page ? header.page.title : ""; color: Theme.ink; font.family: header.home ? Theme.bodyFamily : Theme.displayFamily; font.weight: Font.DemiBold; font.pointSize: header.home ? Theme.body : Theme.title; Layout.fillWidth: true; elide: Text.ElideRight }
        }
        SButton { visible: header.home && header.width >= 500; text: qsTr("Pomoc"); quiet: true; onClicked: ui.launch("help") }
        SButton { objectName: "appearanceAction"; text: qsTr("Wygląd"); quiet: true; onClicked: navigation.showAppearance() }
    }
}
