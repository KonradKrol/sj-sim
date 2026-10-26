import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: perks
    property var backend
    property bool stacked: false
    readonly property var pairs: [qsTr("Podejście do startu"),qsTr("Profil skoczni"),qsTr("Regularność")]
    readonly property var definitions: [
        {pair:0,side:1,key:"training",name:qsTr("Błyszczy na treningach"),description:qsTr("Lepsze treningi i serie próbne. Słabsze konkursy."),tint:"#3564b5"},
        {pair:0,side:-1,key:"reserve",name:qsTr("Oszczędza siły"),description:qsTr("Słabsze, mniej równe treningi. Często nieco lepsze konkursy."),tint:"#397667"},
        {pair:1,side:1,key:"flight",name:qsTr("Lotnik"),description:qsTr("Zyskuje wraz z K skoczni. Traci na mniejszych obiektach."),tint:"#5867b5"},
        {pair:1,side:-1,key:"small",name:qsTr("Specjalista małych skoczni"),description:qsTr("Zyskuje na mniejszych K. Traci na większych obiektach."),tint:"#a25c29"},
        {pair:2,side:1,key:"steady",name:qsTr("Regularny"),description:qsTr("Mniejszy rozrzut jakości skoków."),tint:"#337d87"},
        {pair:2,side:-1,key:"volatile",name:qsTr("Nieprzewidywalny"),description:qsTr("Więcej wahań. Częściej psuje, czasem zaskakuje dobrym skokiem."),tint:"#9a4c70"}
    ]
    spacing: Theme.dp(10)
    RowLayout {
        Layout.fillWidth: true
        Label { text: qsTr("Perki"); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.heading }
        Item { Layout.fillWidth: true }
        Label { text: qsTr("%1 / 3 · po jednym z pary").arg((perks.backend.profile.perks || []).filter(function(v){return v!==0}).length); color: Theme.muted; font.pointSize: Theme.caption }
        HelpDot { explanation: qsTr("Kliknij perk, aby go wybrać lub wyłączyć. Przeciwny zostanie zastąpiony. Poziom II wzmacnia efekt. Na razie perki są zapisywane wyłącznie w nowym modelu.") }
    }
    Repeater {
        model: 3
        ColumnLayout {
            id: pairSection
            required property int index
            property int pairIndex: index
            Layout.fillWidth: true; spacing: Theme.dp(6)
            RowLayout {
                Layout.fillWidth: true; spacing: Theme.dp(8)
                Label { text: "0"+(pairSection.pairIndex+1); color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption }
                Label { text: perks.pairs[pairSection.pairIndex]; color: Theme.muted; font.pointSize: Theme.caption }
                Rectangle { Layout.fillWidth: true; height: 1; color: Theme.divider }
            }
            GridLayout {
                Layout.fillWidth: true; columns: perks.stacked || perks.width<Theme.dp(540) ? 1 : 2; columnSpacing: Theme.dp(9); rowSpacing: Theme.dp(7)
                Repeater {
                    model: 2
                    AthletePerkCard { required property int index; Layout.fillWidth: true; Layout.minimumWidth: 0; Layout.fillHeight: true; backend: perks.backend; perk: perks.definitions[pairSection.pairIndex*2+index]; dense: perks.stacked }
                }
            }
        }
    }
}
