import QtQuick 2.15

Item {
    id: mark
    property color color: Theme.signal
    implicitWidth: 32
    implicitHeight: 32
    Accessible.ignored: true
    Repeater {
        model: 2
        Rectangle {
            x: 7 + index * 12
            y: 2
            width: 5
            height: mark.height - 4
            rotation: 24
            color: mark.color
        }
    }
}
