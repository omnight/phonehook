import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: root

    Label {
        text: "No events"
        anchors.horizontalCenter: root.horizontalCenter
        anchors.verticalCenter: root.verticalCenter
        font.pixelSize: Theme.fontSizeLarge
        visible: historyView.model.count == 0
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
            anchors.margins: Theme.paddingLarge

            delegate:
                BackgroundItem {
                    id: delegate

                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        text: model.date
                        color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
                    }
                }
            }
    }
}
