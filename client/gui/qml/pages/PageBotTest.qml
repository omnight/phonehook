import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: root

    property string name
    property int botId

    PageHeader {
        title: name
    }

    Column {
        anchors.fill: parent
        anchors.margins: Theme.paddingLarge

        Item {
            width: parent.width
            height: Screen.height * 0.2
        }

        TextField {
            id: testNumber
            width: parent.width
            placeholderText: "Phone Number"
            anchors.horizontalCenter: parent.horizontalCenter
            inputMethodHints: Qt.ImhDigitsOnly
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter

            text: "Run"
            enabled: testNumber.text.length > 0

            onClicked: {
                _bots.testBot(botId, testNumber.text);
            }
        }
    }
}
