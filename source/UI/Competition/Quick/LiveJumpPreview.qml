import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id:preview
    property var theme
    property var nodes:[]
    property var tabs:theme.find("tabWidget_jumps",nodes)
    property var jumpNodes:tabs.children || []
    property bool hasJump:nodes.length>0
    property string athlete:theme.find("label_nameAndSurname",nodes).text || ""
    onAthleteChanged:reveal.restart()
    SequentialAnimation { id:reveal; NumberAnimation { target:preview; property:"opacity"; from:.65; to:1; duration:180 } }
    spacing:6
    RowLayout {
        Layout.fillWidth:true
        Label { text:"Podgląd skoku"; color:theme.ink; font.pixelSize:15; font.weight:Font.DemiBold; Layout.fillWidth:true }
        LiveButton { visible:preview.hasJump; theme:preview.theme; text:"Pełne dane ↗"; implicitHeight:28; font.pixelSize:11; onClicked:full.open() }
    }
    Label {
        visible:!preview.hasJump; Layout.fillWidth:true; Layout.fillHeight:true; wrapMode:Text.WordWrap; verticalAlignment:Text.AlignVCenter; horizontalAlignment:Text.AlignHCenter
        text:"Po oddaniu skoku wynik pojawi się tutaj automatycznie."; color:theme.muted; font.pixelSize:12
    }
    ScrollView {
        visible:preview.hasJump; Layout.fillWidth:true; Layout.fillHeight:true; contentWidth:availableWidth; clip:true
        Column {
            width:parent.width; spacing:8
            RowLayout {
                width:parent.width
                Image { source:theme.find("label_flag",preview.nodes).image || ""; Layout.preferredWidth:26; Layout.preferredHeight:18; fillMode:Image.PreserveAspectFit }
                Label { objectName:"automaticPreviewAthlete"; text:preview.athlete; color:theme.ink; font.pixelSize:17; font.weight:Font.DemiBold; Layout.fillWidth:true; elide:Text.ElideRight }
                Label { text:theme.find("label_actualPosition",preview.nodes).text || ""; color:theme.muted; font.pixelSize:11 }
            }
            RowLayout {
                width:parent.width
                Repeater {
                    model:[{name:"label_distance",title:"ODLEGŁOŚĆ"},{name:"label_points",title:"PUNKTY"}]
                    ColumnLayout {
                        Layout.fillWidth:true; spacing:1
                        Label { text:modelData.title; color:theme.muted; font.pixelSize:9; font.letterSpacing:1 }
                        Label { objectName:modelData.name==="label_distance" ? "automaticPreviewDistance" : "automaticPreviewPoints"; text:theme.find(modelData.name,preview.jumpNodes).text || "—"; color:modelData.name==="label_distance" ? (theme.dark ? "#8cb9ff" : "#2562bd") : theme.ink; font.pixelSize:25; font.weight:Font.DemiBold }
                    }
                }
            }
            Label { visible:!!theme.find("label_DSQ",preview.jumpNodes).id; text:theme.find("label_DSQ",preview.jumpNodes).text || ""; color:"#e47171"; font.bold:true }
            RowLayout {
                width:parent.width
                Label { text:"Belka "+(theme.find("label_gate",preview.jumpNodes).text || "—"); color:theme.muted; font.pixelSize:12; Layout.fillWidth:true }
                Label { text:"Wiatr "+(theme.find("label_averageWind",preview.jumpNodes).text || "—"); color:theme.muted; font.pixelSize:12 }
            }
            Label { text:theme.find("label_judges",preview.jumpNodes).text || ""; color:theme.ink; font.pixelSize:12; width:parent.width; wrapMode:Text.WordWrap }
            Label { text:"Łącznie: "+(theme.find("label_pointsSum",preview.nodes).text || "—"); color:theme.muted; font.pixelSize:12 }
            LiveChoice { visible:(preview.tabs.options || []).length>1; width:parent.width; theme:preview.theme; model:preview.tabs.options || []; currentIndex:preview.tabs.value || 0; onActivated:live.edit(preview.tabs.id,currentIndex) }
            GridLayout {
                width:parent.width; columns:2; columnSpacing:10; rowSpacing:7
                Repeater {
                    model:[{name:"label_totalCompensation",title:"Rekompensata"},{name:"label_judgesPoints",title:"Punkty za styl"},{name:"label_landingType",title:"Lądowanie"},{name:"label_positionInRound",title:"Miejsce w serii"},{name:"label_takeoffRating",title:"Wybicie"},{name:"label_flightRating",title:"Lot"},{name:"label_snowRain",title:"Śnieg"},{name:"label_random",title:"Losowość"}]
                    ColumnLayout {
                        property var metric:theme.find(modelData.name,preview.jumpNodes)
                        visible:!!metric.id; Layout.fillWidth:true; spacing:1
                        Label { text:modelData.title; color:theme.muted; font.pixelSize:10 }
                        Label { text:parent.metric.text || "—"; color:theme.ink; font.pixelSize:12; Layout.fillWidth:true; elide:Text.ElideRight }
                    }
                }
            }
        }
    }
    Popup {
        id:full; parent:Overlay.overlay; anchors.centerIn:parent; width:Math.min(720,theme.width-32); height:Math.min(800,theme.height-32); modal:true; focus:true; padding:18
        background:Rectangle { color:theme.panel; radius:10; border.color:theme.line }
        ColumnLayout {
            anchors.fill:parent
            RowLayout { Layout.fillWidth:true; Label { text:"Szczegóły skoków"; color:theme.ink; font.bold:true; Layout.fillWidth:true } LiveButton { theme:preview.theme; text:"Zamknij"; onClicked:full.close() } }
            ScrollView {
                Layout.fillWidth:true; Layout.fillHeight:true; contentWidth:availableWidth; clip:true
                Column { width:parent.width; spacing:8; Repeater { model:preview.nodes; LiveNode { width:parent.width; dataNode:modelData; theme:preview.theme } } }
            }
        }
    }
}
