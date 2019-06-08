import QtQuick 2.5
import Sailfish.Silica 1.0

Rectangle {
    id: btn
    color: p.pressed ? Theme.highlightColor :  colorAlpha(Qt.darker(Theme.highlightColor, 2), .5)
    width: (parent.width - Theme.paddingLarge) / 2
    height: width


    property string icon
    property string link
    property string text

    Image {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: Theme.paddingLarge*2
        width: parent.width*.4
        height: parent.width*.4
        source: btn.icon
    }

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Theme.paddingLarge
        color: Theme.primaryColor
        font.weight: Font.Bold
        text: btn.text
    }

    MouseArea {
        id: p
        anchors.fill: parent
        onClicked: {
            onClicked: pageStack.push(Qt.resolvedUrl(btn.link))
        }
    }

}
