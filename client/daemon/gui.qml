import QtQuick 2.0
import Sailfish.Silica 1.0
import QtGraphicalEffects 1.0

// debug
//import "screen.js" as Screen

Rectangle {
    visible: false
    width: Screen.width - 60
    height: results.height + 40 + btnRow.height + statusField.height
    id: rewt
    y: 100
    x: (Screen.width - width) / 2

    property bool active: false

    onActiveChanged: {
        if(active) fadeIn.start();
        else       fadeOut.start();
    }

    Component.onCompleted: {
        parent = notificationLayer;
    }

    SequentialAnimation {
        id: fadeIn
        PropertyAction {
            target: rewt
            property: "visible"
            value: true
        }
        PropertyAnimation {
            target: rewt;
            property: "opacity";
            duration: 500;
            from: 0;
            to: 0.7
            easing.type: Easing.InOutQuad
        }
    }

    SequentialAnimation {
        id: fadeOut
        PropertyAnimation {
            target: rewt;
            property: "opacity";
            duration: 500;
            from: 0.7;
            to: 0
            easing.type: Easing.InOutQuad
        }
        PropertyAction {
            target: rewt
            property: "visible"
            value: false
        }
    }


    ListModel {
        id: infoModel
    }

    function stateChange(state) {
        active = true;
        console.log('GOT LOOKUP STATE!!!', state);

        if(state == "activate") {
            infoModel.clear();
        } else if(state == "finished") {
            if(infoModel.count == 0) {
                statusField.text = "No results"
            } else {
                statusField.text = "";
            }
        } else if(state.indexOf('running:') == 0 ) {
            statusField.text = state.replace('running:','');
        }

    }

    function result(data) {
        active = true;
        var arr = JSON.parse(data);
        for(var i=0; i < arr.length; i++)
            infoModel.append(arr[i]);
    }


    RectangularGlow {
        anchors.fill: inner
        color: "#80222222"
        spread: 0.2
        glowRadius: 10
    }


    Rectangle {
        anchors.fill: parent
        id: inner

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#eeeeee" }
            GradientStop { position: 1.0; color: "#b9d7d7" }
        }

        border.width: 2
        border.color: "#6179b0"

        Rectangle {
            width: img.width + border.width*2
            height: img.height + border.width * 2
            anchors.bottom: parent.top
            anchors.bottomMargin: -parent.border.width
            anchors.right: parent.right
            color: "#eeeeee"

            radius: 2
            border.width: parent.border.width
            border.color: "#6179b0"

            Rectangle {
                color: parent.color
                height: parent.border.width
                x: parent.border.width
                width: parent.width - parent.border.width * 2
                anchors.bottom: parent.bottom
            }

            Image {
                id: img
                source: "file:///C:/Johan/Sailfish/phonehook/client/phonehook/images/ph-logo.png"
                width: sourceSize.width
                height: sourceSize.height
                anchors.top: parent.top
                anchors.left: parent.left
            }

        }



        /*
    DBusInterface {
        id: dbi
        busType: DBusInterface.SessionBus
        destination: "com.omnight.phonehook"
        path: "/"
        iface: "com.omnight.phonehook"
    }*/

        ListView {
            id: results
            model: infoModel
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 20

            //        interactive: !(atYBeginning && verticalVelocity < 0) &&
            //                     !(atYEnd && verticalVelocity > 0)

            boundsBehavior: Flickable.StopAtBounds
            clip: true

            onCountChanged: {
                if(count > 0)
                    height = Qt.binding(function() { return Math.min(Screen.height / 2, contentHeight); })
            }


            delegate: Column {
                width: parent.width * 0.9
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    id: text_header
                    font.pixelSize: 15
                    font.weight: Font.Bold
                    text: model.title
                    width: parent.width
                    color: '#222244'
                    //horizontalAlignment: Text.AlignHCenter
                }
                Text {
                    font.pixelSize: 18
                    id: text_content
                    text: model.value
                    width: parent.width
                    color: '#000000'
                    wrapMode: Text.WordWrap
                    fontSizeMode: Text.Fit
                    maximumLineCount: 5
                }
                Item {
                    width: parent.width
                    height: 15
                }

            }
        }

        Text {
            anchors.right: parent.right
            anchors.top: results.bottom
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.rightMargin: 30
            height: 25
            id: statusField
            color: "#000044"
            font.pixelSize: 20
            horizontalAlignment: Text.AlignHCenter
            text: ""
        }


        Row {
            id: btnRow
            anchors.bottomMargin: spacer.width
            height: 40
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.left: parent.left

            Item {
                id: spacer
                width: 10
                height: parent.height
            }


            Rectangle {
                height: parent.height
                width: (parent.width - (spacer.width * 3)) / 2

                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#eeeeee" }
                    GradientStop { position: 1.0; color: "#bfbfbf" }
                }

                border.width: 1
                border.color: "#333333"
                radius: 3

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        active = false;
                    }
                }

                Text {
                    text: "Close"
                    font.pixelSize: parent.height/2
                    anchors.centerIn: parent
                }

            }

            Item {
                width: spacer.width
                height: parent.height
            }

            Rectangle {
                height: parent.height
                width: (parent.width - (spacer.width * 3)) / 2

                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#eeeeee" }
                    GradientStop { position: 1.0; color: "#bfbfbf" }
                }

                border.width: 1
                border.color: "#333333"
                radius: 3

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                    }
                }

                Text {
                    text: "Menu"
                    font.pixelSize: parent.height/2
                    anchors.centerIn: parent
                }

            }

            Item {
                width: spacer.width
                height: parent.height
            }

        }
    }
}


