import QtQuick 2.15

QtObject {
    readonly property var pairs: [qsTr("Podejście"),qsTr("Skocznie"),qsTr("Regularność")]
    readonly property var items: [
        {pair:0,side:1,key:"training",name:qsTr("Błyszczy na treningach"),description:qsTr("Lepsze treningi i serie próbne. Słabsze konkursy."),color:"#3e72cb"},
        {pair:0,side:-1,key:"reserve",name:qsTr("Oszczędza siły"),description:qsTr("Słabsze i mniej równe treningi. W konkursach często nieco lepiej."),color:"#29826b"},
        {pair:1,side:1,key:"flight",name:qsTr("Lotnik"),description:qsTr("Zyskuje wraz z K skoczni. Traci na mniejszych obiektach."),color:"#7964c8"},
        {pair:1,side:-1,key:"small",name:qsTr("Specjalista małych skoczni"),description:qsTr("Zyskuje na mniejszych K. Traci na większych obiektach."),color:"#9c5e2a"},
        {pair:2,side:1,key:"steady",name:qsTr("Regularny"),description:qsTr("Mniejszy rozrzut jakości skoków."),color:"#227785"},
        {pair:2,side:-1,key:"volatile",name:qsTr("Nieprzewidywalny"),description:qsTr("Więcej wahań. Częściej psuje, czasem zaskakuje dobrym skokiem."),color:"#ad527d"}
    ]
    function selected(pair,value){return items[pair*2+(value<0?1:0)]}
}
