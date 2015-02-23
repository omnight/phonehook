import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.dbus 1.0

Page {
    id: root

    DBusInterface {
        id: searchService
        destination: "com.omnight.phonehook"
        path: "/"
        iface: "com.omnight.phonehook"

        signalsEnabled: true


        function searchState(state) {
            console.log('search state', state);

            if(state == 'active:person_search' || state == 'activate:business_search') {
                isLoading = true;
            }
        }

        function searchResult(result) {
            console.log('search results!!', result);

            isLoading = false;
            nextParams = {};

            var arr = JSON.parse(result);

            for(var i=0; i < arr.length; i++) {
                if(arr[i].tagname == "result")
                    resultModel.append(arr[i]);
                if(arr[i].tagname == "next") {
                    delete arr[i].tagname;
                    nextParams = arr[i];
                }
            }
        }

    }

    DBusInterface {
        id: dialIf
        destination: 'com.jolla.voicecall.ui'
        iface: 'com.jolla.voicecall.ui'
        path: '/'
    }

    DBusInterface {
        id: mapIf
        destination: 'org.sailfishos.maps'
        iface: 'org.sailfishos.maps'
        path: '/'
    }

    property variant nextParams: ({})
    property bool isLoading: true
    property int botId: 0


    Component.onCompleted: {
        console.log('incoming bot id', botId)
    }

    SequentialAnimation {
        id: clickAnimation

        property variant target

        ScriptAction {
            script: {
                clickAnimation.target.enabled = false;
            }
        }

        NumberAnimation {
            target: clickAnimation.target
            property: "scale"
            from: 1
            to: 0.8
            easing: Easing.InBack
            duration: 150
        }

        NumberAnimation {
            target: clickAnimation.target
            property: "scale"
            from: 0.8
            to: 1
            duration: 200
            easing: Easing.OutBack
        }

        ScriptAction {
            script: {
                clickAnimation.target.enabled = true;
            }
        }

        function startWithTarget(t) {
            target = t;
            start();
        }
    }


    SequentialAnimation {
        id: rowAnimation
        property variant target

        PropertyAction {
            target: rowAnimation.target
            property: "color"
            value: Theme.rgba(Theme.highlightColor, 0.5)
        }

        PauseAnimation { duration: 300 }

        ColorAnimation {
            target: rowAnimation.target
            property: "color"
            to: "#00000000";
            duration: 1000
        }

        function startWithTarget(t) {
            target = t;
            start();
        }
    }


    ListModel {
        id: resultModel
    }


    SilicaFlickable {
        anchors.fill: parent

        PageHeader {
            title: "Results"
            id: header
        }


        Label {
            id: noResultLabel
            visible: !isLoading && resultModel.count == 0
            text: "No results"
            font.pixelSize: Theme.fontSizeLarge
            color: Theme.primaryColor
            anchors.centerIn: parent
        }

        ListView {
            id: resultList
            model: resultModel

            anchors.fill: parent
            anchors.bottomMargin: Theme.paddingLarge
            anchors.topMargin: header.height

            clip: true

            footer: BusyIndicator {
                size: BusyIndicatorSize.Medium
                running: isLoading
                visible: isLoading
                height: 100
                width: 100
                anchors.horizontalCenter: parent.horizontalCenter
            }


            onAtYEndChanged: {
                if(!isLoading && atYEnd && Object.getOwnPropertyNames(nextParams).length > 0) {

                    isLoading = true;

                    console.log('next page', JSON.stringify(nextParams), botId);

                    searchService.call('search', [ nextParams,
                                                   [ botId ] ]);
                }
            }

            delegate: Item {
                anchors.left: parent.left
                anchors.right: parent.right

                height: expanded ? textRows.height + extras.height + Theme.paddingMedium*2: textRows.height + Theme.paddingMedium
                id: listItem

                anchors.leftMargin: Theme.paddingLarge
                anchors.rightMargin: Theme.paddingLarge

                property QtObject pmodel: model

                property bool expanded : false


                Rectangle {
                    id: highlightBox
                    color: "#00000000"
                    height: parent.height
                    anchors.centerIn: parent
                    width: parent.width + Theme.paddingLarge*2
                }

                Image {
                    id: profileImage
                    anchors.left: parent.left
                    height: 60
                    width: 60
                    fillMode: Image.PreserveAspectCrop
                    source: model.image || (model.type == 'person' ?
                                            '../images/contacts-48.png' :
                                            '../images/home-5-48.png')
                }

                Column {
                    id: textRows
                    anchors.left: profileImage.right
                    anchors.leftMargin: Theme.paddingMedium
                    anchors.right: parent.right
                    Text {
                        width: parent.width
                        id: lblName
                        text: model.name
                        color: Theme.primaryColor
                        font.pixelSize: Theme.fontSizeMedium
                        wrapMode: expanded ? Text.Wrap : Text.NoWrap
                        elide: Text.ElideRight
                    }

                    Text {
                        width: parent.width
                        id: lblAddress
                        text: model.address
                        visible: model.address != ''
                        color: Theme.secondaryColor
                        font.pixelSize: Theme.fontSizeSmall
                        wrapMode: expanded ? Text.Wrap : Text.NoWrap
                        elide: Text.ElideRight
                    }
                }


                MouseArea {
                    width: parent.width
                    anchors.fill: textRows
                    onClicked: {
                        listItem.expanded = !listItem.expanded;
                        if(listItem.expanded)
                            rowAnimation.startWithTarget(highlightBox)

                    }
                }

                Column {
                    id: extras
                    anchors.top: textRows.bottom
                    anchors.topMargin: Theme.paddingMedium
                    width: parent.width
                    visible: listItem.expanded
                    spacing: Theme.paddingMedium

                    ListView {
                        height: 0
                        width: parent.width
                        spacing: Theme.paddingMedium
                        model: listItem.pmodel.number_array || (listItem.pmodel.number ? listItem.pmodel : [] )

                        onCountChanged: {
                            if(count > 0)
                                height = Qt.binding(function() { return contentHeight; } )
                        }

                        delegate:
                            Item {

                                width:parent.width
                                height: 60
                                Row {
                                    id: nrRow
                                    anchors.fill: parent
                                    spacing: Theme.paddingMedium
                                    Image {
                                        anchors.verticalCenter: parent.verticalCenter
                                        id: callIcon
                                        height: 48
                                        width: 48
                                        source: '../images/phone-46-48.png'
                                    }

                                    Text {
                                        anchors.verticalCenter: parent.verticalCenter
                                        width: parent.width
                                        id: lblPhone
                                        text: model.number
                                        color: Theme.primaryColor
                                        font.weight: Font.Bold
                                        font.pixelSize: Theme.fontSizeLarge
                                    }
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        clickAnimation.startWithTarget(nrRow)
                                        dialIf.call('dial', [ model.number ]);
                                    }
                                }
                            }
                    }

                    Row {
                        id: itemRow
                        width: parent.width
                        spacing: Theme.paddingLarge
                        height: 86

                        Rectangle {
                            height: 86
                            width: 86
                            id: mapContainer
                            visible: model.number != ''
                            color: "#00000000"

                            Image {
                                id: mapIcon
                                anchors.fill: parent

                                source: 'image://theme/icon-launcher-map'

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        mapIf.call('openAddress', [ model.address, '', '', '', '' ]);
                                        clickAnimation.startWithTarget(mapContainer)
                                    }
                                }

                            }
                        }

                        Rectangle {
                            height: 86
                            width: 86
                            id: urlContainer
                            visible: model.url != ''
                            color: "#00000000"

                            Image {
                                id: urlIcon
                                anchors.fill: parent
                                source: 'image://theme/icon-launcher-browser'

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        clickAnimation.startWithTarget(urlContainer)
                                        Qt.openUrlExternally(model.url);
                                    }
                                }

                            }
                        }

                    }

                    // spacer item
                    Item {
                        width: parent.width
                        height: 0
                    }
                }


            }
        }
    }
}
