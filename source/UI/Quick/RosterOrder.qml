import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: order
    property var backend
    property bool moving: false
    property string title: qsTr("Lista startowa")
    spacing: 8
    RowLayout {
        Layout.fillWidth: true
        Label { text: qsTr("%1 · %2").arg(order.title).arg(order.backend.count); color: Theme.ink; font.family: Theme.displayFamily; font.pointSize: Theme.section; Layout.fillWidth: true; elide: Text.ElideRight }
        SButton { text: qsTr("Przenieś"); quiet: true; enabled: order.backend.markedCount > 0; onClicked: order.moving=!order.moving }
    }
    SField { objectName: "rosterOrderSearch"; Layout.fillWidth: true; Layout.minimumWidth: 0; text: order.backend.orderQuery; placeholderText: qsTr("Szukaj na liście"); Accessible.name: placeholderText; onTextEdited: order.backend.orderQuery=text }
    RowLayout {
        visible: order.moving && order.backend.markedCount > 0
        Layout.fillWidth: true
        Label { text: qsTr("Pozycja"); color: Theme.muted; font.pointSize: Theme.caption }
        SField { id: target; objectName: "rosterTargetPosition"; Layout.fillWidth: true; Layout.minimumWidth: 40; text: "1"; validator: IntValidator { bottom: 1; top: Math.max(1,order.backend.count+1) } Accessible.name: qsTr("Pozycja docelowa"); onAccepted: if(acceptableInput){order.backend.moveMarked(Number(text));order.moving=false} }
        SButton { objectName: "rosterMoveConfirm"; text: qsTr("Przenieś"); enabled: target.acceptableInput; onClicked: {order.backend.moveMarked(Number(target.text));order.moving=false} }
        SButton { text: "×"; quiet: true; Accessible.name: qsTr("Zamknij"); onClicked: order.moving=false }
    }
    ListView {
        id: rows
        objectName: "rosterOrder"
        Layout.fillWidth: true; Layout.fillHeight: true; Layout.minimumHeight: 0
        model: order.backend.ordered
        spacing: 4; clip: true; activeFocusOnTab: true
        ScrollBar.vertical: ScrollBar {}
        delegate: RosterRow {
            width: rows.width; backend: order.backend; ordered: true; viewport: rows
            athleteId: model.athleteId; athleteName: model.athleteName; countryCode: model.countryCode
            included: true; marked: model.marked; position: model.position; activeSlot: model.activeSlot
            onMoveRequested: order.moving=true
        }
        Label { visible: rows.count === 0; anchors.centerIn: parent; width: parent.width; text: order.backend.count === 0 ? qsTr("Wybierz zawodników do listy") : qsTr("Brak pasujących zawodników"); wrapMode: Text.WordWrap; color: Theme.muted; font.pointSize: Theme.body }
        Keys.onPressed: {
            if((event.modifiers & Qt.ControlModifier) && event.key === Qt.Key_A){order.backend.markAll();event.accepted=true}
            else if((event.modifiers & Qt.ControlModifier) && (event.key === Qt.Key_Up || event.key === Qt.Key_Down)){order.backend.step(event.key === Qt.Key_Up ? -1 : 1);event.accepted=true}
            else if(event.key === Qt.Key_Delete && order.backend.membership){order.backend.removeMarked();event.accepted=true}
        }
    }
    RowLayout {
        Layout.fillWidth: true
        Label { text: order.backend.markedCount ? qsTr("Zaznaczono: %1").arg(order.backend.markedCount) : order.backend.activeLimit ? qsTr("W składzie: %1").arg(order.backend.activeLimit) : qsTr("Kolejność startu"); font.pointSize: Theme.caption; color: Theme.muted; Layout.fillWidth: true; elide: Text.ElideRight }
        SButton { visible: order.backend.membership && order.backend.markedCount > 0; text: qsTr("Usuń zaznaczonych"); onClicked: order.backend.removeMarked() }
    }
}
