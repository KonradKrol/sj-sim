import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: rating
    property var backend
    property var design
    property bool form: false
    property bool compact: false
    readonly property int value: Number(backend.profile[form ? "form" : "overall"] || 70)
    readonly property real numberSize: Theme.fontSize(form ? (compact ? 36 : 48) : (compact ? 68 : design.variant===0 ? 116 : design.variant===1 ? 100 : 86))
    spacing: Theme.dp(4)
    RowLayout {
        Layout.fillWidth: true; spacing: Theme.dp(10)
        Label { text: rating.form ? qsTr("Forma") : "OVR"; color: rating.design.muted; font.family: rating.design.detailFont; font.pointSize: Theme.body }
        Item { Layout.fillWidth: true }
        Label { visible: !rating.compact; text: "/ 100"; color: rating.design.muted; font.family: Theme.mono; font.pointSize: Theme.caption }
    }
    TextField {
        id: number
        objectName: rating.form ? "athleteForm" : "athleteOverall"
        Layout.fillWidth: true
        implicitHeight: contentHeight+Theme.dp(2)
        topPadding: 0; bottomPadding: 0; leftPadding: 0; rightPadding: 0
        text: String(slider.pressed ? Math.round(slider.value) : rating.value)
        color: rating.form ? rating.design.ink : rating.design.accent
        font.family: rating.design.variant===2 ? Theme.mono : Theme.displayFamily
        font.pointSize: Math.min(rating.numberSize,rating.width/(rating.design.variant===2 ? 2.6 : 2)); font.weight: Font.Black
        hoverEnabled: true
        selectByMouse: true; selectionColor: rating.design.accent; selectedTextColor: rating.design.accentInk
        inputMethodHints: Qt.ImhDigitsOnly
        Accessible.name: rating.form ? qsTr("Forma") : "OVR"
        validator: IntValidator { bottom: 1; top: 100 }
        background: Rectangle { color: number.hovered ? rating.design.surface : "transparent"; border.width: number.activeFocus ? 2 : 0; border.color: rating.design.accent; radius: Theme.dp(3) }
        onActiveFocusChanged: if(!activeFocus && !acceptableInput)text=Qt.binding(function(){return String(rating.value)})
        onEditingFinished: {if(acceptableInput)rating.backend.setValue(rating.form?"form":"overall",Number(text));text=Qt.binding(function(){return String(rating.value)})}
    }
    Slider {
        id: slider
        objectName: rating.form ? "athleteFormSlider" : "athleteOverallSlider"
        Layout.fillWidth: true; implicitHeight: Theme.dp(27)
        from: 1; to: 100; stepSize: 1; value: rating.value
        Accessible.name: rating.form ? qsTr("Forma") : "OVR"
        onPressedChanged: if(!pressed && Math.round(value)!==rating.value)rating.backend.setValue(rating.form?"form":"overall",Math.round(value))
        Keys.onReleased: if(Math.round(value)!==rating.value)rating.backend.setValue(rating.form?"form":"overall",Math.round(value))
        background: Rectangle {
            x: slider.leftPadding; y: slider.topPadding+slider.availableHeight/2-height/2
            width: slider.availableWidth; height: Theme.dp(rating.design.variant===2 ? 9 : 3); radius: rating.design.variant===2 ? height/2 : 0; color: rating.design.line
            Rectangle { width: slider.visualPosition*parent.width; height: parent.height; radius: parent.radius; color: rating.design.accent }
        }
        handle: Rectangle { x: slider.leftPadding+slider.visualPosition*(slider.availableWidth-width); y: slider.topPadding+slider.availableHeight/2-height/2; width: Theme.dp(15); height: width; radius: rating.design.variant===2 || rating.design.variant===4 ? width/2 : 2; color: rating.design.accent; border.width: slider.activeFocus ? 3 : 1; border.color: slider.activeFocus ? rating.design.ink : rating.design.paper }
    }
}
