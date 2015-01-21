import QtQuick 2.0
import Sailfish.Silica 1.0
import QtGraphicalEffects 1.0
// debug
//import "screen.js" as Screen

Item {
    visible: false
    width: Screen.width
    height: Math.max(250, results.height + 200 + btnRow.height + statusField.height)
    id: rewt
    y: 800
    x: 0

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
        ParallelAnimation {

            PropertyAnimation {
                target: rewt;
                property: "y";
                duration: 500;
                from: -height;
                to: 100
                easing.type: Easing.OutBounce
            }

            PropertyAnimation {
                target: rewt;
                property: "opacity";
                duration: 500;
                from: 0;
                to: 1
                easing.type: Easing.InOutQuad
            }
        }
    }

    SequentialAnimation {
        id: fadeOut
        ParallelAnimation {

            PropertyAnimation {
                target: rewt;
                property: "y";
                duration: 500;
                from: y;
                to: -height
                easing.type: Easing.InOutQuad
            }

            PropertyAnimation {
                target: rewt;
                property: "opacity";
                duration: 500;
                from: 1;
                to: 0
                easing.type: Easing.InOutQuad
            }
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
            statusRow.state = "idle";
            infoModel.clear();
        } else if(state == "finished") {
            statusRow.state = "idle";
            if(infoModel.count == 0) {
                statusField.text = "No results"
            } else {
                statusField.text = "";
            }
        } else if(state.indexOf('running:') == 0 ) {
            statusRow.state = "running";
            sourceName.text = state.replace('running:','');
        }

    }

    function result(data) {
        active = true;
        var arr = JSON.parse(data);
        for(var i=0; i < arr.length; i++)
            infoModel.append(arr[i]);
    }


    Image {
        id: topBorder
        source: "/usr/share/phonehook/qml/popup/images/fade.png"
        fillMode: Image.TileHorizontally
        height: 96
        width: parent.width
        anchors.top: parent.top
        anchors.left: parent.left
        rotation: 180
    }

    Image {
        id: bottomBorder
        source: "/usr/share/phonehook/qml/popup/images/fade.png"
        fillMode: Image.TileHorizontally
        height: 96
        width: parent.width
        anchors.bottom:parent.bottom
        anchors.left: parent.left
        rotation: 0
    }

    Rectangle {
        anchors.top: topBorder.bottom
        anchors.bottom: bottomBorder.top
        anchors.left: parent.left
        anchors.right: parent.right
        id: inner

        color:"#FFFFFF"

//        Image {
//            id: img
//            source: "/usr/share/phonehook/qml/popup/images/ph-logo.png"
//            width: sourceSize.width
//            height: sourceSize.height
//            anchors.top: parent.top
//            anchors.right: parent.right
//            anchors.margins: 10
//        }

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


            Behavior on height {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.InOutBack
                }
            }

            delegate: Column {
                width: parent.width * 0.9
                anchors.horizontalCenter: parent.horizontalCenter

                Row {
                    width: parent.width
                    spacing: 10
                    Text {
                        id: text_header
                        font.pixelSize: 20
                        font.weight: Font.Bold
                        text: model.title
                        color: '#222244'
                        wrapMode: Text.NoWrap
                    }
                    Text {
                        id: text_source
                        font.pixelSize: 16
                        anchors.verticalCenter: parent.verticalCenter
                        text: '- ' + model.source
                        opacity: 0.6
                        color: "#444444"
                        wrapMode: Text.NoWrap
                    }
                }

                Text {
                    font.pixelSize: model.textsize || 26
                    id: text_content
                    text: model.value
                    width: parent.width
                    color: model.color || '#000000'
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


        Item {
            anchors.right: parent.right
            anchors.bottom: btnRow.top
            anchors.left: parent.left
            height: 30
            anchors.leftMargin: 30
            anchors.rightMargin: 30
            id: statusRow

            states: [
                State {
                    name: "running"
                    PropertyChanges {
                        target: statusField
                        visible: false
                    }

                    PropertyChanges {
                        target: loadingAnimation
                        visible: true
                    }
                },
                State {
                    name:"idle"
                    PropertyChanges {
                        target: sourceName
                        visible: false
                    }
                }

            ]

//            ProgressBar {
//                indeterminate: true
//                anchors.fill: parent
//            }

            Item {
                id: loadingAnimation
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: sourceName.paintedWidth + 10
                anchors.verticalCenter: parent.verticalCenter
                height: 20
                clip: true
                visible: false

                Image {
                    id: im
                    source: "/usr/share/phonehook/qml/popup/images/wave.png"
                    width: parent.width+sourceSize.width
                    height: parent.height
                    fillMode: Image.TileHorizontally
                    y: 0
                    x: 0
                }

                PropertyAnimation {
                    target: im
                    property: "x"
                    from: 0
                    to: -im.sourceSize.width
                    loops: Animation.Infinite
                    running: loadingAnimation.visible && active
                    duration: 400
                }

            }

            Text {
                anchors.right: parent.right
                id: sourceName
                color: "#000022"
                font.pixelSize: 20
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
                text: ""
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                height: 25
                id: statusField
                color: "#000044"
                font.pixelSize: 20
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
                text: ""
            }

        }

        Item {
            id: btnRow
            anchors.bottomMargin: 10
            height: 40
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.left: parent.left

//            Item {
//                id: spacer
//                width: 10
//                height: parent.height
//            }


            Rectangle {
                height: parent.height
                width: parent.width * 0.7  // (parent.width - (spacer.width * 3)) / 2
                anchors.horizontalCenter: parent.horizontalCenter

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

        }
    }
}


