import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Loader {
    id: node
    property var dataNode: ({})
    property var theme
    property string caption: ""
    objectName: "live-" + (dataNode.name || dataNode.id || "group")
    property var n: dataNode
    enabled: n.enabled === undefined || n.enabled
    opacity: enabled ? 1 : .5
    sourceComponent: n.kind==="label" ? labelComponent : n.kind==="button" ? buttonComponent : n.kind==="check" ? checkComponent : n.kind==="number" ? numberComponent : n.kind==="choice" ? choiceComponent : n.kind==="tabs" ? tabsComponent : n.kind==="table" ? tableComponent : n.kind==="text" ? textComponent : n.kind==="slider" ? sliderComponent : groupComponent
    Component {
        id: groupComponent
        Column {
            spacing: 7
            Label { visible: !!n.text; text:n.text || ""; color:theme.ink; font.weight:Font.DemiBold; width:parent.width; wrapMode:Text.WordWrap }
            Repeater { model: n.children || []; Loader { width:parent.width; property var childNode:modelData; Component.onCompleted: setSource("LiveNode.qml", {"dataNode":childNode, "theme":node.theme}) } }
        }
    }
    Component {
        id: labelComponent
        RowLayout {
            spacing:8
            Image { visible:!!n.image; source:n.image || ""; fillMode:Image.PreserveAspectFit; Layout.preferredWidth: n.name && n.name.toLowerCase().indexOf("img")>=0 ? 70 : 32; Layout.preferredHeight: n.name && n.name.toLowerCase().indexOf("img")>=0 ? 70 : 22 }
            Label { visible:!!n.text; text:n.text || ""; color:theme.ink; Layout.fillWidth:true; wrapMode:Text.WordWrap; textFormat:Text.AutoText; font.pixelSize:n.name==="label_nameAndSurname" ? 20 : 14; font.weight:n.name==="label_nameAndSurname" ? Font.DemiBold : Font.Normal; onLinkActivated: Qt.openUrlExternally(link) }
        }
    }
    Component {
        id: buttonComponent
        LiveButton {
            theme:node.theme
            text:node.caption || ({pushButton_jump:"Oddaj skok",pushButton_generateNewWinds:"Wylosuj wiatr",pushButton_windsGeneratorSettings:"Ustawienia wiatru",pushButton_inrunSnowGeneratorSettings:qsTr("Ustawienia śniegu"),pushButton_coachGate:"Belka trenera",toolButton_autoSimulationActions:"Automatyczna symulacja ▾",toolButton_cancelActions:qsTr("Odwołaj…")})[n.name] || n.text || ""; primary:n.name==="pushButton_jump" || n.name==="pushButton_goToNext" || (n.name && n.name.indexOf("submit")>=0)
            hoverEnabled:true
            ToolTip.visible:hovered && !!n.tip; ToolTip.text:n.tip || ""; ToolTip.delay:600
            onClicked: { if(n.actions && n.actions.length) actionMenu.open(); else live.activate(n.id) }
            Menu {
                id:actionMenu
                Repeater { model:n.actions || []; MenuItem { text:modelData.text; enabled:modelData.enabled; onTriggered:live.activate(n.id,index) } }
            }
        }
    }
    Component { id:checkComponent; CheckBox {
        id:check; text:n.text; checked:!!n.value; onClicked:live.activate(n.id)
        implicitHeight:40
        contentItem:Text { text:check.text; color:theme.ink; leftPadding:34; verticalAlignment:Text.AlignVCenter; wrapMode:Text.WordWrap; font.pixelSize:13 }
        indicator:Rectangle { width:21; height:21; y:(check.height-height)/2; radius:4; color:check.checked ? "#2d6bdd" : theme.panel; border.color:check.activeFocus ? "#639aff" : theme.muted; Text { anchors.centerIn:parent; text:check.checked ? "✓" : ""; color:"white" } }
    } }
    Component {
        id:numberComponent
        RowLayout {
            spacing:5
            TextField {
                id:numberField; objectName:"input-"+(n.name || n.id)
                Layout.fillWidth:true; selectByMouse:true
                color:theme.ink
                background:Rectangle { radius:7; color:theme.dark ? "#142031" : "#f7f9fc"; border.color:numberField.activeFocus ? "#639aff" : theme.dark ? "#35465d" : "#dbe4ef" }
                text:Number(n.value).toLocaleString(Qt.locale(), 'f', n.decimals)
                validator:DoubleValidator { bottom:n.min; top:n.max; decimals:n.decimals; locale:Qt.locale().name }
                onEditingFinished: { var v=Number.fromLocaleString(Qt.locale(),text); if(isFinite(v) && acceptableInput && v!==n.value) live.edit(n.id,v) }
                Keys.onUpPressed:live.edit(n.id,Math.min(n.max,Number(n.value)+Number(n.step)))
                Keys.onDownPressed:live.edit(n.id,Math.max(n.min,Number(n.value)-Number(n.step)))
                ToolTip.visible:hovered && !!n.tip; ToolTip.text:n.tip || ""
            }
            Label { visible:!!n.suffix; text:n.suffix || ""; color:theme.muted }
            LiveButton { theme:node.theme; text:"−"; enabled:n.value>n.min; onClicked:live.edit(n.id,Math.max(n.min,Number(n.value)-Number(n.step))) }
            LiveButton { theme:node.theme; text:"+"; enabled:n.value<n.max; onClicked:live.edit(n.id,Math.min(n.max,Number(n.value)+Number(n.step))) }
        }
    }
    Component { id:choiceComponent; LiveChoice { theme:node.theme; model:n.options || []; currentIndex:n.value; onActivated:live.edit(n.id,currentIndex);  } }
    Component {
        id:tabsComponent
        Column {
            spacing:8
            LiveChoice { theme:node.theme; width:parent.width; model:n.options || []; currentIndex:n.value; onActivated:live.edit(n.id,currentIndex) }
            Repeater { model:n.children || []; Loader { width:parent.width; property var childNode:modelData; Component.onCompleted: setSource("LiveNode.qml", {"dataNode":childNode, "theme":node.theme}) } }
        }
    }
    Component { id:tableComponent; LiveTable { table:n; theme:node.theme; height:live.competition ? (n.name==="tableView_KOGroupResults" ? Math.min(220,50+(n.rows || []).length*40) : Math.max(260, Math.min(620,theme.height-230))) : 340 } }
    Component { id:textComponent; TextField { color:theme.ink; background:Rectangle { radius:7; color:theme.dark ? "#142031" : "#f7f9fc"; border.color:theme.muted } text:n.value || ""; selectByMouse:true; onEditingFinished:live.edit(n.id,text) } }
    Component { id:sliderComponent; Slider { from:n.min; to:n.max; value:n.value; stepSize:1; onMoved:live.edit(n.id,value) } }
}
