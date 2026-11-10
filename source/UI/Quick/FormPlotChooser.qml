import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

SButton {
    id: chooser
    property var backend
    text: qsTr("Na wykresie: %1 / 10 ▾").arg(backend.plottedIndexes.length)
    enabled: !backend.busy
    onClicked: popup.open()
    Popup {
        id: popup
        y: chooser.height
        width: Math.min(chooser.width,Theme.dp(330))
        height: Math.min(Theme.dp(340),chooser.parent.height*0.6)
        padding: Theme.dp(8)
        contentItem: ColumnLayout {
            spacing: Theme.dp(6)
            SField { id: search; Layout.fillWidth: true; placeholderText: qsTr("Szukaj zawodnika") }
            ListView {
                Layout.fillWidth: true; Layout.fillHeight: true; clip: true
                model: chooser.backend.plotNames
                ScrollBar.vertical: ScrollBar {}
                delegate: ItemDelegate {
                    id: entry
                    required property int index
                    required property string modelData
                    readonly property bool chosen: chooser.backend.plottedIndexes.indexOf(index)>=0
                    readonly property bool matches: modelData.toLocaleLowerCase().indexOf(search.text.toLocaleLowerCase())>=0
                    width: ListView.view.width
                    height: matches ? implicitHeight : 0
                    visible: matches
                    text: (chosen ? "✓  " : "    ")+modelData
                    highlighted: chosen
                    enabled: chosen || chooser.backend.plottedIndexes.length<10
                    onClicked: {
                        var selected=chooser.backend.plottedIndexes.slice()
                        var position=selected.indexOf(index)
                        if(position>=0)selected.splice(position,1);else selected.push(index)
                        chooser.backend.setPlottedAthletes(selected)
                    }
                }
            }
            SButton { text: qsTr("Wyczyść"); onClicked: chooser.backend.setPlottedAthletes([]) }
        }
    }
}
