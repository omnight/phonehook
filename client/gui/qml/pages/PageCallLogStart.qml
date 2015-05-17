import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: root

    anchors.fill: parent
    property variant contextItem

    SilicaFlickable {
        anchors.fill: parent

        contentHeight: header.height + callView.height + 50

        PageHeader {
            id: header
            title: qsTr("Missed Calls")
        }


        SilicaListView {
            id: callView
            model: _calls.call_log
            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: contentHeight || 0
            interactive: false
            property Item contextMenu

            delegate: Item {
                id: myListItem
                property bool menuOpen: callView.contextMenu != null && callView.contextMenu.parent === myListItem
                property string number: model.remoteUid
                property string name: _blocks.contactNameFromNumber(model.remoteUid)

                width: ListView.view.width
                height: menuOpen ? callView.contextMenu.height + contentItem.height : contentItem.height

                BackgroundItem {
                    id: contentItem

                    Column {
                        anchors.left: parent.left
                        anchors.right: callCountLabel.left
                        anchors.leftMargin: Theme.paddingMedium
                        anchors.verticalCenter: parent.verticalCenter

                        Label {
                            width: parent.width
                            text: myListItem.name || myListItem.number
                            color: contentItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                        }

                        Label {
                            font.pixelSize: Theme.fontSizeSmall
                            width: parent.width
                            text: new Date(model.recent_time*1000).toISOString()
                                    .replace("T", " ")
                                    .replace(".000Z", "");
                            color: contentItem.highlighted ? Theme.highlightColor : Theme.secondaryColor
                        }
                    }

                    Label {
                        id: callCountLabel
                        anchors.right: parent.right
                        text: model.call_count
                        font.bold: true
                        anchors.rightMargin: Theme.paddingLarge
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    onClicked: {
                        contextItem = myListItem
                        if (!callView.contextMenu)
                            callView.contextMenu = contextMenuComponent.createObject(callView)
                        callView.contextMenu.show(myListItem)
                    }
                }
            }

            Component {
                id: contextMenuComponent
                ContextMenu {
                    MenuItem {
                        text: qsTr("Lookup");
                        onClicked: {
                            _bots.testBot(0, contextItem.number);
                        }
                    }
                    MenuItem {
                        text: qsTr("Block");
                        onClicked: {
                            pageStack.push(Qt.resolvedUrl("PageBlockAddManual.qml"), {name: contextItem.name, number: contextItem.number } )
                        }
                    }
                }
            }

        }

    }

}
