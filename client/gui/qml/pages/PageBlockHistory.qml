import QtQuick 2.5
import Sailfish.Silica 1.0

Page {
    id: root
    property variant contextItem

    Label {
        text: qsTr("No events")
        anchors.horizontalCenter: root.horizontalCenter
        anchors.verticalCenter: root.verticalCenter
        font.pixelSize: Theme.fontSizeLarge
        visible: historyView.model.count === 0
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: historyView.height+header.height+50

        PageHeader {
            id: header
            title: qsTr("History")
        }

        SilicaListView {
            id: historyView
            model: _blocks.history
            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: contentHeight || 0
            interactive: false
            property Item contextMenu

            delegate:
                BackgroundItem {
                    id: delegate
                    property string number: model.number
                    property bool menuOpen: historyView.contextMenu != null && historyView.contextMenu.parent === delegate
                    height: menuOpen ? historyView.contextMenu.height + contentItem.height : contentItem.height

                    Column {
                        id: contentItem
                        anchors.left: parent.left
                        anchors.leftMargin: Theme.paddingMedium
                        anchors.right: parent.right
                        anchors.rightMargin: Theme.paddingMedium
                        Label {
                            text: model.number
                            color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
                        }
                        Label {
                            text: new Date(model.date).toLocaleString(null, Locale.ShortFormat)
                            color: delegate.highlighted ? Theme.highlightColor : Theme.secondaryColor
                            font.pixelSize: Theme.fontSizeSmall
                        }
                    }

                    onClicked: {
                        contextItem = delegate
                        if (!historyView.contextMenu)
                            historyView.contextMenu = contextMenuComponent.createObject(historyView)
                        historyView.contextMenu.show(delegate)
                    }

                }


                Component {
                    id: contextMenuComponent
                    ContextMenu {
                        MenuItem {
                            text: qsTr("Lookup");
                            enabled: contextItem.number.length > 4
                            onClicked: {
                                _bots.testBot(0, contextItem.number);
                            }
                        }
                    }
                }

            }
    }



}
