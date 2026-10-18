import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Pane {
    id: root
    padding: 0
    font.family: "Arial"
    font.pixelSize: 14
    palette.highlight: "#2e55c5"
    palette.text: "#292e2a"
    palette.buttonText: "#363b39"
    palette.base: "#ffffff"
    focus: true
    property var sections: [qsTr("Konkurs"), qsTr("Skocznia"), qsTr("Zasady"), qsTr("Warunki"), qsTr("Zawodnicy")]
    property var selectedRows: []
    property real pageWidth: Math.min(width - (width < 700 ? 32 : 80), 1000)
    function run(name) { focusSink.forceActiveFocus(); competition.action(name, selectedRows) }
    function navigate(index) {
        if (index === competition.section) return
        focusSink.forceActiveFocus()
        competition.section = index
        scroll.contentItem.contentY = 0
        pageAnimation.restart()
    }
    background: Rectangle { color: "#f6f7f4" }
    Item { id: focusSink; objectName: "commitFocus"; focus: true }
    Shortcut { sequence: "Ctrl+Return"; onActivated: root.run("start") }
    Shortcut { sequence: "Alt+1"; onActivated: root.navigate(0) }
    Shortcut { sequence: "Alt+2"; onActivated: root.navigate(1) }
    Shortcut { sequence: "Alt+3"; onActivated: root.navigate(2) }
    Shortcut { sequence: "Alt+4"; onActivated: root.navigate(3) }
    Shortcut { sequence: "Alt+5"; onActivated: root.navigate(4) }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        Item {
            Layout.fillWidth: true
            implicitHeight: 78
            RowLayout {
                width: root.pageWidth
                anchors.centerIn: parent
                spacing: 16
                Label { text: "Sj.Sim"; color: "#2e55c5"; font.pixelSize: 19; font.bold: true; visible: root.width >= 700 }
                Rectangle { width: 1; height: 22; color: "#daded5"; visible: root.width >= 700 }
                Label { text: qsTr("Pojedynczy konkurs"); font.pixelSize: 21; color: "#262d25"; Layout.fillWidth: true }
                ActionButton { text: qsTr("Widok klasyczny"); onClicked: root.run("widgets"); ToolTip.visible: hovered; ToolTip.text: qsTr("Otwórz te same ustawienia w Qt Widgets") }
            }
        }
        Item {
            Layout.fillWidth: true
            implicitHeight: 58
            Rectangle {
                id: tabTrack
                width: Math.min(root.pageWidth, 640)
                height: 42
                x: (parent.width - root.pageWidth) / 2
                radius: 12
                color: "#eaece6"
                Rectangle {
                    x: 4 + competition.section * ((tabTrack.width - 8) / 5)
                    y: 4
                    width: (tabTrack.width - 8) / 5
                    height: 34
                    radius: 9; color: "white"
                    Behavior on x { NumberAnimation { duration: 190; easing.type: Easing.OutCubic } }
                }
                Row {
                    anchors.fill: parent; anchors.margins: 4
                    Repeater {
                        model: root.sections
                        delegate: Button {
                            width: (tabTrack.width - 8) / 5; height: 34
                            text: modelData
                            background: Rectangle { radius: 9; color: parent.hovered && competition.section !== index ? "#e0e4da" : "transparent"; border.color: parent.activeFocus ? "#2e55c5" : "transparent"; Behavior on color { ColorAnimation { duration: 100 } } }
                            contentItem: Text { text: parent.text; font.family: root.font.family; font.pixelSize: root.width < 600 ? 12 : 14; color: competition.section === index ? "#292e2a" : "#757c70"; verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter }
                            onClicked: root.navigate(index)
                        }
                    }
                }
            }
        }
        ScrollView {
            id: scroll
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            contentWidth: availableWidth
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            Item {
                width: scroll.availableWidth
                implicitHeight: page.height + 32
                Column {
                    id: page
                    width: root.pageWidth
                    x: (scroll.availableWidth - width) / 2
                    spacing: 16
                    ParallelAnimation {
                        id: pageAnimation
                        NumberAnimation { target: page; property: "opacity"; from: 0.4; to: 1; duration: 150 }
                        NumberAnimation { target: page; property: "y"; from: 6; to: 0; duration: 180; easing.type: Easing.OutCubic }
                    }
                    Flow {
                        width: parent.width
                        spacing: 8
                        visible: competition.section !== 2
                        ActionButton { visible: competition.section === 0; text: qsTr("Dobierz belkę"); onClicked: root.run("autoGate") }
                        ActionButton { visible: competition.section === 1; text: qsTr("Cechy skoczni…"); onClicked: root.run("characteristics") }
                        ActionButton { visible: competition.section === 3; text: qsTr("Losuj warunki…"); onClicked: root.run("randomWind") }
                        ActionButton { visible: competition.section === 3; text: qsTr("Wszystkie sektory…"); onClicked: root.run("fillWind") }
                        ActionButton { visible: competition.section === 4; text: qsTr("Wczytaj listę…"); onClicked: { root.selectedRows = []; root.run("load") } }
                        ActionButton { visible: competition.section === 4; text: qsTr("Losuj kolejność"); onClicked: { root.run("randomOrder"); root.selectedRows = [] } }
                        ActionButton { visible: competition.section === 4; text: qsTr("Według krajów"); onClicked: { root.run("countryOrder"); root.selectedRows = [] } }
                        ActionButton { visible: competition.section === 4 && competition.summary.team; text: qsTr("Składy drużyn…"); onClicked: root.run("teams") }
                        ActionButton { visible: competition.section === 4 && competition.summary.ko; text: qsTr("Grupy KO…"); onClicked: root.run("ko") }
                    }
                    Repeater {
                        model: competition.cards.length
                        delegate: Rectangle {
                            property var cardData: competition.cards[index]
                            width: page.width
                            height: cardContent.height + 48
                            radius: 16
                            color: "white"
                            Column {
                                id: cardContent
                                x: 24; y: 24
                                width: parent.width - 48
                                spacing: 22
                                Label { text: cardData.title; color: "#2c342a"; font.pixelSize: 16; font.weight: Font.Medium; width: parent.width; wrapMode: Text.WordWrap }
                                Grid {
                                    width: parent.width
                                    columns: root.pageWidth >= 650 ? 2 : 1
                                    columnSpacing: 36; rowSpacing: 20
                                    Repeater {
                                        model: cardData.fields.length
                                        Field { width: (cardContent.width - (parent.columns - 1) * parent.columnSpacing) / parent.columns; descriptor: cardData.fields[index] }
                                    }
                                }
                            }
                        }
                    }
                    Rectangle {
                        visible: competition.section === 4 && !competition.summary.team && !competition.summary.ko
                        width: parent.width
                        height: roster.height + 40
                        radius: 16; color: "white"
                        Column {
                            id: roster
                            x: 20; y: 20; width: parent.width - 40
                            spacing: 12
                            RowLayout {
                                width: parent.width
                                Label { text: qsTr("Lista startowa"); font.pixelSize: 16; color: "#2c342a"; Layout.fillWidth: true }
                                Label { text: competition.summary.count + qsTr(" zawodników"); color: "#868d80"; font.pixelSize: 13 }
                            }
                            TextField {
                                id: search
                                width: parent.width; height: 42
                                placeholderText: qsTr("Szukaj zawodnika lub kraju")
                                selectByMouse: true
                                leftPadding: 12
                                Accessible.name: placeholderText
                                background: Rectangle { color: "#f4f5f2"; radius: 9; border.color: search.activeFocus ? "#2e55c5" : color }
                            }
                            Flow {
                                width: parent.width; spacing: 6
                                ActionButton { text: qsTr("W górę"); enabled: root.selectedRows.length > 0; onClicked: { root.run("up"); root.selectedRows = [] } }
                                ActionButton { text: qsTr("W dół"); enabled: root.selectedRows.length > 0; onClicked: { root.run("down"); root.selectedRows = [] } }
                                ActionButton { text: qsTr("Usuń z listy"); enabled: root.selectedRows.length > 0; onClicked: { root.run("remove"); root.selectedRows = [] } }
                            }
                            Repeater {
                                model: competition.jumpers
                                delegate: CheckDelegate {
                                    width: roster.width; height: 42
                                    visible: (modelData.name + " " + modelData.country).toLowerCase().indexOf(search.text.toLowerCase()) >= 0
                                    text: (modelData.row + 1) + ".   " + modelData.name + "   ·   " + modelData.country
                                    checked: root.selectedRows.indexOf(modelData.row) >= 0
                                    onClicked: {
                                        var rows = root.selectedRows.slice()
                                        var pos = rows.indexOf(modelData.row)
                                        if (pos >= 0) rows.splice(pos, 1); else rows.push(modelData.row)
                                        root.selectedRows = rows
                                    }
                                    background: Rectangle { radius: 8; color: parent.checked ? "#edf1fd" : parent.hovered ? "#f4f5f2" : "transparent"; Behavior on color { ColorAnimation { duration: 100 } } }
                                    contentItem: Label { text: parent.text; rightPadding: 36; verticalAlignment: Text.AlignVCenter; elide: Text.ElideRight; color: "#3e463b" }
                                }
                            }
                            Label { visible: competition.summary.count === 0; text: qsTr("Wczytaj zawodników z pliku JSON, aby utworzyć listę."); width: parent.width; wrapMode: Text.WordWrap }
                        }
                    }
                    Label {
                        visible: competition.section === 4 && (competition.summary.team || competition.summary.ko)
                        text: competition.summary.team ? qsTr("Składy %1 drużyn możesz zmienić w edytorze składów.").arg(competition.summary.teams) : qsTr("Dobierz grupy KO lub zmień ich skład w edytorze.")
                        width: parent.width; wrapMode: Text.WordWrap; color: "#737d6d"
                    }
                }
            }
        }
        Rectangle {
            Layout.fillWidth: true
            implicitHeight: 80
            color: "#f6f7f4"
            RowLayout {
                width: root.pageWidth
                anchors.centerIn: parent
                spacing: 20
                Label {
                    Layout.fillWidth: true
                    text: (competition.summary.hs > 0 ? competition.summary.hill + " HS " + competition.summary.hs + "  ·  " : "") + competition.summary.count + qsTr(" zawodników")
                    color: "#7b8475"; font.pixelSize: 13
                    elide: Text.ElideRight
                }
                ActionButton { primary: true; text: qsTr("Rozpocznij konkurs"); implicitHeight: 44; leftPadding: 26; rightPadding: 26; onClicked: root.run("start"); ToolTip.visible: hovered; ToolTip.text: "Ctrl+Enter" }
            }
        }
    }
}
