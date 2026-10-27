import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id:arena
    property var theme
    property var startTable:theme.find("listView_startList",theme.section(2))
    property var koTable:theme.find("tableView_KOGroupResults",theme.section(2))
    property bool showStandings:theme.page===4
    spacing:10
    Rectangle {
        Layout.fillWidth:true; implicitHeight:athleteRow.implicitHeight+24
        color:theme.panel; radius:9; border.color:theme.line
        RowLayout {
            id:athleteRow; anchors.fill:parent; anchors.margins:12; spacing:theme.width>1100 ? 24 : 12
            Rectangle { Layout.preferredWidth:3; Layout.fillHeight:true; radius:2; color:"#387bea" }
            Image {
                visible:!!source.toString() && theme.width>=900
                source:theme.find("label_nextJumperIMG").image || ""
                Layout.preferredWidth:48; Layout.preferredHeight:56; fillMode:Image.PreserveAspectFit
            }
            ColumnLayout {
                Layout.fillWidth:true; spacing:6
                Label { text:qsTr("NA BELCE"); color:theme.muted; font.pixelSize:10; font.letterSpacing:1.5; font.bold:true }
                RowLayout {
                    Image { source:theme.find("label_flag").image || ""; Layout.preferredWidth:28; Layout.preferredHeight:20; fillMode:Image.PreserveAspectFit }
                    Label { text:theme.text("label_nameAndSurname"); color:theme.ink; font.pixelSize:theme.width<700 ? 18 : 23; font.weight:Font.DemiBold; Layout.fillWidth:true; elide:Text.ElideRight }
                }
                Label { visible:!!theme.find("label_pointsBehindLeader").id || !!theme.find("label_lastRoundPosition").id || !!theme.find("label_personalBest").id; text:[theme.find("label_lastRoundPosition").id ? "Miejsce po serii: "+theme.text("label_lastRoundPosition") : "",theme.find("label_pointsBehindLeader").id ? "Do lidera: "+theme.text("label_pointsBehindLeader") : "",theme.find("label_personalBest").id ? "PB: "+theme.text("label_personalBest") : ""].filter(function(x){return x.length}).join("   ·   "); color:theme.muted; font.pixelSize:11; Layout.fillWidth:true; elide:Text.ElideRight }
            }
            Repeater {
                model:theme.width>=700 ? [{title:"DO PROWADZENIA",name:"label_toBeatDistance"},{title:"DO AWANSU",name:"label_toAdvancementDistance"}] : []
                ColumnLayout {
                    spacing:4
                    Label { text:modelData.title; color:theme.muted; font.pixelSize:10 }
                    Label { text:theme.text(modelData.name); color:modelData.name==="label_toBeatDistance" ? (theme.dark ? "#8cb9ff" : "#2562bd") : theme.ink; font.pixelSize:24; font.weight:Font.DemiBold }
                }
            }
            ColumnLayout {
                Layout.preferredWidth:theme.width<700 ? 126 : 170; Layout.maximumWidth:theme.width<700 ? 126 : 170; spacing:4
                LiveNode { Layout.fillWidth:true; dataNode:theme.find("pushButton_goToNext").id ? theme.find("pushButton_goToNext") : theme.find("pushButton_jump"); theme:arena.theme }
                Label { text:"Ctrl + Enter"; color:theme.muted; font.pixelSize:10; Layout.alignment:Qt.AlignHCenter }
            }
        }
    }
    Flow {
        id:toolbar
        Layout.fillWidth:true; Layout.preferredHeight:implicitHeight
        spacing:6
        Rectangle {
            width:gateRow.implicitWidth+16; height:38; radius:6; color:theme.panel; border.color:theme.line
            RowLayout {
                id:gateRow; anchors.centerIn:parent; spacing:7
                Label { text:qsTr("Belka"); color:theme.muted; font.pixelSize:12 }
                TextField {
                    id:gate; objectName:"input-spinBox_actualGate"; implicitWidth:45; implicitHeight:30; padding:4; selectByMouse:true
                    text:theme.find("spinBox_actualGate").value===undefined ? "" : theme.find("spinBox_actualGate").value
                    enabled:!!theme.find("spinBox_actualGate").enabled
                    color:theme.ink; horizontalAlignment:Text.AlignHCenter
                    validator:IntValidator { bottom:theme.find("spinBox_actualGate").min || 0; top:theme.find("spinBox_actualGate").max || 1000 }
                    background:Rectangle { radius:4; color:theme.dark ? "#121d2c" : "#f2f5f9"; border.color:gate.activeFocus ? "#639aff" : "transparent" }
                    onEditingFinished:if(acceptableInput && Number(text)!==theme.find("spinBox_actualGate").value) live.edit(theme.find("spinBox_actualGate").id,Number(text))
                }
            }
        }
        Repeater {
            model:["pushButton_coachGate","pushButton_manipulateJump","pushButton_generateNewWinds"]
            LiveNode { caption:modelData==="pushButton_generateNewWinds" ? "Nowy wiatr" : ""; width:modelData==="pushButton_generateNewWinds" ? 112 : modelData==="pushButton_manipulateJump" ? 108 : 120; height:38; dataNode:theme.find(modelData); theme:arena.theme }
        }
        Rectangle {
            visible:theme.width<700; width:weather.implicitWidth+20; height:38; radius:6; color:theme.panel; border.color:theme.line
            Row {
                id:weather; anchors.centerIn:parent; spacing:12
                Label { text:qsTr("Wiatr  ")+theme.text("label_actualAvgWind"); color:theme.ink; font.pixelSize:12 }
                Label { text:qsTr("Śnieg  ")+theme.text("label_snowLevel"); color:theme.muted; font.pixelSize:12 }
            }
        }
        LiveNode { caption:"Auto-symulacja ▾"; width:160; height:38; dataNode:theme.find("toolButton_autoSimulationActions"); theme:arena.theme }
        LiveButton { theme:arena.theme; text:qsTr("Ustawienia  ···"); height:38; onClicked:settings.open() }
    }
    RowLayout {
        Layout.fillWidth:true; visible:!theme.wide; spacing:6
        Repeater {
            model:[{text:qsTr("Wyniki"),page:1},{text:qsTr("Lista startowa"),page:2},{text:qsTr("Ostatni skok"),page:3}]
            LiveButton { Layout.fillWidth:true; theme:arena.theme; text:modelData.text; primary:theme.page===modelData.page || (modelData.page===1 && theme.page===0); onClicked:theme.page=modelData.page }
        }
    }
    RowLayout {
        Layout.fillWidth:true; Layout.fillHeight:true; spacing:10
        Rectangle {
            visible:theme.wide || (theme.page!==2 && theme.page!==3)
            Layout.fillWidth:true; Layout.fillHeight:true; color:theme.panel; radius:9; border.color:theme.line
            ColumnLayout {
                anchors.fill:parent; anchors.margins:12; spacing:8
                RowLayout {
                    Layout.fillWidth:true
                    Label { text:arena.showStandings ? qsTr("Klasyfikacja na żywo") : qsTr("Wyniki konkursu"); color:theme.ink; font.pixelSize:15; font.weight:Font.DemiBold; Layout.fillWidth:true }
                    LiveButton { visible:theme.section(4).length>0; theme:arena.theme; text:arena.showStandings ? qsTr("Wyniki") : qsTr("Klasyfikacja"); implicitHeight:30; onClicked:theme.page=arena.showStandings ? 1 : 4 }
                }
                Label { visible:!arena.showStandings; text:qsTr("Dwuklik zawodnika pokazuje jego skoki w panelu obok"); color:theme.muted; font.pixelSize:11; Layout.fillWidth:true; elide:Text.ElideRight }
                Loader {
                    Layout.fillWidth:true; Layout.fillHeight:true
                    sourceComponent:arena.showStandings ? classification : results
                }
            }
        }
        ColumnLayout {
            visible:theme.wide || theme.page===2 || theme.page===3
            Layout.preferredWidth:theme.width>=1300 ? 370 : 310
            Layout.fillWidth:!theme.wide; Layout.fillHeight:true; spacing:10
            Rectangle {
                id:startPanel; objectName:"persistentStartList"
                visible:theme.wide || theme.page===2
                Layout.fillWidth:true; Layout.fillHeight:true
                Layout.preferredHeight:arena.koTable.id ? 280 : 230; Layout.minimumHeight:130
                color:theme.panel; radius:9; border.color:theme.line
                ColumnLayout {
                    anchors.fill:parent; anchors.margins:12; spacing:6
                    RowLayout {
                        Layout.fillWidth:true
                        Label { text:qsTr("Lista startowa"); color:theme.ink; font.pixelSize:15; font.weight:Font.DemiBold; Layout.fillWidth:true }
                        Label { text:arena.startTable.rows ? arena.startTable.rows.length+qsTr(" zawodników") : ""; color:theme.muted; font.pixelSize:11 }
                    }
                    LiveTable { visible:!!arena.koTable.id; Layout.fillWidth:true; Layout.preferredHeight:Math.min(110,28+40*(arena.koTable.rows || []).length); table:arena.koTable; theme:arena.theme; compact:true }
                    LiveTable { Layout.fillWidth:true; Layout.fillHeight:true; table:arena.startTable; theme:arena.theme; compact:true }
                }
            }
            Rectangle {
                id:jumpPanel; objectName:"automaticJumpPreview"
                visible:theme.wide || theme.page===3
                Layout.fillWidth:true; Layout.fillHeight:true; Layout.preferredHeight:240; Layout.minimumHeight:130
                color:theme.panel; radius:9; border.color:theme.line
                LiveJumpPreview { anchors.fill:parent; anchors.margins:12; theme:arena.theme; nodes:theme.section(3) }
            }
        }
    }
    Rectangle {
        visible:!theme.wide && theme.page!==3 && theme.section(3).length>0
        Layout.fillWidth:true; Layout.preferredHeight:140
        color:theme.panel; radius:9; border.color:theme.line
        LiveJumpPreview { anchors.fill:parent; anchors.margins:10; theme:arena.theme; nodes:theme.section(3) }
    }
    Component {
        id:results
        Item {
            Repeater { model:theme.section(1); LiveTable { anchors.fill:parent; table:modelData; theme:arena.theme } }
        }
    }
    Component {
        id:classification
        ScrollView {
            clip:true; contentWidth:availableWidth
            Column { width:parent.width; spacing:8; Repeater { model:theme.section(4); LiveNode { width:parent.width; dataNode:modelData; theme:arena.theme } } }
        }
    }
    Popup {
        id:settings; parent:Overlay.overlay; anchors.centerIn:parent; width:Math.min(420,theme.width-32); height:Math.min(600,theme.height-40); modal:true; focus:true; padding:18
        background:Rectangle { color:theme.panel; radius:10; border.color:theme.line }
        ScrollView {
            anchors.fill:parent; contentWidth:availableWidth; clip:true
            Column {
                width:parent.width; spacing:9
                Label { text:qsTr("Ustawienia i przebieg konkursu"); color:theme.ink; font.bold:true; width:parent.width; wrapMode:Text.WordWrap }
                Repeater { model:["pushButton_windsGeneratorSettings","pushButton_inrunSnowGeneratorSettings","toolButton_cancelActions","checkBox_liveCompetition","pushButton_sendResultsWebhook","label_record"]; LiveNode { width:parent.width; dataNode:theme.find(modelData); theme:arena.theme } }
                LiveButton { width:parent.width; theme:arena.theme; text:qsTr("Zamknij"); onClicked:settings.close() }
            }
        }
    }
}
