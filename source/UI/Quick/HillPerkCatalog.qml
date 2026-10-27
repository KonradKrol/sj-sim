import QtQuick 2.15

QtObject {
    readonly property var pairs: [qsTr("Charakter skoczni"),qsTr("Lądowanie za HS"),qsTr("Zmienność wiatru"),qsTr("Dominujący kierunek")]
    readonly property var items: [
        {pair:0,side:1,key:"takeoff",name:qsTr("Premiuje wybicie"),description:qsTr("Premiuje mocne wybicie bardziej niż typowa skocznia o tym punkcie K."),color:"#35816b"},
        {pair:0,side:-1,key:"flying",name:qsTr("Lotna skocznia"),description:qsTr("Bardziej nagradza lot niż typowa skocznia o tym punkcie K."),color:"#7964c8"},
        {pair:1,side:1,key:"forgiving",name:qsTr("Przyjazny zeskok"),description:qsTr("Przesuwa próg trudnego lądowania dalej za HS. Ułatwia ustanie dalekich skoków."),color:"#29826b"},
        {pair:1,side:-1,key:"demanding",name:qsTr("Utrudnione lądowanie"),description:qsTr("Próg trudnego lądowania pojawia się wcześniej. Nawet okolice HS wymagają ostrożności."),color:"#ad527d"},
        {pair:2,side:1,key:"lottery",name:qsTr("Wietrzna aura"),description:qsTr("Gra częściej proponuje loteryjne, gwałtownie zmieniające się warunki. Nie zmienia ręcznych ustawień pogody."),color:"#9c5e2a"},
        {pair:2,side:-1,key:"stable",name:qsTr("Spokojna pogoda"),description:qsTr("Gra częściej proponuje równy, stabilny wiatr. Nie zmienia ręcznych ustawień pogody."),color:"#227785"},
        {pair:3,side:1,key:"headwind",name:qsTr("Dominuje wiatr przedni"),description:qsTr("Przy losowaniu warunków częściej pojawia się wiatr pod narty. Ręcznie wybrany kierunek pozostaje bez zmian."),color:"#29826b"},
        {pair:3,side:-1,key:"tailwind",name:qsTr("Dominuje wiatr tylny"),description:qsTr("Przy losowaniu warunków częściej pojawia się wiatr w plecy. Ręcznie wybrany kierunek pozostaje bez zmian."),color:"#ad527d"}
    ]
}
