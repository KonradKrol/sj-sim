import QtQuick 2.15

Rectangle {
    color: Theme.background
    LayoutMirroring.enabled: Qt.application.layoutDirection === Qt.RightToLeft
    LayoutMirroring.childrenInherit: true
    Loader {
        anchors.fill: parent
        source: ui.profile === "SingleJumpsResultsWindow" ? Qt.resolvedUrl("SingleJumpsResultsScreen.qml") : ui.profile === "AuthorWindow" ? Qt.resolvedUrl("AuthorPage.qml") : ui.profile === "CompetitionManagerWindow" ? Qt.resolvedUrl("RaceScreen.qml") : Qt.resolvedUrl("Workspace.qml")
    }
    Keys.onEscapePressed: navigation.back()
}
