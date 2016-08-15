import QtQuick 2.0
import QtQuick.XmlListModel 2.0
import Sailfish.Silica 1.0

Page {
    id: root

    anchors.fill: parent
    property bool isReady: false
    property bool isError: false
    property bool isLoading: false


    PageHeader {
        id: header
        title: qsTr("Available Sources")
    }

    BusyIndicator {
        anchors.centerIn: parent
        size: BusyIndicatorSize.Large
        running: isLoading
    }

    Label {
        anchors.centerIn: parent
        text: qsTr("Failed to load")
        visible: isError
    }

    Component.onCompleted: {
        serverBots.clear()

        var xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function() {

            console.log('readystate', xhr.readyState)

            if(xhr.readyState == 1 || xhr.readyState == 2 || xhr.readyState == 3) {
                isLoading = true;
            }

            if(xhr.readyState == 4) {
                isLoading = false;
                var serverData =JSON.parse(xhr.responseText);

                var myCountry = _setting.get('location', '');
                var prioSort = [];
                for(var n = 0; n < serverData.length; n++) {
                    if(serverData[n].country == myCountry.toUpperCase()) {
                        console.log(n);
                        prioSort = prioSort.concat(serverData.splice(n, 1));
                        n--;
                    }
                }

                var sb = prioSort.concat(serverData);
                isReady = true;

                var sc = {};

                for(var n in sb) {
                    var t2 = [];
                    for(var nn in sb[n].tags) {
                        t2.push({ "cap": sb[n].tags[nn] })
                    }
                    sb[n].tags = t2;
                    sc[sb[n].sort_key] = (sc[sb[n].sort_key] || 0) + 1
                    serverBots.append(sb[n])
                }

                // auto-expand first selection
                expandMap = [sb[0].sort_key];

                console.log(JSON.stringify(sc))
                sectionCount = sc;
            }
        };



        xhr.open('GET', _setting.get("sources_index_url", 'https://raw.githubusercontent.com/omnight/phonehook-sources/master/files/index.js'), true);
        xhr.send();
    }


    ListModel {
        id: serverBots
        ListElement {
            name: ""
            country: ""
            revision: 0
            description: ""
            icon: ""
            link: ""
            tags: []
            file: ""
            minversion: 0
            sort_key: ""
        }
    }


    property variant expandMap: [ ]
    property variant sectionCount: { 0 : 0 }

    function isExpanded(section) {
        return expandMap.indexOf(section) != -1;
    }

    function expand(section) {
        if(expandMap.indexOf(section) == -1) {
            var e = expandMap;
            e.push(section);
            expandMap = e;
        }
    }

    function contract(section) {
        var i = expandMap.indexOf(section);
        if(i != -1) {
            var e = expandMap;
            e.splice(i,1);
            expandMap = e;
        }
    }

    function toggle(section) {
        if(isExpanded(section)) contract(section)
        else                    expand(section)
    }

    ListView {
        id: botListView
        model: serverBots
        anchors.fill: parent
        anchors.topMargin: header.height
        clip: true
        section.property: "sort_key"


        footer: Item {
            width: parent.width
            height: 150
            Item {
                height: 20
                width: parent.width
            }

            Text {
                visible: isReady
                anchors.margins: Theme.paddingLarge
                anchors.left: parent.left
                anchors.right: parent.right
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                wrapMode: Text.Wrap
                text: qsTr("All names and logos listed here are properties of respective rights holders. Phonehook is not endorsed by any of these services. ")
            }
        }

        section.delegate: Item {

            property string sectionCountry: /\|(.*)/.exec(section)[1]
            property bool expanded

            width: parent.width
            height: 70

            Row {
                id: countryNameLine
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.leftMargin: Theme.paddingLarge
                anchors.verticalCenter: parent.verticalCenter
                Image {
                    id: flag
                    anchors.verticalCenter: parent.verticalCenter
                    source: "https://omnight.github.io/phonehook-sources/flags/" + sectionCountry.toLowerCase() + ".png"

                    Rectangle {
                        anchors.left: parent.right
                        anchors.top: parent.bottom
                        anchors.leftMargin: -10
                        anchors.topMargin: -20
                        radius: 5
                        color: "#222255"
                        height: cLabel.height
                        width: cLabel.width+10
                        opacity: .8

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            id: cLabel
                            font.pixelSize: Theme.fontSizeExtraSmall
                            font.weight: Font.Bold
                            text: sectionCount[section]
                            color: Theme.primaryColor
                        }
                    }

                }

                Item {
                    height: parent.height
                    width: 20
                }

                Text {
                    color: Theme.primaryColor
                    font.pixelSize: Theme.fontSizeMedium
                    text: _bots.getCountryName(sectionCountry)
                    anchors.verticalCenter: parent.verticalCenter
                }

            }

            Image {
                source: "../images/expand.png"
                height: 31
                width: 30
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: Theme.paddingLarge
                rotation: isExpanded(section) ? 180 : 0
                Behavior on rotation { NumberAnimation { duration: 200 } }
            }

            Rectangle {
                anchors.top: countryNameLine.bottom
                anchors.left: parent.left
                anchors.topMargin: -5
                height: 5
                color: "#44FFFFFF"
                width: parent.width
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    toggle(section)
                }
            }

        }

        delegate:
            BackgroundItem {
                id: delegate
                //height: 80
                anchors.left: parent.left
                anchors.right: parent.right

                height: isExpanded(sort_key) ? 80 : 0
                visible: isExpanded(sort_key) ? true : false

                Item {
                    height: 70
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.rightMargin: Theme.paddingLarge
                    anchors.leftMargin: Theme.paddingLarge
                    anchors.verticalCenter: parent.verticalCenter

                    Image {
                        id: icon
                        height: 60
                        width: 60
                        fillMode: Image.PreserveAspectFit

                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left

                        //visible: typeof model.icon !== 'undefined'
                        source: model.icon || ''
                    }

                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: icon.right
                        anchors.leftMargin: 10
                        text: model.name
                        color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
                    }

                    Component.onCompleted: {
                        var bstatus = _bots.botStatusCompare(model.name, model.revision)
                        checkedUpdated.visible = (bstatus == 2)
                        checkedInstalled.visible = (bstatus >= 1)
                    }

                    Connections {
                        target: _bots.botList
                        onCount_changed: {
                            var bstatus = _bots.botStatusCompare(model.name, model.revision)
                            checkedUpdated.visible = (bstatus == 2)
                            checkedInstalled.visible = (bstatus >= 1)
                        }
                    }

                    Image {
                        id: checkedUpdated
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: checkedInstalled.left
                        source: "../images/new-48.png"
                        height: parent.height*0.8
                        width: height
                        visible: false
                    }

                    Image {
                        id: checkedInstalled
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        source: "../images/approval-48.png"
                        height: parent.height*0.8
                        width: height
                        visible: false
                    }

                }

                onClicked: {
                    pageStack.push(Qt.resolvedUrl("PageBotDownload.qml"), { botData: model } )
                }
            }
    }
}
