import QtQuick 2.15

Transition {
    id: fade
    property bool leaving: false
    NumberAnimation { property: "opacity"; from: fade.leaving ? 1 : 0; to: fade.leaving ? 0 : 1; duration: fade.leaving ? Theme.motion/2 : Theme.motion; easing.type: Easing.OutCubic }
}
