import QtQuick 2.15

Rectangle {
    color: Theme.background
    LayoutMirroring.enabled: Qt.application.layoutDirection === Qt.RightToLeft
    LayoutMirroring.childrenInherit: true
    Loader {
        anchors.fill: parent
        source: ui.root.values.kind === "balanceLab" ? Qt.resolvedUrl("BalanceLabScreen.qml") : ui.root.values.kind === "playground" ? Qt.resolvedUrl("PlaygroundScreen.qml") : ui.root.values.kind === "classificationContributions" ? Qt.resolvedUrl("ClassificationContributionPage.qml") : ui.root.values.kind === "resultHistory" ? Qt.resolvedUrl("ResultsHistoryPage.qml") : ui.profile === "NewSimulationSaveConfigurationWindow" ? Qt.resolvedUrl("NewSimulationPage.qml") : ui.profile === "SingleJumpsResultsWindow" ? Qt.resolvedUrl("SingleJumpsResultsScreen.qml") : ui.profile === "AuthorWindow" ? Qt.resolvedUrl("AuthorPage.qml") : ui.profile === "CompetitionManagerWindow" ? Qt.resolvedUrl("RaceScreen.qml") : Qt.resolvedUrl("Workspace.qml")
    }
    Keys.onEscapePressed: navigation.back()
}
