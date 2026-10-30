import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: limits
    property var backend
    property real listHeight: Theme.dp(150)
    padding: Theme.dp(10)
    background: Rectangle { color: Theme.raised; radius: Theme.dp(8) }
    contentItem: ColumnLayout {
        spacing: Theme.dp(5)
        Label { text: qsTr("Limity krajów"); color: Theme.ink; font.pointSize: Theme.body; font.bold: true }
        Label { text: qsTr("Pomocnicze limity nie blokują wyboru. Puste pole oznacza brak limitu."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Label { visible: rows.count===0; text: qsTr("Nie ma jeszcze zawodników, dla których można ustawić limit."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        ListView {
            id: rows
            objectName: "rosterCountryLimits"
            Layout.fillWidth: true; Layout.preferredHeight: Math.min(limits.listHeight, count * Theme.dp(44))
            clip: true; spacing: Theme.dp(3); model: limits.backend.limitRows
            ScrollBar.vertical: ScrollBar {}
            delegate: RowLayout {
                required property var model
                width: rows.width; height: Theme.dp(41); spacing: Theme.dp(8)
                Image { source: model.flag; visible: !!source; sourceSize.width: 40; sourceSize.height: 28; Layout.preferredWidth: Theme.dp(24); Layout.preferredHeight: Theme.dp(17); fillMode: Image.PreserveAspectFit; Accessible.ignored: true }
                Label { text: model.country; color: Theme.ink; font.bold: true; Layout.minimumWidth: Theme.dp(42); Layout.preferredWidth: Theme.dp(42) }
                Label { text: model.over ? qsTr("Przekroczono: %1 / %2").arg(model.count).arg(model.limit) : qsTr("Wybrano: %1").arg(model.count); color: model.over ? Theme.danger : Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; elide: Text.ElideRight }
                SField {
                    id: field
                    objectName: "roster-limit-" + model.country
                    Layout.preferredWidth: Theme.dp(125); Layout.minimumWidth: Theme.dp(90)
                    text: model.limit >= 0 ? String(model.limit) : ""
                    placeholderText: qsTr("Bez limitu")
                    Accessible.name: qsTr("Limit kraju %1").arg(model.country)
                    inputMethodHints: Qt.ImhDigitsOnly
                    background: Rectangle { color: Theme.surface; radius: Theme.dp(5); border.color: field.activeFocus ? Theme.accent : Theme.outline; border.width: field.activeFocus ? 2 : 1 }
                    validator: RegularExpressionValidator { regularExpression: /^(?:[0-9]{1,4}|10000)?$/ }
                    onEditingFinished: {
                        var limit = text.trim() === "" ? -1 : (acceptableInput ? Number(text) : -2)
                        if (limit === -2 || !limits.backend.setCountryLimit(model.country, limit))
                            text = model.limit >= 0 ? String(model.limit) : ""
                    }
                }
            }
        }
    }
}
