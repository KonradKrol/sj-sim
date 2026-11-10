import QtQuick 2.15

QtObject {
    readonly property var pairs: [qsTr("Podejście"),qsTr("Skocznie"),qsTr("Regularność"),qsTr("Forma w sezonie")]
    readonly property var items: [
        {pair:0,side:1,key:"training",name:qsTr("Błyszczy na treningach"),description:qsTr("Pomaga na treningach i w seriach próbnych."),color:"#3e72cb"},
        {pair:0,side:-1,key:"reserve",name:qsTr("Oszczędza siły"),description:qsTr("Na treningach i w próbnych częściej odpuszcza."),color:"#29826b"},
        {pair:1,side:1,key:"flight",name:qsTr("Lotnik"),description:qsTr("Lepiej radzi sobie na dużych skoczniach."),color:"#7964c8"},
        {pair:1,side:-1,key:"small",name:qsTr("Specjalista małych skoczni"),description:qsTr("Lepiej radzi sobie na małych skoczniach."),color:"#9c5e2a"},
        {pair:2,side:1,key:"steady",name:qsTr("Regularny"),description:qsTr("Równiejsze skoki."),color:"#227785"},
        {pair:2,side:-1,key:"volatile",name:qsTr("Nieprzewidywalny"),description:qsTr("Częściej psuje skoki, czasem zaskakuje."),color:"#ad527d"},
        {pair:3,side:-1,key:"formStable",name:qsTr("Równa forma"),description:qsTr("Spokojniejsze okresy automatycznej formy w sezonie."),color:"#227785"},
        {pair:3,side:1,key:"formVolatile",name:qsTr("Chwiejna forma"),description:qsTr("Większe wahania automatycznej formy w sezonie."),color:"#ad527d"}
    ]
    function selected(pair,value){return items[pair*2+(pair===3?(value>0?1:0):(value<0?1:0))]}
}
