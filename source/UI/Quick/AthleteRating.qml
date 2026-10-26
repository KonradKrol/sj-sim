import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: rating
    property var backend
    property bool form: false
    property bool compact: false
    readonly property int value: Number(backend.profile[form ? "form" : "overall"] || 70)
    readonly property color tint: form ? (Theme.dark ? "#8cdbc7" : "#146956") : Theme.signal
    padding: Theme.dp(compact ? 12 : 20)
    implicitHeight: contentItem.implicitHeight+topPadding+bottomPadding
    background: Rectangle { color: rating.form ? Theme.surface : Theme.stadium; radius: Theme.dp(12); border.color: rating.form ? Theme.divider : "#35536a" }
    contentItem: ColumnLayout {
        spacing: Theme.dp(8)
        RowLayout {
            Layout.fillWidth: true
            ColumnLayout {
                Layout.fillWidth: true; spacing: 1
                Label { text: rating.form ? qsTr("Forma") : "OVR"; color: rating.form ? Theme.ink : Theme.snow; font.family: Theme.displayFamily; font.pointSize: Theme.section }
                Label { text: rating.form ? qsTr("Aktualna dyspozycja") : qsTr("Ogólny poziom"); color: rating.form ? Theme.muted : Theme.headerMuted; font.pointSize: Theme.caption }
            }
            SField {
                id: number
                objectName: rating.form ? "athleteForm" : "athleteOverall"
                Layout.preferredWidth: Theme.dp(rating.compact ? 80 : 100)*Math.max(1,Theme.scale/Theme.uiScale)
                text: String(slider.pressed ? Math.round(slider.value) : rating.value)
                color: rating.form ? Theme.ink : Theme.signal
                font.family: Theme.displayFamily; font.pointSize: Theme.fontSize(rating.compact ? 34 : 48); font.weight: Font.Bold
                onActiveFocusChanged: if(!activeFocus && !acceptableInput)text=Qt.binding(function(){return String(rating.value)})
            horizontalAlignment: TextInput.AlignRight; inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator { bottom: 1; top: 100 }
                background: Rectangle { color: rating.form ? Theme.raised : "#203f53"; radius: Theme.dp(6); border.color: number.activeFocus ? Theme.accent : "transparent"; border.width: 2 }
                onEditingFinished: { if(acceptableInput)rating.backend.setValue(rating.form?"form":"overall",Number(text));text=Qt.binding(function(){return String(rating.value)}) }
            }
        }
        Slider {
            id: slider
            objectName: rating.form ? "athleteFormSlider" : "athleteOverallSlider"
            Layout.fillWidth: true
            from: 1; to: 100; stepSize: 1; value: rating.value
            Accessible.name: rating.form ? qsTr("Forma") : "OVR"
            onPressedChanged: if(!pressed && Math.round(value)!==rating.value)rating.backend.setValue(rating.form?"form":"overall",Math.round(value))
            Keys.onReleased: if(Math.round(value)!==rating.value)rating.backend.setValue(rating.form?"form":"overall",Math.round(value))
            background: Rectangle {
                x: slider.leftPadding; y: slider.topPadding+slider.availableHeight/2-height/2
                width: slider.availableWidth; height: Theme.dp(6); radius: height/2; color: rating.form ? Theme.divider : "#3b5667"
                Rectangle { width: slider.visualPosition*parent.width; height: parent.height; radius: parent.radius; color: rating.form ? rating.tint : Theme.signal }
            }
            handle: Rectangle { x: slider.leftPadding+slider.visualPosition*(slider.availableWidth-width); y: slider.topPadding+slider.availableHeight/2-height/2; width: Theme.dp(18); height: width; radius: width/2; color: rating.form ? rating.tint : Theme.signal; border.width: slider.activeFocus ? 3 : 2; border.color: slider.activeFocus ? Theme.accent : rating.form ? Theme.surface : Theme.stadium }
        }
        RowLayout { Layout.fillWidth: true; Label { text: "1"; color: rating.form ? Theme.muted : Theme.headerMuted; font.family: Theme.mono; font.pointSize: Theme.caption } Item { Layout.fillWidth: true } Label { text: "100"; color: rating.form ? Theme.muted : Theme.headerMuted; font.family: Theme.mono; font.pointSize: Theme.caption } }
    }
}
