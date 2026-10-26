import QtQuick 2.15

Image {
    id: art
    property string family: "patch"
    property string perkKey
    property int tier: 0
    source: "icons/athlete/"+family+"-"+perkKey+"-"+tier+".svg"
    sourceSize.width: 192
    sourceSize.height: 192
    Accessible.ignored: true
    Image {
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -art.height*.06
        width: art.width*.72
        height: width
        source: "image://perkIcons/"+art.perkKey
        sourceSize.width: 256
        sourceSize.height: 256
        asynchronous: true
        Accessible.ignored: true
    }
}
