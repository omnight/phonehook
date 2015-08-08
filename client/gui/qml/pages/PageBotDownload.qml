import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {

    property variant botData
    id: rewt

    onAccepted: {
        pageStack.replace(Qt.resolvedUrl("PageDownloadWait.qml"))
        _bots.downloadBot(botData.file)
    }

    DialogHeader {
        id: header
        acceptText: "Install"
    }

    function niceNameCapability(s) {
        switch(s) {
            case 'lookup': return 'Reverse caller lookup';
            case 'person_search': return 'Search for people';
            case 'business_search': return 'Search for businesses';
            case 'block': return 'Auto-blocking';
        }

        return s;   // non-nice name
    }

    SilicaFlickable {
        anchors.fill: parent
        anchors.topMargin: header.height
        anchors.margins: Theme.paddingLarge
        clip: true

       contentHeight: concol.height

       Column {
           id: concol
           width: parent.width

            Image {
                width: Screen.width / 2
                height: (sourceSize.height / sourceSize.width) * width
                source: botData.icon
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Label {
                text: botData.name
                font.pixelSize: Theme.fontSizeLarge
                anchors.horizontalCenter: parent.horizontalCenter
            }


            Item {
                height: 20
                width: parent.width
            }

            Label {
                text: "Status"
            }

            Text {
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
                Component.onCompleted: {

                    var status = _bots.botStatusCompare(botData.name, botData.revision)

                    console.log('minv', botData.minversion, _bots.version());

                    if(botData.minversion && botData.minversion > _bots.version()) status = 3;

                    if(status == 0) { text = "Not installed yet"; }
                    if(status == 1) { text = "Installed and up-to-date"; header.acceptText = "Reset"; color = "#DDFFDD" }
                    if(status == 2) { text = "Update available"; header.acceptText = "Update"; color = "#FFFFDD" }
                    if(status == 3) { text = "A newer version of phonehook is required"; header.enabled = false; canAccept = false; header.acceptText = ""; }
                }
            }

            Item {
                height: 20
                width: parent.width
                visible: botData.link != ""
            }

            Label {
                text: "Link"
                visible: botData.link != ""
            }

            Text {
                width: parent.width
                color: Theme.secondaryColor
                text: botData.link
                visible: botData.link != ""
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                font.underline: true
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        Qt.openUrlExternally(botData.link);
                    }
                }
            }

            Item {
                visible: botData.capabilities != ""
                height: 20
                width: parent.width
            }

            Label {
                text: "Capabilities"
                visible: botData.capabilities != ""
            }

            ListView {
                height: contentHeight
                interactive: false
                width: parent.width
                model: botData.capabilities.split('|')
                delegate: Text {
                    font.pixelSize: Theme.fontSizeSmall
                    color: Theme.secondaryColor
                    text: '  • ' + niceNameCapability(modelData)
                    width: parent.width
                }
            }

            Item {
                height: 20
                width: parent.width
            }

            Label {
                text: "Description"
            }

            Text {
                width: parent.width
                color: Theme.secondaryColor
                text: botData.description
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
            }


        }

    }


}
