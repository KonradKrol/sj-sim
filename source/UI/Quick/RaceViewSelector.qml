import QtQuick 2.15
import QtQuick.Controls 2.15

SButton {
    id: selector
    property var backend
    property int field: 0
    property var model: []
    property int currentIndex: 0
    signal activated(int index)
    text: (model[currentIndex] || "")+" ▾"
    contentItem: Text { text: selector.text; font: selector.font; color: selector.foreground; verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter; elide: Text.ElideRight }
    implicitHeight: Theme.controlHeight
    Accessible.name: model[currentIndex] || ""
    function openOptions(){var at=mapToGlobal(0,height);backend.openResultViewOptions(field,at.x,at.y,width)}
    onClicked: openOptions()
    Keys.onDownPressed: openOptions()
}
