import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: root

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: contactView.height+header.height+50

        PageHeader {
            id: header
            title: qsTr("Block Contact")
        }

        SilicaListView {
            id: contactView
            model: _blocks.contacts
            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: contentHeight || 0
            //anchors.fill: parent
            interactive: false

            //property variant currentItem

            delegate:
                BackgroundItem {
                    id: delegate

                    Label {
                        anchors.left: parent.left
                        anchors.leftMargin: Theme.paddingLarge
                        anchors.verticalCenter: parent.verticalCenter
                        text: model.displayLabel
                        color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
                    }

                    onClicked: {
                        _blocks.addBlockedContact(model.contactId);
                        pageStack.pop();
                    }
                }
            }


    }



}
