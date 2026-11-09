import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Popup {
    enter: PopupFade {}
    exit: PopupFade { leaving: true }
    id: popup
    width: 300
    height: Math.min(implicitHeight,parent ? parent.height : implicitHeight)
    padding: 20
    modal: false
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    background: Rectangle { color: Theme.surface; border.color: Theme.outline; radius: Theme.radius }
    contentItem: ScrollView {
        id: scroll
        contentWidth: availableWidth
        implicitHeight: settings.implicitHeight
        clip: true
        ColumnLayout {
        id: settings
        width: scroll.availableWidth
        spacing: 12
        Label { text: qsTr("Wygląd i dostępność"); font.pointSize: Theme.body; font.weight: Font.DemiBold; color: Theme.ink; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Label { text: qsTranslate("QuickBridge","Język interfejsu"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        SChoice {
            id: languageChoice
            objectName: "appearanceLanguage"
            model: ["Polski","English","Čeština"]
            currentIndex: navigation.languageIndex
            Layout.fillWidth: true
            Accessible.name: qsTranslate("QuickBridge","Język interfejsu")
            onActivated: {languageError.visible=!navigation.setLanguage(index);if(!languageError.visible && Theme.motion)languageFeedback.restart()}
            SequentialAnimation {
                id: languageFeedback
                NumberAnimation { target: languageChoice; property: "opacity"; from: 0.5; to: 1; duration: Theme.motion*2; easing.type: Easing.OutCubic }
            }
            Connections { target: Theme; function onMotionChanged(){if(!Theme.motion){languageFeedback.stop();languageChoice.opacity=1}} }
        }
        Label { id: languageError; visible: false; text: qsTranslate("AppSettingsWindow","Nie udało się wczytać pliku tłumaczenia."); color: Theme.danger; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        Label { text: qsTr("Motyw kolorystyczny"); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true }
        Repeater {
            model: [qsTr("Aktualny"),qsTr("Różowy"),qsTr("Czysty")]
            ItemDelegate {
                id: themeChoice
                required property int index
                required property string modelData
                readonly property var preview: Theme.colorsFor(index,Theme.dark)
                Layout.fillWidth: true
                objectName: "appearanceColorTheme-"+index
                implicitHeight: Math.max(Theme.dp(36),contentItem.implicitHeight+Theme.dp(12))
                padding: Theme.dp(8)
                highlighted: Theme.colorTheme===index
                Accessible.name: modelData
                Accessible.role: Accessible.RadioButton
                Accessible.checked: highlighted
                onClicked: appearance.colorTheme=index
                background: Rectangle {
                    radius: Theme.dp(6)
                    color: themeChoice.highlighted ? Theme.selection : themeChoice.hovered ? Theme.raised : Theme.surface
                    border.color: themeChoice.highlighted || themeChoice.activeFocus ? Theme.accent : Theme.outline
                    border.width: themeChoice.highlighted || themeChoice.activeFocus ? 2 : 1
                }
                contentItem: RowLayout {
                    spacing: Theme.dp(8)
                    Label { text: themeChoice.modelData; color: Theme.ink; font.pointSize: Theme.body; font.bold: themeChoice.highlighted; Layout.fillWidth: true; Layout.minimumWidth: 0; elide: Text.ElideRight }
                    Row {
                        spacing: Theme.dp(3)
                        Repeater {
                            model: [themeChoice.preview.stadium,themeChoice.preview.accent,themeChoice.preview.signal]
                            Rectangle { required property string modelData; width: Theme.dp(15); height: width; radius: width/2; color: modelData; Accessible.ignored: true }
                        }
                    }
                    Label { text: themeChoice.highlighted ? "✓" : ""; color: Theme.accent; font.bold: true; font.pointSize: Theme.body; Layout.preferredWidth: Theme.dp(16); Accessible.ignored: true }
                }
            }
        }
        SCheck { text: qsTr("Ciemny motyw"); checked: appearance && appearance.dark; onClicked: appearance.dark = checked; Layout.fillWidth: true }
        SCheck { text: qsTr("Ogranicz animacje"); checked: appearance && appearance.reducedMotion; onClicked: appearance.reducedMotion = checked; Layout.fillWidth: true }
        Label { text: qsTr("Skala interfejsu (%)"); color: Theme.muted; font.pointSize: Theme.caption }
        SpinBox {
            objectName: "appearanceUiScale"; from: 50; to: 150; stepSize: 5; editable: true
            value: Math.round(appearance.uiScale*100); font.pointSize: Theme.body; Layout.fillWidth: true
            textFromValue: function(value,locale){return value.toString()}
            valueFromText: function(text,locale){return Number(text.replace("%", "").trim())}
            validator: IntValidator { bottom: 50; top: 150 }
            onValueModified: appearance.uiScale=value/100
            Accessible.name: qsTr("Skala interfejsu")
        }
        Label { text: qsTr("Rozmiar tekstu (%)"); color: Theme.muted; font.pointSize: Theme.caption }
        SpinBox {
            objectName: "appearanceTextSize"; from: 75; to: 200; stepSize: 5; editable: true
            value: Math.round(appearance.textScale*100); font.pointSize: Theme.body; Layout.fillWidth: true
            textFromValue: function(value,locale){return value.toString()}
            valueFromText: function(text,locale){return Number(text.replace("%", "").trim())}
            validator: IntValidator { bottom: 75; top: 200 }
            onValueModified: appearance.textScale=value/100
            Accessible.name: qsTr("Rozmiar tekstu")
        }
        SButton { objectName: "appearanceDone"; text: qsTr("Gotowe"); onClicked: popup.close(); Layout.fillWidth: true }
        }
    }
}
