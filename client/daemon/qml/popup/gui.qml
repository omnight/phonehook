import QtQuick 2.0
import Sailfish.Silica 1.0
import QtGraphicalEffects 1.0

// debug
//import "screen.js" as Screen

Item {
    width: Screen.width
    height: Screen.height
    id: win

    property bool active: false
    property bool phoneLocked: false
    property bool wantVisible: false

    onActiveChanged: {
        if(active) fadeIn.start();
        else       fadeOut.start();
    }


    function updateClickArea() {
        if(active) {
            _view.setClickArea(popup.x, popup.y, popup.width,popup.height);
        } else {
            _view.setClickArea(0, 0, 0,0);
        }
    }

    SequentialAnimation {
        id: fadeIn
        PropertyAction {
            target: win
            property: "wantVisible"
            value: true
        }

        ParallelAnimation {

            PropertyAnimation {
                target: popup;
                property: "y";
                duration: 1000;
                from: -height;
                to: 100
                easing.type: Easing.OutBounce
            }

            PropertyAnimation {
                target: popup;
                property: "opacity";
                duration: 500;
                from: 0;
                to: 1
                easing.type: Easing.InOutQuad
            }
        }

        ScriptAction {
            script: updateClickArea()
        }

    }

    SequentialAnimation {
        id: fadeOut

        ScriptAction {
            script: updateClickArea()
        }

        ParallelAnimation {

            PropertyAnimation {
                target: popup;
                property: "y";
                duration: 500;
                from: y;
                to: -height
                easing.type: Easing.InOutQuad
            }

            PropertyAnimation {
                target: popup;
                property: "opacity";
                duration: 500;
                from: 1;
                to: 0
                easing.type: Easing.InOutQuad
            }
        }
        PropertyAction {
            target: win
            property: "wantVisible"
            value: false
        }
    }


    Rectangle {
        visible: wantVisible && !phoneLocked
        width: Screen.width - 60
        height: Math.max(150, results.height + 40 + btnRow.height + statusField.height)
        id: popup
        y: 100
        x: (Screen.width - width) / 2

//        Timer {
//            id: activeTest
//            interval: 1000
//            running: true
//            onTriggered: {
//                active=true;
//            }
//        }

        Component.onCompleted: {
    //        parent = notificationLayer;
        }


    //    NemoDBus.DBusInterface {
    //        iface: "com.nokia.mce.signal"
    //        path: "/com/nokia/mce/signal"
    //        service: "com.nokia.mce"
    //        bus: NemoDBus.DBus.SystemBus
    //        signalsEnabled: true

    //        function tklock_mode_ind(status) {
    //            phoneLocked = (status == 'locked');
    //        }

    //    }


        onHeightChanged: {
            updateClickArea();
        }


        ListModel {
            id: infoModel
        }

        function stateChange(state) {

            console.log('GOT LOOKUP STATE!!!', state);

            if(state == "activate:lookup") {
                active = true;
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


        Timer {
            running: false
            id: hidePopupTimer
            onTriggered: {
                active = false;
            }
        }

        function hideIn(s) {
            hidePopupTimer.interval = s;
            hidePopupTimer.restart();
        }

        Connections {
            target: _control
            onCommand: {
                console.log(fn,data);
                popup[fn](data);
            }
        }

        function result(data) {
            if(hidePopupTimer.running) hidePopupTimer.restart();

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
                GradientStop { position: 0.0; color: Qt.darker(Theme.highlightColor, 1.5) }
                GradientStop { position: 1.0; color: Qt.darker(Theme.highlightColor, 3) }
            }

            border.width: 2
            border.color: Theme.highlightDimmerColor

            Image {
                id: img
                source: "images/ph-logo.png"
                width: sourceSize.width
                height: sourceSize.height
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.margins: 10
            }

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
                            color: Theme.primaryColor
                            wrapMode: Text.NoWrap
                        }
                        Text {
                            id: text_source
                            font.pixelSize: 16
                            anchors.verticalCenter: parent.verticalCenter
                            text: '- ' + model.source
                            opacity: 0.6
                            color: Theme.highlightColor
                            wrapMode: Text.NoWrap
                        }
                    }

                    Text {
                        font.pixelSize: model.textsize || 26
                        id: text_content
                        text: model.value
                        width: parent.width
                        color: model.color || Theme.secondaryColor
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
                        source: "images/wave.png"
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
                    color: Theme.primaryColor
                    font.pixelSize: 20
                    horizontalAlignment: Text.AlignHCenter
                    anchors.verticalCenter: parent.verticalCenter
                    text: ""
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 25
                    id: statusField
                    color: Theme.secondaryColor
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
                        GradientStop { position: 0.0; color: Qt.darker(Theme.highlightColor, 1.5) }
                        GradientStop { position: 1.0; color: Qt.darker(Theme.highlightColor, 2.0) }
                    }

                    border.width: 1
                    border.color: Theme.highlightDimmerColor
                    radius: 3

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            hidePopupTimer.stop();
                            active = false;
                        }
                    }

                    Text {
                        text: "Close"
                        font.pixelSize: parent.height/2
                        anchors.centerIn: parent
                        color: Theme.primaryColor
                        //font.pixelSize: Theme.fontSizeMedium
                        //font.weight: Font.Bold
                    }

                }

    //            Item {
    //                width: spacer.width
    //                height: parent.height
    //            }

    //            Rectangle {
    //                height: parent.height
    //                width: (parent.width - (spacer.width * 3)) / 2

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
    //                    }
    //                }

    //                Text {
    //                    text: "Menu"
    //                    font.pixelSize: parent.height/2
    //                    anchors.centerIn: parent
    //                }

    //            }

    //            Item {
    //                width: spacer.width
    //                height: parent.height
    //            }

            }
        }
    }



}



