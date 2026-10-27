import QtQuick

QtObject {
    readonly property var pairs: [qsTr("Charakter skoczni"),qsTr("Lądowanie za HS"),qsTr("Wiatr")]
    readonly property var items: [
        {pair:0,side:1,key:"takeoff",name:qsTr("Premiuje wybicie"),description:qsTr("Premiuje mocne wybicie bardziej niż typowa skocznia o tym punkcie K."),color:"#35816b"},
        {pair:0,side:-1,key:"flying",name:qsTr("Lotna skocznia"),description:qsTr("Bardziej nagradza lot niż typowa skocznia o tym punkcie K."),color:"#7964c8"},
        {pair:1,side:1,key:"forgiving",name:qsTr("Przyjazny zeskok"),description:qsTr("Przesuwa próg trudnego lądowania dalej za HS. Ułatwia ustanie dalekich skoków."),color:"#29826b"},
        {pair:1,side:-1,key:"demanding",name:qsTr("Utrudnione lądowanie"),description:qsTr("Próg trudnego lądowania pojawia się wcześniej. Nawet okolice HS wymagają ostrożności."),color:"#ad527d"},
        {pair:2,side:1,key:"lottery",name:qsTr("Wietrzna aura"),description:qsTr("Sprzyja częstszym i mocniejszym wahaniom wiatru podczas konkursu."),color:"#9c5e2a"},
        {pair:2,side:-1,key:"stable",name:qsTr("Spokojna pogoda"),description:qsTr("Sprzyja równiejszym warunkom i mniejszym wahaniom wiatru."),color:"#227785"}
    ]
}
