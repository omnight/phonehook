import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.dbus 1.0

Dialog {
    id: apps

    property int selectedBotId: 0
    property string selectedTag: 'person_search'
    acceptDestinationAction: PageStackAction.Push
    acceptDestination: Qt.resolvedUrl("PageSearchResults.qml")

    canAccept: inputName.text.length > 0 && selectedBotId != 0


    DialogHeader {
        id: header
        acceptText: qsTr("Search")
    }

    SilicaFlickable {
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        contentHeight: contentColumn.height + 40
        clip: true

        Column {
            id: contentColumn
            anchors.margins: Theme.paddingLarge
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top


            TextField {
                id: inputName
                width: parent.width
                placeholderText: qsTr("What?")
            }

            TextField {
                id: inputLocation
                width: parent.width
                placeholderText: qsTr("Where?")
            }

            Label {
                width: parent.width
                text: qsTr("Search for")
                font.weight: Font.Bold
                font.pixelSize: Theme.fontSizeMedium
                color: Theme.primaryColor
            }

            Row {
                width: parent.width
                TextSwitch {
                    id: searchPeopleCheck
                    width: parent.width/2
                    checked: true
                    onCheckedChanged: {
                        if(checked) {
                            selectedTag = 'person_search';
                            searchBusinessCheck.checked = false;
                            _bots.setBotSearchListTag('person_search');
                        } else {
                            checked = !searchBusinessCheck.checked
                        }
                    }
                    text: qsTr('People')
                }

                TextSwitch {
                    id: searchBusinessCheck
                    //width: parent.width/2
                    checked: false
                    onCheckedChanged: {
                        if(checked) {
                            selectedTag = 'business_search';
                            searchPeopleCheck.checked = false;
                            _bots.setBotSearchListTag('business_search');
                        } else {
                            checked = !searchPeopleCheck.checked
                        }
                    }
                    text: qsTr('Businesses')
                }
            }



            Item {
                height: 20
                width: parent.width
            }

            Label {
                width: parent.width
                //: referring to what source to use
                text: qsTr("Search at")
                font.weight: Font.Bold
                font.pixelSize: Theme.fontSizeMedium
                color: Theme.primaryColor
            }


            Item {
                height: 20
                width: parent.width
                visible: botView.model.count == 0
            }

            Label {
                width: parent.width
                wrapMode: Text.Wrap
                text: qsTr("There are no installed sources with search capability")
                font.pixelSize: Theme.fontSizeSmall
                visible: botView.model.count == 0
            }

            ListView {
                id: botView
                width: parent.width
                height: contentHeight || 0
                //anchors.fill: parent
                interactive: false

                delegate:
                    BackgroundItem {
                        id: delegate

                        TextSwitch {
                            width: parent.width
                            text: model.name || ''
                            checked: selectedBotId == model.id
                            onCheckedChanged: {
                                console.log('checked to', model.id, checked);
                                if(checked)
                                    selectedBotId = model.id;

                                checked = Qt.binding(function() { return selectedBotId == model.id; });
                            }
                        }
                    }
                }
            }
    }

    DBusInterface {
        id: searchService
        destination: "com.omnight.phonehook"
        path: "/"
        iface: "com.omnight.phonehook"
    }

    onAccepted: {
        acceptDestinationProperties = { botId : selectedBotId }


        console.log('accepted', selectedBotId);
        searchService.call('search', [ {'tagWanted': selectedTag,
                                        'searchName': inputName.text.trim(),
                                        'searchArea': inputLocation.text.trim()},
                                        [ selectedBotId ] ]);
    }

    Component.onCompleted: {
        _bots.setBotSearchListTag('person_search');
        botView.model = _bots.botSearchList;
    }

    Connections {
        target: _bots
        onBotSearchList_changed: {

            // validate selection
            var valid = false;
            for(var i=0; i < _bots.botSearchList.count; i++) {
                valid = valid || (_bots.botSearchList.getValue(i, "id") == selectedBotId);
            }

            if(!valid)
                selectedBotId = _bots.botSearchList.getValue(0, "id");

        }
    }


}
