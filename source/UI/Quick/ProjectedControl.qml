import QtQuick 2.15

Loader {
    id: projection
    property var node
    active: !!node
    visible: active && node.values.available !== false
    source: Qt.resolvedUrl("Node.qml")
    onLoaded: item.node = Qt.binding(function(){return projection.node})
}
