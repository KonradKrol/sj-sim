import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

RowLayout {
    id: badge
    objectName: "releaseBadge"
    spacing: 8
    Accessible.role: Accessible.StaticText
    Accessible.name: "Sj.Sim " + ui.version
    Label { text: "Sj.Sim"; color: Theme.muted; font.pointSize: Theme.caption; font.family: Theme.bodyFamily; font.weight: Font.DemiBold }
    Label { text: ui.version; color: Theme.ink; font.pointSize: Theme.caption; font.family: Theme.mono }
}
