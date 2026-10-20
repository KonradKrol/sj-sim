import QtQuick 2.15

Rectangle {
    color: Theme.surface
    implicitHeight: settings.implicitHeight
    AppearanceMenu {
        id: settings
        x: 0; y: 0; width: parent.width
        visible: true
        onClosed: navigation.closeAppearance()
    }
}
