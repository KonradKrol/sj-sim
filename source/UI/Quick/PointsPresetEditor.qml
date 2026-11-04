import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: editor
    property var node
    readonly property var controls: node ? node.values.controls : ({})
    spacing: Theme.dp(12)
    QtObject {
        id: adapter
        readonly property var controls: editor.controls
        readonly property int pointsSource: 0
        function value(name) {return controls[name] ? controls[name].values : ({})}
    }
    Label { text: qsTr("Punktacja za miejsca"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading }
    Label { text: qsTr("Ustal, ile punktów w klasyfikacji daje każde miejsce. Zmiany zapisują się w tej paczce na bieżąco."); color: Theme.muted; font.pointSize: Theme.caption; wrapMode: Text.WordWrap; Layout.fillWidth: true }
    ProjectedControl { node: editor.controls.lineEdit_name || null; Layout.fillWidth: true }
    ClassificationPoints { workspace: adapter; standalone: true; Layout.fillWidth: true }
}
