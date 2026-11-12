import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Popup {
    id: welcome
    objectName: "homeWelcome"
    property int step: 0
    readonly property var pages: [
        {title:qsTr("Witaj w Sj.Simie!"),image:"overview",text:qsTr("Korzystasz właśnie z symulatora stworzonego dla pasjonatów skoków narciarskich. Pozwoli Ci on przeprowadzać własne symulacje światów na swoich warunkach. Zacznij od aktualnego sezonu, przypomnij sobie czasy Małyszomanii albo zaprojektuj fikcyjne uniwersum!")},
        {title:qsTr("Twórz własny świat"),image:"athletes",text:qsTr("Możesz stworzyć własne zasoby lub skorzystać z istniejących. Na <a href=\"https://discord.gg/NrusAMJgSw\">serwerze Discord</a> znajdują się paczki stworzone przez społeczność.")+"<br><br>"+qsTr("Śmiało eksperymentuj z atrybutami skoczków i skoczni. Niektórzy zawodnicy świetnie radzą sobie na treningach, ale „spalają się” w konkursie. Inni są urodzonymi specjalistami od skoczni mniejszych. Za to skocznie takie jak Willingen są dość „wietrzne” i zarazem sprzyjają lotnikom. Sj.Sim to wszystko wspiera!")},
        {title:qsTr("Kieruj konkursami"),image:"competition",text:qsTr("Podczas zawodów masz wpływ niemal na wszystko, począwszy od zasad i kolejności startowej, a kończąc na warunkach wietrznych i belce. Jeśli chcesz, możesz z góry przesądzić o wyniku wybranych zawodników ;)")+"<br><br>"+qsTr("Dostępne są zarówno konkursy indywidualne, jak i drużynówki czy popularny format KO. Możesz tworzyć złożone kombinacje konkursów wieloseryjnych, z wyzerowaniem not punktowych czy podziałem na grupy według różnych kryteriów.")},
        {title:qsTr("Rozgrywaj pełne sezony"),image:"calendar",text:qsTr("Układaj kalendarze, ustalaj zasady kwalifikacji i prowadź własne klasyfikacje. Łącz letnie i zimowe zawody, planuj weekendy i wybieraj zawodników na kolejne konkursy.")+"<br><br>"+qsTr("Śledź wyniki w archiwum i obserwuj zmiany formy w sezonach z datami zawodów. Na początek przygotowaliśmy przykładową symulację z Letnim Grand Prix 2026 i Pucharem Świata 2026/2027.")}
    ]
    parent: Overlay.overlay
    visible: ui.context.showWelcome===true
    onOpened: step=0
    modal: true; focus: true; closePolicy: Popup.NoAutoClose
    width: Math.min(Theme.dp(752),parent.width-Theme.dp(24))
    height: Math.min(Theme.dp(384),parent.height-Theme.dp(24))
    x: (parent.width-width)/2; y: (parent.height-height)/2
    padding: Theme.dp(20)
    enter: PopupFade {}
    exit: PopupFade { leaving: true }
    background: Surface { fillColor: Theme.surface; edgeColor: Theme.divider; strokeWidth: 1; sweep: 28 }
    contentItem: ColumnLayout {
        spacing: Theme.dp(12)
        RowLayout {
            Layout.fillWidth: true
            Repeater {
                model: 4
                Rectangle { required property int index; Layout.preferredWidth: Theme.dp(index===welcome.step?36:12); Layout.preferredHeight: Theme.dp(5); radius: height/2; color: index===welcome.step?Theme.accent:Theme.divider }
            }
            Item { Layout.fillWidth: true }
            Label { text: (welcome.step+1)+" / 4"; color: Theme.muted; font.family: Theme.mono; font.pointSize: Theme.caption }
        }
        GridLayout {
            id: body
            Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
            columns: welcome.width>=Theme.dp(700)?2:1
            columnSpacing: Theme.dp(20); rowSpacing: Theme.dp(12)
            Rectangle {
                objectName: "welcomeImageFrame"
                Layout.fillWidth: body.columns===1; Layout.alignment: Qt.AlignVCenter; Layout.minimumWidth: 0
                Layout.preferredWidth: Theme.dp(351); Layout.preferredHeight: body.columns===1?Theme.dp(100):Math.min(Theme.dp(200),body.height)
                color: Theme.raised; radius: Theme.radius; clip: true; border.color: Theme.divider
                Image { objectName: "welcomeImage"; anchors.fill: parent; anchors.margins: Theme.dp(5); source: "assets/welcome/"+welcome.pages[welcome.step].image+".png"; fillMode: Image.PreserveAspectCrop; smooth: true; mipmap: true; asynchronous: true; Accessible.name: welcome.pages[welcome.step].title }
            }
            ScrollView {
                id: description
                Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumWidth: 0; Layout.minimumHeight: Theme.dp(100); Layout.preferredWidth: Theme.dp(600)
                contentWidth: availableWidth; clip: true
                Column {
                    width: description.availableWidth; spacing: Theme.dp(12)
                    Label { text: welcome.pages[welcome.step].title; width: parent.width; wrapMode: Text.WordWrap; font.family: Theme.displayFamily; font.pointSize: Theme.heading; font.bold: true; color: Theme.ink }
                    Label { id: descriptionText; text: welcome.pages[welcome.step].text; width: parent.width; wrapMode: Text.WordWrap; textFormat: Text.RichText; font.pointSize: Theme.body; lineHeight: 1.2; color: Theme.ink; linkColor: Theme.accent; onLinkActivated: Qt.openUrlExternally(link); HoverHandler { cursorShape: descriptionText.hoveredLink.length>0 ? Qt.PointingHandCursor : Qt.ArrowCursor } }
                }
            }
        }
        Rectangle { Layout.fillWidth: true; implicitHeight: 1; color: Theme.divider }
        RowLayout {
            Layout.fillWidth: true; spacing: Theme.dp(8)
            SButton { visible: welcome.step>0; text: qsTr("Wstecz"); quiet: true; onClicked: {welcome.step--;description.contentItem.contentY=0} }
            SButton { visible: welcome.step===0; text: qsTr("O autorze"); quiet: true; onClicked: ui.launch("author") }
            Item { Layout.fillWidth: true }
            SButton {
                objectName: "homeWelcomeSecondary"
                text: welcome.step===3 ? (ui.context.exampleBusy ? qsTr("Przygotowywanie…") : qsTr("Rozpocznij przykładowy sezon")) : qsTr("Przejdź do menu")
                quiet: true; enabled: !ui.context.exampleBusy
                onClicked: ui.launch(welcome.step===3 ? "example" : "dismissWelcome")
            }
            SButton {
                objectName: "homeWelcomeNext"
                text: welcome.step===3 ? qsTr("Ok") : qsTr("Dalej")
                primary: true; enabled: !ui.context.exampleBusy
                onClicked: {if(welcome.step===3)ui.launch("dismissWelcome");else{welcome.step++;description.contentItem.contentY=0}}
            }
        }
    }
}
