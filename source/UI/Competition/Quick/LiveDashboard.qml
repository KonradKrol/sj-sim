import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Column {
    id: dashboard
    property var nodes: []
    property var theme
    spacing: 14
    function find(name, list) {
        list=list || nodes
        for(var i=0;i<list.length;++i) {
            if(list[i].name===name) return list[i]
            if(list[i].children) { var result=find(name,list[i].children); if(result.id) return result }
        }
        return {kind:"group",children:[]}
    }
    function text(name) { return find(name).text || "—" }
    Label { text:dashboard.text("label_hillNameAndHSPoint"); color:theme.muted; font.pixelSize:13; width:parent.width; elide:Text.ElideRight }
    Rectangle {
        width:parent.width; height:athlete.implicitHeight+30; radius:12
        color:theme.dark ? "#223754" : "#eaf2ff"
        Column {
            id:athlete; anchors.left:parent.left; anchors.right:parent.right; anchors.top:parent.top; anchors.margins:15; spacing:10
            Label { text:"NA BELCE"; color:theme.dark ? "#8bbcff" : "#3975c7"; font.pixelSize:10; font.letterSpacing:2; font.bold:true }
            RowLayout {
                width:parent.width
                Image { source:dashboard.find("label_flag").image || ""; Layout.preferredWidth:30; Layout.preferredHeight:22; fillMode:Image.PreserveAspectFit }
                Label { text:dashboard.text("label_nameAndSurname"); font.pixelSize:22; font.weight:Font.DemiBold; color:theme.ink; Layout.fillWidth:true; wrapMode:Text.WordWrap }
                Image { source:dashboard.find("label_nextJumperIMG").image || ""; Layout.preferredWidth:48; Layout.preferredHeight:54; fillMode:Image.PreserveAspectFit }
            }
            Flow {
                width:parent.width; spacing:12
                Repeater {
                    model:["label_pointsBehindLeader", "label_lastRoundPosition", "label_personalBest"]
                    Label { property var detail:dashboard.find(modelData); visible:!!detail.id && !!detail.text; text:(modelData==="label_personalBest" ? "PB " : modelData==="label_lastRoundPosition" ? "Miejsce " : "Do lidera ")+ (detail.text || ""); color:theme.muted; font.pixelSize:12 }
                }
            }
            LiveNode { width:parent.width; dataNode:dashboard.find("pushButton_jump"); theme:dashboard.theme }
            LiveNode { width:parent.width; dataNode:dashboard.find("pushButton_goToNext"); theme:dashboard.theme }
        }
    }
    GridLayout {
        width:parent.width; columns:2; columnSpacing:12; rowSpacing:3
        Label { text:"DO PROWADZENIA"; color:theme.muted; font.pixelSize:10; font.letterSpacing:1 }
        Label { text:"DO AWANSU"; color:theme.muted; font.pixelSize:10; font.letterSpacing:1 }
        Label { text:dashboard.text("label_toBeatDistance"); color:theme.ink; font.pixelSize:26; font.weight:Font.DemiBold; Layout.fillWidth:true }
        Label { text:dashboard.text("label_toAdvancementDistance"); color:theme.ink; font.pixelSize:26; font.weight:Font.DemiBold; Layout.fillWidth:true }
    }
    Rectangle { width:parent.width; height:1; color:theme.dark ? "#304159" : "#e2e8f1" }
    Label { text:"Warunki na skoczni"; color:theme.ink; font.weight:Font.DemiBold; font.pixelSize:15 }
    GridLayout {
        width:parent.width; columns:2; columnSpacing:10; rowSpacing:8
        Label { text:"Wiatr"; color:theme.muted }
        Label { text:dashboard.text("label_actualAvgWind"); color:theme.ink; font.bold:true; Layout.fillWidth:true; horizontalAlignment:Text.AlignRight }
        Label { text:"Śnieg"; color:theme.muted }
        Label { text:dashboard.text("label_snowLevel"); color:theme.ink; Layout.fillWidth:true; horizontalAlignment:Text.AlignRight; wrapMode:Text.WordWrap }
    }
    Label { text:"Belka startowa"; color:theme.muted; font.pixelSize:12 }
    LiveNode { width:parent.width; dataNode:dashboard.find("spinBox_actualGate"); theme:dashboard.theme }
    Repeater {
        model:["pushButton_coachGate", "pushButton_generateNewWinds", "pushButton_manipulateJump"]
        LiveNode { width:parent.width; dataNode:dashboard.find(modelData); theme:dashboard.theme }
    }
    Label { text:"Ustawienia i przebieg"; color:theme.ink; font.weight:Font.DemiBold; font.pixelSize:15 }
    Repeater {
        model:["pushButton_windsGeneratorSettings", "pushButton_inrunSnowGeneratorSettings", "toolButton_autoSimulationActions", "toolButton_cancelActions", "checkBox_liveCompetition", "pushButton_sendResultsWebhook", "label_record"]
        LiveNode { width:parent.width; dataNode:dashboard.find(modelData); theme:dashboard.theme }
    }
}
