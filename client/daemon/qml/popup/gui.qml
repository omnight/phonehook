import QtQuick 2.5
import Sailfish.Silica 1.0
import QtGraphicalEffects 1.0
import Nemo.DBus 2.0
// debug
//import "screen.js" as Screen

Item {
    width: Screen.width
    height: Screen.height
    id: win

    property bool active: false
    property bool phoneLocked: false
    property bool wantVisible: false
    property bool adWarning: false
    property string firstField: ""

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

    function localize(text) {

        // this function is here to auto-populate the TS files

        if(text === 'name') return qsTr('Name');
        if(text === 'address') return qsTr('Address');
        if(text === 'country') return qsTr('Country');
        //: network operator
        if(text === 'operator') return qsTr('Operator');
        //: owner properties
        if(text === 'properties') return qsTr('Properties');
        //: social security number/ personal number
        if(text === 'ssn') return qsTr('SSN');
        if(text === 'marital status') return qsTr('Marital Status');
        if(text === 'vehicles') return qsTr('Vehicles');
        if(text === 'organization number') return qsTr('Organization Number');
        if(text === 'company information') return qsTr('Company Information');
        if(text === 'remarks') return qsTr('Remarks');
        if(text === 'owner') return qsTr('Owner');
        if(text === 'error') return qsTr('Error');
        if(text === 'email') return qsTr('Email');
        if(text === 'website') return qsTr('Website');
        if(text === 'profession') return qsTr('Profession');
        if(text === 'category') return qsTr('Category');
        if(text === 'comment') return qsTr('Comment');
        if(text === 'status') return qsTr('Status');
        if(text === 'age') return qsTr('Age');

        return null;
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
                property: "opacity";
                duration: 500;
                from: 0;
                to: 1
                easing.type: Easing.InOutQuad
            }
        }

        ScriptAction {
            script: {
                bgGrid.targetH = Qt.binding(function() { return popup.height })
                updateClickArea()
            }
        }

    }

    SequentialAnimation {
        id: fadeOut

        ScriptAction {
            script: updateClickArea()
        }

        ParallelAnimation {

            PropertyAction {
                target: bgGrid
                property: "targetH"
                value: 0
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


    HexGrid {
        id: bgGrid
        anchors.topMargin: 0
        anchors.bottomMargin: 0
        targetH: 0 //popup.height+30
        targetW: popup.width
        anchors.fill: popup
        showWarning: adWarning
        Component.onCompleted: {
            //console.log('hex fit?');
            //hexFit()
        }
    }

    Item {
        visible: wantVisible && !phoneLocked
        width: Screen.width
        height: Math.max(150, resultPager.height + 70 + statusField.height)
        id: popup
        y: 100
        x: (Screen.width - width) / 2
        clip: true



/*        Timer {
            id: activeTest
            interval: 1000
            running: true
            onTriggered: {
                active=true;
            }
        }*/


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

        DBusInterface  {
            id: contactIf
            service: 'com.jolla.contacts.ui'
            iface: 'com.jolla.contacts.ui'
            path: '/com/jolla/contacts/ui'
        }

        onHeightChanged: {
            updateClickArea();
        }


        ListModel {
            id: infoModel
        }

        function stateChange(state) {

            console.log('GOT LOOKUP STATE!!!', state);

            if(state == "activate:lookup") {    // full reset
                active = true;
                statusRow.state = "idle";
                infoModel.clear();
                adWarning = false;
                resultPager.state = "results";
                firstField = "";
            } else if(state == "finished") {
                statusRow.state = "idle";
                if(infoModel.count == 0) {
                    statusField.text = qsTr("No results");
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
            for(var i=0; i < arr.length; i++) {
                if(arr[i].tagname=='field') {
                    infoModel.append(arr[i]);
                    if(firstField == "") firstField = arr[i].value
                }
                if(arr[i].tagname=='block')
                    adWarning = true;
            }
        }

        Item {
            anchors.fill: parent
            id: inner

            Image {
                id: img
                source: "images/ph-logo-white.png"
                width: sourceSize.width
                height: sourceSize.height
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 20
            }


            Flickable {

                id: resultPager
                anchors.left: parent.left
                anchors.right: btnColumn.left
                anchors.top: parent.top
                anchors.topMargin: 70
                contentWidth: rw.width
                clip: true
                interactive: false

                Behavior on contentX {
                   NumberAnimation {
                       duration: 250
                       easing.type: Easing.OutBounce
                   }
                }

                states: [
                    State {
                        name: "results"
                        PropertyChanges {
                            target: resultPager
                            height: rw.height
                            contentX: 0
                        }
                    },
                    State {
                        name: "settings"
                        PropertyChanges {
                            target: resultPager
                            height: settingsBox.height
                            contentX: resultPager.width
                        }
                    }

                ]

                state: "results"

                Rectangle {
                    id: scrollbar
                    anchors.left: rw.left
                    anchors.leftMargin: 4
                    opacity: 0.5
                    y: (results.visibleArea.yPosition * results.height)
                    width: 10
                    height: results.visibleArea.heightRatio * results.height
                    color: Theme.highlightColor
                }

                Row {
                    id: rw
                    height: results.height
                    width: resultPager.width*2

                    ListView {
                        id: results
                        model: infoModel
                        width: resultPager.width

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
                                    text: localize(model.stitle) || model.title
                                    color: Theme.secondaryColor
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
                                font.pixelSize: model.textsize || 28
                                id: text_content
                                text: model.value
                                width: parent.width
                                color: model.color || Theme.primaryColor
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
                        id: settingsBox
                        width: resultPager.width
                        height: 200

                        Row {
                            anchors.verticalCenter: settingsBox.verticalCenter
                            anchors.horizontalCenter: settingsBox.horizontalCenter
                            height: 85
                            spacing: Theme.paddingLarge * 2

                            Image {
                                id: backBtn
                                source: "images/arrow-117-64.png"
                                height: 64
                                width: 64

                                Label {
                                    anchors.top: backBtn.bottom
                                    anchors.horizontalCenter: backBtn.horizontalCenter
                                    text: qsTr("Back")
                                    font.pixelSize: Theme.fontSizeSmall
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        resultPager.state = "results"
                                    }
                                }
                            }

                            Image {
                                id: stopBtn
                                source: "images/stop-3-64.png"
                                height: 64
                                width: 64

                                Label {
                                    anchors.top: stopBtn.bottom
                                    anchors.horizontalCenter: stopBtn.horizontalCenter
                                    text: qsTr("Block")
                                    font.pixelSize: Theme.fontSizeSmall
                                }

                                SequentialAnimation {
                                    id: flash
                                    loops: 2
                                    animations: [
                                        PropertyAnimation {
                                            target: stopBtn
                                            properties: "opacity"
                                            from: 1
                                            to: 0
                                            duration: 100
                                        },

                                        PropertyAnimation {
                                            target: stopBtn
                                            properties: "opacity"
                                            from: 0
                                            to: 1
                                            duration: 100
                                        }
                                    ]
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        _control.blockLastCall(firstField);
                                        flash.restart();
                                    }
                                }
                            }

                            Image {
                                id: contactBtn
                                source: "images/contacts-64.png"
                                height: 64
                                width: 64

                                Label {
                                    anchors.top: contactBtn.bottom
                                    anchors.horizontalCenter: contactBtn.horizontalCenter
                                    text: qsTr("Save")
                                    font.pixelSize: Theme.fontSizeSmall
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        active = false;
                                        contactIf.call("importContactFile", [ "file:///home/nemo/.phonehook/phonehook.vcf" ]);
                                    }
                                }
                            }


                        }



                    }

                }

            }

            Column {
                id: btnColumn
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 20
                spacing: 20
                anchors.topMargin: 40
                width: 50

                Image {
                    width: 48
                    height: 48
                    source: "images/x-mark-5-48.png"
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            hidePopupTimer.stop();
                            active = false;
                        }

                    }
                }

                Image {
                    width: 48
                    height: 48
                    source: "images/gear-2-48.png"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            resultPager.state = "settings"
                        }

                    }
                }
            }


            Item {
                anchors.right: parent.right
                anchors.bottom: parent.bottom
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
        }
    }  
}



