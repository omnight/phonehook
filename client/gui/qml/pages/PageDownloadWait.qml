import QtQuick 2.5
import Sailfish.Silica 1.0

Page {
    id: root
    backNavigation: false
    canNavigateForward: false


    BusyIndicator {
        id: loader
        anchors.centerIn: parent
        size: BusyIndicatorSize.Large
        running: true
    }

    Label {
        id: errorText
        anchors.centerIn: parent
        text: qsTr("Download Error :(")
        visible: false
    }

    Connections {
        target: _bots
        onBotDownloadSuccess: {
            pageStack.completeAnimation()
            pageStack.replace(Qt.resolvedUrl("PageBotDetails.qml"),
                            { botId: botId })
        }

        onBotDownloadFailure: {
            state = "failure"
        }
    }

    states: [
        State {
            name: "failure"
            PropertyChanges {
                target: loader
                visible: false
            }
            PropertyChanges {
                target: errorText
                visible: true
            }
            PropertyChanges {
                target: root
                backNavigation: true
            }

        }
    ]


}
