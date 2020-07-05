import QtQuick 2.5
import Sailfish.Silica 1.0

Dialog {
    id: editPage
    canAccept: true

    property string contact_id: ""
    property string contact_name: ""
    property string contact_nbr: ""

    Component.onCompleted: {
        contactName.forceActiveFocus()
    }

    onAccepted: {
        _blocks.updateBlock(contact_id, contactName.text, contactNbr.text)
    }

    SilicaFlickable {
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: col.height

        clip: true

        ScrollDecorator {
        }

        Column {
            id: col
            spacing: 5
            width: parent.width
            DialogHeader {
                acceptText: qsTr("Save")
                cancelText: qsTr("Cancel")
            }
            SectionHeader {
                text: qsTr("Blocked number")
            }
            TextField {
                id: contactName
                label: qsTr("Enter new name")
                placeholderText: label
                inputMethodHints: Qt.ImhNoPredictiveText
                EnterKey.enabled: text.trim().length > 0
                width: col.width - Theme.paddingLarge * 2
                x: Theme.paddingLarge
                y: Theme.paddingLarge
                text: contact_name
            }
            TextField {
                id: contactNbr
                label: qsTr("Enter new number")
                placeholderText: label
                inputMethodHints: Qt.ImhNoPredictiveText
                EnterKey.enabled: text.trim().length > 0
                width: col.width - Theme.paddingLarge * 2
                x: Theme.paddingLarge
                y: Theme.paddingLarge
                text: contact_nbr
            }
        }
    }
}
