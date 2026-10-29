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
    readonly property real numberSize: Theme.fontSize(form ? (compact ? 34 : 44) : (compact ? (design.variant===2 ? 42 : design.variant===3 ? 50 : 60) : design.variant===0 ? 100 : design.variant===2 ? 72 : 84))
    readonly property TextMetrics numberMetrics: TextMetrics { font: number.font; text: "100" }

    spacing: Theme.dp(4)
    RowLayout {
        Layout.fillWidth: true; spacing: Theme.dp(10)
        Label { text: rating.form ? qsTr("Forma") : "OVR"; color: rating.design.muted; font.family: rating.design.detailFont; font.pointSize: Theme.body }
        Item { Layout.fillWidth: true }
        Label { visible: !rating.compact; text: "/ 100"; color: rating.design.muted; font.family: Theme.mono; font.pointSize: Theme.caption }
    }
    readonly property int displayedValue: slider.pressed ? Math.round(slider.value) : value
    readonly property int effectiveValue: number.activeFocus && number.acceptableInput ? Number(number.text) : value
    function step(delta) {
        backend.setValue(form?"form":"overall",Math.max(1,Math.min(100,effectiveValue+delta)))
        number.text=Qt.binding(function(){return String(rating.displayedValue)})
    }
    RowLayout {
        Layout.fillWidth: true; spacing: Theme.dp(3)
        TextField {
        id: number
        objectName: rating.form ? "athleteForm" : "athleteOverall"
        Layout.fillWidth: false; Layout.preferredWidth: Math.ceil(Math.max(rating.numberMetrics.advanceWidth,rating.numberMetrics.width))+Theme.dp(8); Layout.minimumWidth: Layout.preferredWidth
        implicitHeight: contentHeight+Theme.dp(2)
        topPadding: 0; bottomPadding: 0; leftPadding: 0; rightPadding: 0
        text: String(rating.displayedValue)
        color: rating.form ? rating.design.ink : rating.design.accent
        font.family: rating.design.variant===2 ? Theme.mono : Theme.displayFamily
        font.pointSize: rating.numberSize; font.weight: Font.Black
        hoverEnabled: true
        selectByMouse: true; selectionColor: rating.design.accent; selectedTextColor: rating.design.accentInk
        inputMethodHints: Qt.ImhDigitsOnly
        Accessible.name: rating.form ? qsTr("Forma") : "OVR"
        validator: IntValidator { bottom: 1; top: 100 }
        background: Rectangle { color: number.hovered ? rating.design.surface : "transparent"; border.width: number.activeFocus ? 2 : 0; border.color: rating.design.accent; radius: Theme.dp(3) }
        onActiveFocusChanged: if(!activeFocus && !acceptableInput)text=Qt.binding(function(){return String(rating.displayedValue)})
        onEditingFinished: {if(acceptableInput)rating.backend.setValue(rating.form?"form":"overall",Number(text));text=Qt.binding(function(){return String(rating.displayedValue)})}
    }
        Column {
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: Theme.dp(24)
            spacing: Theme.dp(1)
            SButton {
                objectName: rating.form ? "athleteFormIncrease" : "athleteOverallIncrease"
                width: Theme.dp(24); height: Math.max(Theme.dp(22),Theme.captionMetrics.height+2)
                text: "+"; quiet: true; padding: 0; foreground: rating.design.muted; font.pointSize: Theme.caption
                autoRepeat: true; autoRepeatDelay: 250; autoRepeatInterval: 45
                enabled: rating.effectiveValue<100
                Accessible.name: rating.form ? qsTr("Zwiększ formę") : qsTr("Zwiększ OVR")
                hint: Accessible.name
                onPressed: rating.step(1)
            }
            SButton {
                objectName: rating.form ? "athleteFormDecrease" : "athleteOverallDecrease"
                width: Theme.dp(24); height: Math.max(Theme.dp(22),Theme.captionMetrics.height+2)
                text: "−"; quiet: true; padding: 0; foreground: rating.design.muted; font.pointSize: Theme.caption
                autoRepeat: true; autoRepeatDelay: 250; autoRepeatInterval: 45
                enabled: rating.effectiveValue>1
                Accessible.name: rating.form ? qsTr("Zmniejsz formę") : qsTr("Zmniejsz OVR")
                hint: Accessible.name
                onPressed: rating.step(-1)
            }
        }
        Item { Layout.fillWidth: true }
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
