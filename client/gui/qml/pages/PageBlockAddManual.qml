import QtQuick 2.5
import Sailfish.Silica 1.0

Dialog {
    id: root

    property string number
    property string name

    acceptDestinationAction: PageStackAction.Pop

    onAccepted: {
        _blocks.addManualBlock(blockName.text,blockNumber.text,blockHidden.checked);
    }

    canAccept: blockHidden.checked || (blockName.text != '' && blockNumber.text != '')

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: clmn.height+header.height+50

        DialogHeader {
            id: header
            acceptText: qsTr("Add Block")
        }

        Column {
            id: clmn
            spacing: Theme.paddingLarge
            anchors.fill: parent
            anchors.topMargin: header.height
            anchors.leftMargin: Theme.paddingLarge
            anchors.rightMargin: Theme.paddingLarge

            Text {
                color: Theme.secondaryColor
                text: qsTr("Add a number to be blocked. Number may be either local or international variety. Use * for wildcard matching.")
                font.pixelSize: Theme.fontSizeSmall
                width: parent.width
                wrapMode: Text.Wrap
            }

            Item {
                height: 50
                width: parent.width
            }

            TextSwitch {
                width: parent.width
                id: blockHidden
                //: toggle switch for hidden number
                text: qsTr("Hidden number")
                checked: false
                onCheckedChanged: {

                }
            }

            TextField {
                id: blockName
                width: parent.width
                //: placeholder for name input
                placeholderText: qsTr("Name")
                visible: !blockHidden.checked
                text: name
                anchors.horizontalCenter: parent.horizontalCenter
            }

            TextField {
                id: blockNumber
                visible: !blockHidden.checked
                width: parent.width
                //: placeholder for number input
                placeholderText: qsTr("Phone Number")
                text: number
                anchors.horizontalCenter: parent.horizontalCenter
                inputMethodHints: Qt.ImhDialableCharactersOnly
            }
        }
    }
}
