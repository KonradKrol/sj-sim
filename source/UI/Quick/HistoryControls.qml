import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
RowLayout {
    id: controls
    property var history
    readonly property var fields: history ? history.values : ({})
    spacing: 4
    Node { showLabel: false; node: controls.fields.modeField || null; Layout.preferredWidth: 125; Layout.minimumWidth: 70; Layout.fillWidth: true }
    Node { showLabel: false; node: controls.fields.roundField || null; Layout.preferredWidth: 130; Layout.minimumWidth: 85; Layout.fillWidth: true }
    Label { visible: !!controls.fields.paused; text: qsTr("Podgląd"); color: Theme.muted; font.pointSize: Theme.caption }
}
