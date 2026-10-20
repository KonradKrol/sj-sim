import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: hero
    property bool compact: false
    readonly property int inset: compact ? 20 : 32
    color: Theme.background
    implicitHeight: (compact ? copy.implicitHeight : Math.max(copy.implicitHeight,104)) + inset * 2 + 16
    clip: true
    Item {
        anchors.right: parent.right; anchors.top: parent.top; anchors.bottom: parent.bottom
        width: Math.min(parent.width * 0.4,480)
        visible: !hero.compact
        Repeater {
            model: 7
            Rectangle { x: index * 64; y: -80; width: 1; height: hero.height + 160; color: Theme.divider; rotation: 24 }
        }
    }
    Column {
        id: copy
        x: hero.inset; y: hero.inset
        width: hero.width - hero.inset * 2 - (hero.compact ? 0 : 240)
        spacing: 12
        Row {
            spacing: 12
            TrackMark { width: 28; height: 24 }
            Label { text: ui.version; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption; topPadding: 3 }
        }
        Label { objectName: "homeHeadline"; text: "Sj.Sim"; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.display; font.weight: Font.ExtraBold; font.letterSpacing: -1; width: parent.width }
        Label { objectName: "homeDescription"; text: qsTr("Symulator skoków narciarskich"); color: Theme.ink; font.pointSize: Theme.body; width: parent.width; wrapMode: Text.WordWrap }
    }
    Rectangle {
        visible: !hero.compact
        anchors.right: parent.right; anchors.top: parent.top; anchors.bottom: parent.bottom; anchors.margins: hero.inset
        width: 200
        color: "#ffffff"
        Rectangle { anchors.left: parent.left; anchors.top: parent.top; width: 5; height: parent.height; color: Theme.accent }
        Image {
            anchors.fill: parent; anchors.margins: 12
            source: "qrc:/sj-sim.png"
            sourceSize.width: 480; sourceSize.height: 480
            fillMode: Image.PreserveAspectFit
            asynchronous: true
            Accessible.name: qsTr("Logo Sj.Sim")
        }
    }
    Rectangle {
        anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom
        height: 16; color: Theme.background
        Repeater {
            model: Math.ceil(hero.width / 20)
            Rectangle { x: index * 20; width: 1; height: index % 5 === 0 ? 12 : 5; anchors.bottom: parent.bottom; color: Theme.divider }
        }
        Rectangle { width: hero.inset + 64; height: 3; anchors.bottom: parent.bottom; color: Theme.accent }
    }
}
