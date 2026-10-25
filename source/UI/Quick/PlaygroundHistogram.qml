import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: chart
    property var series: []
    property bool places: false
    property int unclassified: 0
    property string caption: ""
    readonly property var palette: [Theme.accent,Theme.signal,"#ca7529","#a75ab0","#328f9e","#b14461"]
    readonly property var plotted: {
        var keys={},lo=Infinity,hi=-Infinity
        for(var s=0;s<series.length;s++)for(var j=0;j<series[s].bins.length;j++){var b=series[s].bins[j];var key=places?b.place:b.from;keys[key]=true;lo=Math.min(lo,key);hi=Math.max(hi,places?key:b.to)}
        if(!isFinite(lo))return {bins:[],max:1,lo:0,hi:0}
        var step=places?1:2
        if(series.length && series[0].bins.length && !places)step=series[0].bins[0].to-series[0].bins[0].from
        var span=Math.max(step,Math.ceil((hi-lo+step)/60/step)*step),bins=[],max=0
        for(var x=lo;x<=hi;x+=span){var values=[];for(s=0;s<series.length;s++){var n=0,total=0;for(j=0;j<series[s].bins.length;j++){b=series[s].bins[j];total+=b.count;key=places?b.place:b.from;if(key>=x && key<x+span)n+=b.count}var share=total?n/total:0;values.push({name:series[s].name,n:n,share:share});max=Math.max(max,share)}bins.push({from:x,to:Math.min(hi,x+span-(places?1:0)),values:values})}
        return {bins:bins,max:max||1,lo:lo,hi:hi}
    }
    spacing: 6
    Label { text: chart.caption; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    Flow {
        Layout.fillWidth: true; spacing: 12
        Repeater { model: chart.series; Row { required property var modelData; required property int index; spacing: 5; Rectangle { width: 10; height: 10; anchors.verticalCenter: parent.verticalCenter; color: chart.palette[parent.index%chart.palette.length]; radius: 2 } Label { text: parent.modelData.name; color: Theme.muted; font.pointSize: Theme.caption } } }
    }
    Item {
        Layout.fillWidth: true; Layout.preferredHeight: 190; Layout.minimumHeight: 160
        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: Theme.outline }
        Label { anchors.left: parent.left; anchors.top: parent.top; text: (chart.plotted.max*100).toLocaleString(Qt.locale(),'f',1)+" %"; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption }
        Row {
            anchors.fill: parent; anchors.topMargin: 22; spacing: 1
            Repeater {
                model: chart.plotted.bins
                Item {
                    id: bucket
                    required property var modelData
                    width: Math.max(1,(parent.width-Math.max(0,chart.plotted.bins.length-1))/Math.max(1,chart.plotted.bins.length)); height: parent.height
                    Row {
                        anchors.fill: parent; spacing: 0
                        Repeater {
                            model: parent.parent.modelData.values
                            Rectangle {
                                required property var modelData
                                required property int index
                                width: parent.width/Math.max(1,chart.series.length); height: Math.max(modelData.n?1:0,parent.height*modelData.share/chart.plotted.max); anchors.bottom: parent.bottom; color: chart.palette[index%chart.palette.length]
                                MouseArea { id: hover; anchors.fill: parent; hoverEnabled: true }
                                ToolTip.visible: hover.containsMouse
                                ToolTip.text: modelData.name+"\n"+(chart.places && bucket.modelData.from===chart.unclassified?qsTr("Nieklasyfikowany"):bucket.modelData.from+"–"+bucket.modelData.to)+" · "+modelData.n+" · "+(modelData.share*100).toLocaleString(Qt.locale(),'f',2)+" %"
                            }
                        }
                    }
                }
            }
        }
        EmptyState { anchors.fill: parent; visible: chart.plotted.bins.length===0; compact: true; title: qsTr("Brak obserwacji w tym zakresie") }
    }
    RowLayout { Layout.fillWidth: true; Label { text: chart.plotted.lo; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption } Item { Layout.fillWidth: true } Label { text: chart.places && chart.plotted.hi===chart.unclassified ? qsTr("N/K") : chart.plotted.hi; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption } }
    Label { text: qsTr("Udział obserwacji · najedź na słupek, aby zobaczyć liczbę i zakres."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
}
