import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: root

    anchors.fill: parent

    property variant remorseItem
    function remorseDelete() {
        remorseDeleteBlock.execute(remorseItem,
                                 qsTr("Deleting %1").arg(remorseItem.name),
                                 function() {
                                     _blocks.deleteBlock(remorseItem.id);
                                 }, 5000);
    }

    Label {
        text: qsTr("No blocks activated")
        anchors.horizontalCenter: root.horizontalCenter
        anchors.verticalCenter: root.verticalCenter
        font.pixelSize: Theme.fontSizeLarge
        visible: blockView.model.count == 0
    }

    SilicaFlickable {
        anchors.fill: parent

        contentHeight: header.height + blockView.height + 50

        PullDownMenu {
            enabled: true

            MenuItem {
                text: qsTr("Add Block Source")
                onClicked: {
                    _blocks.initSources();
                    pageStack.push(Qt.resolvedUrl("PageBlockAddSource.qml"))
                }
            }

            MenuItem {
                text: qsTr("Block Contact")
                onClicked: pageStack.push(Qt.resolvedUrl("PageBlockContact.qml"))
            }

            MenuItem {
                text: qsTr("Block Number")
                onClicked: pageStack.push(Qt.resolvedUrl("PageBlockAddManual.qml"))
            }

            MenuItem {
                text: qsTr("Refresh")
                onClicked: _blocks.initBlocks()
            }
        }

        PageHeader {
            id: header
            title: qsTr("Call Blocks")
        }

        RemorseItem {
            id: remorseDeleteBlock
        }

        SilicaListView {
            id: blockView
            model: _blocks.db_blocks
            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: contentHeight || 0
            interactive: false
            property Item contextMenu

            delegate: Item {
                id: myListItem
                property bool menuOpen: blockView.contextMenu != null && blockView.contextMenu.parent === myListItem
                property int id: model.id

                property string name: model.name || model.bot_name ||
                                      (model.contact_id ? _blocks.contactName(model.contact_id) : ' ??' )

                width: ListView.view.width
                height: menuOpen ? blockView.contextMenu.height + contentItem.height : contentItem.height

                BackgroundItem {
                    id: contentItem


                    Image {
                        id: blockTypeImage
                        anchors.left: parent.left
                        anchors.leftMargin: Theme.paddingLarge
                        anchors.verticalCenter: parent.verticalCenter
                        height: 48
                        width: 48
                        fillMode: Image.PreserveAspectCrop
                        source: model.type == 0 ? '../images/edit-6-48.png' :
                                model.type == 1 ? '../images/contacts-48.png' :
                                model.type == 2 ? '../images/online-48.png': ''
                    }

                    Column {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: blockTypeImage.right
                        anchors.right: blockCountLabel.left
                        anchors.leftMargin: Theme.paddingMedium
                        Label {
                            text: myListItem.name
                            color: contentItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                        }
                        Label {
                            visible: model.number != null
                            text: model.number
                            font.pixelSize: Theme.fontSizeExtraSmall
                            color: Theme.secondaryColor
                        }
                    }

                    Label {
                        id: blockCountLabel
                        anchors.right: parent.right
                        text: model.block_count
                        font.bold: true
                        anchors.rightMargin: Theme.paddingLarge
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    onPressAndHold: {
                        remorseItem = myListItem;
                        if (!blockView.contextMenu)
                            blockView.contextMenu = contextMenuComponent.createObject(blockView)
                        blockView.contextMenu.show(myListItem)
                    }

                    onClicked: {
                        _blocks.initHistory(model.id);
                        pageStack.push(Qt.resolvedUrl("PageBlockHistory.qml"));
                    }
                }
            }

            Component {
                id: contextMenuComponent
                ContextMenu {
                       MenuItem {
                        text: qsTr("Delete");
                        onClicked: {
                            remorseDelete();
                        }
                    }
                }
            }

        }

    }

}
