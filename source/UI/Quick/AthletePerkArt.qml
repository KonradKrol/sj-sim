import QtQuick 2.15

Image {
    id: art
    property string family: "patch"
    property string perkKey
    property int tier: 0
    property bool hill: false
    source: hill ? "" : "icons/athlete/"+family+"-"+perkKey+"-"+Math.min(2,tier)+".svg"
    sourceSize.width: 192
    sourceSize.height: 192
    Accessible.ignored: true
    Rectangle {
        visible: art.tier===4 || art.hill
        anchors.fill: parent; anchors.margins: art.width*.03
        color: art.tier===4 ? "#241237" : art.tier===2 ? "#6c4b1b" : "#224c40"; radius: art.width*.18
        border.width: Math.max(2,art.width*.035); border.color: art.tier>=2 ? "#ffb941" : "#91b6a6"
        Rectangle { anchors.fill: parent; anchors.margins: art.width*.08; color: "transparent"; radius: art.width*.12; border.width: 1; border.color: "#ffdf9d" }
    }
    Image {
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -art.height*.06
        width: art.width*.72
        height: width
        source: art.hill ? "icons/hill/"+art.perkKey+".svg" : "image://perkIcons/"+art.perkKey
        sourceSize.width: 256
        sourceSize.height: 256
        asynchronous: true
        Accessible.ignored: true
    }
    Text { visible: art.tier===4; anchors.horizontalCenter: parent.horizontalCenter; anchors.bottom: parent.bottom; anchors.bottomMargin: art.height*.02; text: "IV"; color: "#ffdf9d"; font.bold: true; font.pixelSize: art.height*.19; Accessible.ignored: true }
}
