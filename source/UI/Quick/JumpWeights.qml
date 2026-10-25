import QtQuick 2.15
import QtQuick.Controls 2.15

Column {
    id: breakdown
    property var summary: ({})
    property bool expanded: false
    visible: !!summary.weighted || !!summary.resetPoints
    spacing: 2
    function number(value,decimals) {return Number(value).toLocaleString(Qt.locale(),'f',decimals)}
    function precise(value,decimals) {return number(value,decimals).replace(/0+$/, "").replace(/[.,]$/, "")}
    onSummaryChanged: if(!summary.weighted)expanded=false
    SButton {
        visible: !breakdown.summary.resetPoints
        objectName: "jumpWeightToggle"
        text: breakdown.expanded ? qsTr("Wagi ▴") : qsTr("Wagi ▾")
        quiet: true; font.pointSize: Theme.caption
        implicitHeight: Math.max(24,Theme.captionMetrics.height+8); padding: 2
        Accessible.name: qsTr("Obliczenie sumy punktów z wagami")
        checkable: true; checked: breakdown.expanded
        onClicked: breakdown.expanded=checked
    }
    Label { objectName: "jumpResetInfo"; visible: !!breakdown.summary.resetPoints; text: qsTr("Po resecie"); color: Theme.muted; font.pointSize: Theme.caption }
    ScrollView {
        id: scroll
        objectName: "jumpWeightCalculation"
        visible: breakdown.expanded && !breakdown.summary.resetPoints
        width: parent.width; height: Math.min(160,calculation.implicitHeight+8)
        contentWidth: availableWidth; clip: true
        Label {
            id: calculation
            width: scroll.availableWidth
            text: (breakdown.summary.weightTerms || []).map(function(term){return qsTr("Seria %1: %2 × %3 = %4 pkt").arg(term.round).arg(breakdown.number(term.points,1)).arg(breakdown.precise(term.weight,4)).arg(breakdown.precise(term.contribution,5))}).join("\n")+"\n"+qsTr("Suma po zaokrągleniu: %1").arg(breakdown.summary.totalPoints || "—")
            color: Theme.muted; font.pointSize: Theme.caption; font.family: Theme.mono
            wrapMode: Text.WordWrap
        }
    }
}
