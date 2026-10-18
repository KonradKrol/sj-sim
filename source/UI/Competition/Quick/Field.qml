import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Column {
    id: field
    property var descriptor
    spacing: 8
    enabled: descriptor.enabled
    opacity: enabled ? 1 : 0.45
    Label {
        width: parent.width
        text: field.descriptor.label
        color: "#646963"
        font.pixelSize: 13
        wrapMode: Text.WordWrap
        visible: field.descriptor.kind !== "check"
    }
    Loader {
        width: parent.width
        height: item ? item.implicitHeight : 42
        sourceComponent: field.descriptor.kind === "number" ? numberInput :
                         field.descriptor.kind === "choice" ? choiceInput :
                         field.descriptor.kind === "check" ? checkInput : textInput
    }
    Component {
        id: textInput
        TextField {
            id: textEdit
            objectName: "quick-" + field.descriptor.label
            implicitHeight: 42
            leftPadding: 12; rightPadding: 12
            text: field.descriptor.value
            maximumLength: field.descriptor.maxLength
            selectByMouse: true
            Accessible.name: field.descriptor.label
            onEditingFinished: if (text !== field.descriptor.value) competition.setField(field.descriptor.id, text)
            background: Rectangle { radius: 9; color: "#f4f5f2"; border.width: 1; border.color: textEdit.activeFocus ? "#2e55c5" : "#f4f5f2"; Behavior on border.color { ColorAnimation { duration: 120 } } }
        }
    }
    Component {
        id: numberInput
        Item {
            implicitHeight: 42
            function step(direction) {
                var v = Number(input.text.replace(",", "."))
                if (!isFinite(v)) v = field.descriptor.value
                competition.setField(field.descriptor.id, Math.max(field.descriptor.minimum,
                    Math.min(field.descriptor.maximum, v + direction * field.descriptor.step)))
            }
            Rectangle { anchors.fill: parent; radius: 9; color: "#f4f5f2"; border.color: input.activeFocus ? "#2e55c5" : color; Behavior on border.color { ColorAnimation { duration: 120 } } }
            TextField {
                id: input
                objectName: "quick-" + field.descriptor.label
                anchors.left: parent.left; anchors.right: steppers.left
                height: parent.height
                leftPadding: 12
                background: Item {}
                text: Number(field.descriptor.value).toFixed(field.descriptor.decimals)
                selectByMouse: true
                Accessible.name: field.descriptor.label
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                validator: DoubleValidator {
                    bottom: field.descriptor.minimum
                    top: field.descriptor.maximum
                    decimals: field.descriptor.decimals
                    notation: DoubleValidator.StandardNotation
                    locale: "en_US"
                }
                onEditingFinished: {
                    if (acceptableInput) {
                        var v = Number(text)
                        if (v !== field.descriptor.value) competition.setField(field.descriptor.id, v)
                    } else text = Qt.binding(function() { return Number(field.descriptor.value).toFixed(field.descriptor.decimals) })
                }
                ToolTip.visible: activeFocus
                ToolTip.text: field.descriptor.minimum + " … " + field.descriptor.maximum
            }
            Row {
                id: steppers
                anchors.right: parent.right; anchors.rightMargin: 5
                anchors.verticalCenter: parent.verticalCenter
                spacing: 2
                Repeater {
                    model: ["−", "+"]
                    delegate: ToolButton {
                        width: 30; height: 30
                        text: modelData
                        font.pixelSize: 18
                        Accessible.name: (index === 0 ? qsTr("Zmniejsz: ") : qsTr("Zwiększ: ")) + field.descriptor.label
                        onClicked: steppers.parent.step(index === 0 ? -1 : 1)
                        background: Rectangle { radius: 7; color: parent.hovered ? "#e1e4de" : "transparent"; border.color: parent.activeFocus ? "#2e55c5" : "transparent"; Behavior on color { ColorAnimation { duration: 100 } } }
                    }
                }
            }
        }
    }
    Component {
        id: choiceInput
        ComboBox {
            id: choice
            objectName: "quick-" + field.descriptor.label
            implicitHeight: 42
            model: field.descriptor.choices
            currentIndex: field.descriptor.value
            Accessible.name: field.descriptor.label
            onActivated: competition.setField(field.descriptor.id, currentIndex)
            contentItem: Text {
                leftPadding: 12; rightPadding: 32
                text: choice.displayText
                font: choice.font
                color: "#292e2a"
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
            background: Rectangle { radius: 9; color: choice.hovered ? "#eceee9" : "#f4f5f2"; border.color: choice.activeFocus ? "#2e55c5" : color; Behavior on color { ColorAnimation { duration: 120 } } }
            indicator: Canvas {
                x: choice.width - 26; y: (choice.height - height) / 2
                width: 12; height: 8
                rotation: choice.popup.visible ? 180 : 0
                Behavior on rotation { NumberAnimation { duration: 130 } }
                onPaint: { var c = getContext("2d"); c.strokeStyle = "#6a7069"; c.lineWidth = 1.5; c.beginPath(); c.moveTo(1, 1); c.lineTo(6, 6); c.lineTo(11, 1); c.stroke() }
            }
            // The default Qt Quick style sets Popup.height directly and leaves
            // Popup.implicitHeight at zero. Size from its list content instead.
            popup.height: Math.min(340, popup.contentItem.implicitHeight + popup.topPadding + popup.bottomPadding)
        }
    }
    Component {
        id: checkInput
        Switch {
            id: toggle
            implicitHeight: 42
            text: field.descriptor.label
            checked: field.descriptor.value
            Accessible.name: text
            onClicked: competition.setField(field.descriptor.id, checked)
            indicator: Rectangle {
                x: toggle.leftPadding; y: (toggle.height - height) / 2
                width: 34; height: 21; radius: 11
                color: toggle.checked ? "#2e55c5" : "#d8ddd4"
                border.color: toggle.activeFocus ? "#163da9" : color
                Behavior on color { ColorAnimation { duration: 140 } }
                Rectangle {
                    x: toggle.checked ? 16 : 3; y: 3
                    width: 15; height: 15; radius: 8; color: "white"
                    Behavior on x { NumberAnimation { duration: 140; easing.type: Easing.OutCubic } }
                }
            }
            contentItem: Text {
                text: toggle.text; font: toggle.font; color: "#363c35"
                leftPadding: toggle.indicator.width + 12
                verticalAlignment: Text.AlignVCenter
                wrapMode: Text.WordWrap
            }
        }
    }
}
