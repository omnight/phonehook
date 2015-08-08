import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: root

    Label {
        text: "No sources to add"
        anchors.horizontalCenter: root.horizontalCenter
        anchors.verticalCenter: root.verticalCenter
        font.pixelSize: Theme.fontSizeLarge
        visible: blockSourceView.model.count == 0
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: blockSourceView.height+header.height+50

        PageHeader {
            id: header
            title: qsTr("Add Block Source")
        }

        SilicaListView {
            id: blockSourceView
            model: _blocks.sources
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
                        text: model.name
                        color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
                    }

                    onClicked: {
                        _bots.setBlockSource(model.id, true);
                        _blocks.initBlocks();
                        pageStack.pop();
                    }
                }
            }


    }



}
