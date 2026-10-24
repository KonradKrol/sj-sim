import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: order
    property var backend
    property bool moving: false
    readonly property bool cramped: height<260
    property string title: qsTr("Lista startowa")
    spacing: cramped ? 4 : 8
    RowLayout {
        Layout.fillWidth: true
        Label { text: dragArea.dragging ? dragArea.athleteName : qsTr("%1 · %2").arg(order.title).arg(order.backend.count); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; elide: Text.ElideRight }
        SButton { visible: order.backend.membership; objectName: "rosterMove"; text: qsTr("Przenieś"); implicitHeight: order.cramped ? 32 : 40; padding: order.cramped ? 4 : 6; quiet: true; enabled: order.backend.markedCount > 0; onClicked: order.moving=!order.moving }
    }
    Label { visible: !order.backend.membership; text: qsTr("Przeciągnij zawodnika na innego, aby zamienić miejsca."); color: Theme.muted; font.pointSize: Theme.caption; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    SField { objectName: "rosterOrderSearch"; Layout.fillWidth: true; Layout.minimumWidth: 0; text: order.backend.orderQuery; placeholderText: qsTr("Szukaj na liście"); Accessible.name: placeholderText; onTextEdited: order.backend.orderQuery=text }
    RowLayout {
        visible: order.backend.membership && order.moving && order.backend.markedCount > 0
        Layout.fillWidth: true
        Label { text: qsTr("Pozycja"); color: Theme.muted; font.pointSize: Theme.caption }
        SField { id: target; objectName: "rosterTargetPosition"; Layout.fillWidth: true; Layout.minimumWidth: 40; text: "1"; validator: IntValidator { bottom: 1; top: Math.max(1,order.backend.count+1) } Accessible.name: qsTr("Pozycja docelowa"); onAccepted: if(acceptableInput){order.backend.moveMarked(Number(text));order.moving=false} }
        SButton { objectName: "rosterMoveConfirm"; text: qsTr("Przenieś"); enabled: target.acceptableInput; onClicked: {order.backend.moveMarked(Number(target.text));order.moving=false} }
        SButton { text: "×"; quiet: true; Accessible.name: qsTr("Zamknij"); onClicked: order.moving=false }
    }
    Item {
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
    ListView {
        id: rows
        objectName: "rosterOrder"
        anchors.fill: parent
        model: order.backend.ordered
        spacing: 4; clip: true; activeFocusOnTab: true
        interactive: !dragArea.dragging
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar {}
        delegate: RosterRow {
            width: rows.width; backend: order.backend; ordered: true; viewport: rows
            athleteId: model.athleteId; athleteName: model.athleteName; countryCode: model.countryCode
            flagSource: model.flagSource || ""
            included: true; marked: model.marked; position: model.position; activeSlot: model.activeSlot
            onMoveRequested: if(order.backend.membership)order.moving=true
        }
        EmptyState { parent: rows; anchors.fill: parent; compact: parent.height<150; visible: rows.count===0; title: order.backend.count===0 ? qsTr("Lista startowa jest pusta") : qsTr("Brak pasujących zawodników"); description: order.backend.count===0 ? qsTr("Wybierz zawodników w panelu wyboru lub wczytaj zapisany skład.") : qsTr("Zmień filtry, aby zobaczyć zawodników z listy.") }
        Keys.onPressed: {
            if((event.modifiers & Qt.ControlModifier) && event.key === Qt.Key_A){order.backend.markAll();event.accepted=true}
            else if((event.modifiers & Qt.ControlModifier) && (event.key === Qt.Key_Up || event.key === Qt.Key_Down)){order.backend.step(event.key === Qt.Key_Up ? -1 : 1);event.accepted=true}
            else if(event.key === Qt.Key_Delete && order.backend.membership){order.backend.removeMarked();event.accepted=true}
        }
    }
    RosterDragArea { id: dragArea; anchors.fill: parent; backend: order.backend; viewport: rows; onMoveRequested: if(order.backend.membership)order.moving=true }
    }
    RowLayout {
        visible: !order.cramped || order.backend.markedCount>0 || order.backend.activeLimit>0
        Layout.fillWidth: true
        Label { text: order.backend.markedCount ? qsTr("Zaznaczono: %1").arg(order.backend.markedCount) : order.backend.activeLimit ? qsTr("W składzie: %1").arg(order.backend.activeLimit) : qsTr("Kolejność startu"); font.pointSize: Theme.caption; color: Theme.muted; Layout.fillWidth: true; elide: Text.ElideRight }
        SButton { visible: order.backend.membership && order.backend.markedCount > 0; text: qsTr("Usuń zaznaczonych"); onClicked: order.backend.removeMarked() }
    }
}
