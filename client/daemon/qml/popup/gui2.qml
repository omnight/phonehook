import QtQuick 2.0
import Sailfish.Silica 1.0
// debug
//import "screen.js" as Screen
import "/usr/share/phonehook/qml/popup/"

Item {
    visible: false
    width: Screen.width
    height: Math.max(250, results.height + 200 + btnRow.height + statusField.height)
    id: rewt
    y: 800
    x: 0

    property bool active: false

    onActiveChanged: {
        rewt.visible = active;
    }

    HexGrid {
        targetW: rewt.width
        targetH: active ? rewt.height : 0
    }

    Component.onCompleted: {
        parent = notificationLayer;
    }

    ListModel {
        id: infoModel
    }

    function stateChange(state) {
        active = true;
        console.log('GOT LOOKUP STATE!!!', state);

        if(state == "activate:lookup") {
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


//    Rectangle {
//        anchors.top: topBorder.bottom
//        anchors.bottom: bottomBorder.top
//        anchors.left: parent.left
//        anchors.right: parent.right
//        id: inner

//        color:"#FFFFFF"

//        ListView {
//            id: results
//            model: infoModel
//            width: parent.width
//            anchors.horizontalCenter: parent.horizontalCenter
//            anchors.top: parent.top
//            anchors.topMargin: 20

//            //        interactive: !(atYBeginning && verticalVelocity < 0) &&
//            //                     !(atYEnd && verticalVelocity > 0)

//            boundsBehavior: Flickable.StopAtBounds
//            clip: true

//            onCountChanged: {
//                if(count > 0)
//                    height = Qt.binding(function() { return Math.min(Screen.height / 2, contentHeight); })
//            }


//            Behavior on height {
//                NumberAnimation {
//                    duration: 200
//                    easing.type: Easing.InOutBack
//                }
//            }

//            delegate: Column {
//                width: parent.width * 0.9
//                anchors.horizontalCenter: parent.horizontalCenter

//                Row {
//                    width: parent.width
//                    spacing: 10
//                    Text {
//                        id: text_header
//                        font.pixelSize: 20
//                        font.weight: Font.Bold
//                        text: model.title
//                        color: '#222244'
//                        wrapMode: Text.NoWrap
//                    }
//                    Text {
//                        id: text_source
//                        font.pixelSize: 16
//                        anchors.verticalCenter: parent.verticalCenter
//                        text: '- ' + model.source
//                        opacity: 0.6
//                        color: "#444444"
//                        wrapMode: Text.NoWrap
//                    }
//                }

//                Text {
//                    font.pixelSize: model.textsize || 26
//                    id: text_content
//                    text: model.value
//                    width: parent.width
//                    color: model.color || '#000000'
//                    wrapMode: Text.WordWrap
//                    fontSizeMode: Text.Fit
//                    maximumLineCount: 5
//                }
//                Item {
//                    width: parent.width
//                    height: 15
//                }

//            }
//        }


//        Item {
//            anchors.right: parent.right
//            anchors.bottom: btnRow.top
//            anchors.left: parent.left
//            height: 30
//            anchors.leftMargin: 30
//            anchors.rightMargin: 30
//            id: statusRow

//            states: [
//                State {
//                    name: "running"
//                    PropertyChanges {
//                        target: statusField
//                        visible: false
//                    }

//                    PropertyChanges {
//                        target: loadingAnimation
//                        visible: true
//                    }
//                },
//                State {
//                    name:"idle"
//                    PropertyChanges {
//                        target: sourceName
//                        visible: false
//                    }
//                }

//            ]

////            ProgressBar {
////                indeterminate: true
////                anchors.fill: parent
////            }

//            Item {
//                id: loadingAnimation
//                anchors.left: parent.left
//                anchors.right: parent.right
//                anchors.rightMargin: sourceName.paintedWidth + 10
//                anchors.verticalCenter: parent.verticalCenter
//                height: 20
//                clip: true
//                visible: false

//                Image {
//                    id: im
//                    source: "/usr/share/phonehook/qml/popup/images/wave.png"
//                    width: parent.width+sourceSize.width
//                    height: parent.height
//                    fillMode: Image.TileHorizontally
//                    y: 0
//                    x: 0
//                }

//                PropertyAnimation {
//                    target: im
//                    property: "x"
//                    from: 0
//                    to: -im.sourceSize.width
//                    loops: Animation.Infinite
//                    running: loadingAnimation.visible && active
//                    duration: 400
//                }

//            }

//            Text {
//                anchors.right: parent.right
//                id: sourceName
//                color: "#000022"
//                font.pixelSize: 20
//                horizontalAlignment: Text.AlignHCenter
//                anchors.verticalCenter: parent.verticalCenter
//                text: ""
//            }

//            Text {
//                anchors.horizontalCenter: parent.horizontalCenter
//                height: 25
//                id: statusField
//                color: "#000044"
//                font.pixelSize: 20
//                horizontalAlignment: Text.AlignHCenter
//                anchors.verticalCenter: parent.verticalCenter
//                text: ""
//            }

//        }

//        Item {
//            id: btnRow
//            anchors.bottomMargin: 10
//            height: 40
//            anchors.bottom: parent.bottom
//            anchors.right: parent.right
//            anchors.left: parent.left

////            Item {
////                id: spacer
////                width: 10
////                height: parent.height
////            }


//            Rectangle {
//                height: parent.height
//                width: parent.width * 0.7  // (parent.width - (spacer.width * 3)) / 2
//                anchors.horizontalCenter: parent.horizontalCenter

//                gradient: Gradient {
//                    GradientStop { position: 0.0; color: "#eeeeee" }
//                    GradientStop { position: 1.0; color: "#bfbfbf" }
//                }

//                border.width: 1
//                border.color: "#333333"
//                radius: 3

//                MouseArea {
//                    anchors.fill: parent
//                    onClicked: {
//                        active = false;
//                    }
//                }

//                Text {
//                    text: "Close"
//                    font.pixelSize: parent.height/2
//                    anchors.centerIn: parent
//                }

//            }

//        }
//    }
}


