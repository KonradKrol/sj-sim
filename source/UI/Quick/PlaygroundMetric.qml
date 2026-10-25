import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: metric
    property string caption
    property string value
    property string detail: ""
    padding: 12
    background: Surface { fillColor: Theme.surface; sweep: 18 }
    contentItem: ColumnLayout {
        spacing: 4
        Label { text: metric.caption; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Label { text: metric.value; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true; Layout.fillWidth: true; elide: Text.ElideRight }
        Label { visible: metric.detail.length>0; text: metric.detail; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    }
}
