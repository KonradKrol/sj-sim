import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: lab
    objectName: "balanceLabScreen"
    anchors.fill: parent
    anchors.margins: Theme.dp(14)
    spacing: Theme.dp(10)
    readonly property var backend: ui.root.values.backend
    readonly property var question: backend.questions[backend.question] || ({})
    readonly property var progress: backend.progress || ({})
    readonly property bool choosing: backend.options.started !== true
    property bool settingsOpen: false
    property bool profilesOpen: false
    GridLayout {
        columns: lab.width < 780 ? 2 : 5
        Layout.fillWidth: true
        SButton { visible: !lab.choosing; text: qsTr("Wybierz temat"); Accessible.name: qsTr("Wybierz temat"); quiet: true; onClicked: lab.backend.setOption("started",false) }
        ColumnLayout {
            Layout.fillWidth: true; spacing: 2
            Label { text: lab.choosing ? qsTr("Laboratorium balansu") : lab.question.name; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            Label { text: lab.choosing ? qsTr("Co chcesz sprawdzić?") : lab.question.prompt; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        }
        SButton { visible: !lab.choosing && lab.width < 920; text: qsTr("Parametry próby"); Accessible.name: qsTr("Ustawienia"); onClicked: lab.settingsOpen = !lab.settingsOpen }
        SButton { objectName: "balanceLabRun"; visible: !lab.choosing; text: lab.backend.busy ? qsTr("Przerwij obliczenia") : qsTr("Uruchom pełną próbę"); primary: true; onClicked: lab.backend.busy ? lab.backend.cancel() : lab.backend.start(false) }
        SButton { text: qsTr("Zapisy i historia"); Accessible.name: qsTr("Profile"); onClicked: lab.profilesOpen = true }
    }
    GridLayout {
        columns: lab.width < 780 ? 2 : 4
        visible: !lab.choosing; Layout.fillWidth: true
        ColumnLayout {
            Layout.fillWidth: true; spacing: Theme.dp(3)
            Label { objectName: "balanceExperimentProgress"; text: lab.backend.busy ? qsTr("%1 · %2 / %3 · etap %4 / %5").arg(lab.progress.stage || "").arg(lab.progress.done || 0).arg(lab.progress.total || 0).arg(lab.progress.phase || 0).arg(lab.progress.phases || 0) : lab.backend.previewBusy ? qsTr("Aktualizowanie podglądu…") : lab.backend.message; color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            ProgressBar { visible: lab.backend.busy; Layout.fillWidth: true; value: lab.progress.fraction || 0; indeterminate: !lab.progress.phases; Accessible.name: qsTr("Postęp etapów eksperymentu") }
            Label { visible: lab.backend.busy; text: lab.progress.remainingSeconds>=0 ? qsTr("%1% etapów · pozostało około %2 min %3 s").arg(Math.floor(100*(lab.progress.fraction || 0))).arg(Math.floor(lab.progress.remainingSeconds/60)).arg(Math.ceil(lab.progress.remainingSeconds%60)) : qsTr("Szacowanie pozostałego czasu…"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        }
        SButton { text: qsTr("Zapamiętaj do porównania"); hint: qsTr("Kopiuje Twoje zmiany do serii „Zapamiętane ustawienia”. Potem możesz dalej stroić i porównywać oba pomysły. Zastępuje poprzednio zapamiętany wariant; nie zapisuje wyniku próby."); quiet: true; enabled: !lab.backend.busy; onClicked: lab.backend.freeze() }
        SButton { text: qsTr("Cofnij zmianę"); quiet: true; enabled: !lab.backend.busy; onClicked: lab.backend.undo() }
    }
    SCheck { objectName: "balancePreviewToggle"; visible: !lab.choosing; text: qsTr("Pokazuj szybki podgląd · 7 konkursów na wariant"); checked: lab.backend.previewEnabled; onClicked: lab.backend.setPreviewEnabled(checked) }
    Loader {
        visible: lab.choosing; active: visible
        Layout.fillWidth: true; Layout.fillHeight: true
        sourceComponent: Component {
            ScrollView {
                id: questionScroll
                contentWidth: availableWidth; clip: true
                GridLayout {
                    width: questionScroll.availableWidth
                    columns: width >= 1100 ? 3 : width >= 650 ? 2 : 1
                    columnSpacing: Theme.dp(12); rowSpacing: Theme.dp(12)
                    Repeater {
                        model: lab.backend.questions
                        ItemDelegate {
                            required property var modelData
                            required property int index
                            objectName: "balanceQuestion" + index
                            Layout.fillWidth: true
                            Layout.preferredHeight: Math.max(Theme.dp(132),questionText.implicitHeight + Theme.dp(32))
                            padding: Theme.dp(16)
                            background: Rectangle { color: parent.hovered ? Theme.raised : Theme.surface; radius: Theme.dp(8); border.color: parent.activeFocus ? Theme.accent : "transparent"; border.width: 2 }
                            contentItem: ColumnLayout {
                                id: questionText
                                spacing: Theme.dp(8)
                                Label { text: modelData.name; color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; font.bold: true; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                                Label { text: modelData.prompt; color: Theme.muted; font.pointSize: Theme.body; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                            }
                            Accessible.name: modelData.name + ". " + modelData.prompt
                            onClicked: lab.backend.chooseQuestion(index)
                        }
                    }
                }
            }
        }
    }
    RowLayout {
        visible: !lab.choosing
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
        spacing: Theme.dp(14)
        Loader {
            active: visible; visible: lab.width >= 920 || lab.settingsOpen
            Layout.preferredWidth: lab.width >= 920 ? Math.min(380,Math.max(300,lab.width * .28)) : Math.min(360,lab.width * .72)
            Layout.fillHeight: true
            sourceComponent: Component { BalanceLabControls { backend: lab.backend; question: lab.question } }
        }
        Loader {
            active: visible; visible: lab.width >= 650 || !lab.settingsOpen
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0
            sourceComponent: Component { BalanceLabResults { backend: lab.backend; question: lab.question } }
        }
    }
    Popup {
        id: profiles
        visible: lab.profilesOpen
        parent: Overlay.overlay
        x: Math.max(12,(parent.width-width)/2); y: Math.max(12,(parent.height-height)/2)
        width: Math.min(parent.width-24,800); height: Math.min(parent.height-24,760)
        modal: true; focus: true; padding: Theme.dp(18)
        onClosed: lab.profilesOpen=false
        background: Rectangle { color: Theme.surface; radius: Theme.dp(10); border.color: Theme.divider }
        contentItem: BalanceLabProfiles { backend: lab.backend; onCloseRequested: profiles.close() }
    }
}
