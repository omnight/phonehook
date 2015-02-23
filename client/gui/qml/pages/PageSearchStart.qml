import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.dbus 1.0

Dialog {
    id: apps

    DialogHeader {
        id: header
        acceptText: "Search"
    }


    property int selectedBotId: 0
    property string selectedTag: 'person_search'
    acceptDestinationAction: PageStackAction.Push

    canAccept: inputName.text.length > 0 && selectedBotId != 0

    DBusInterface {
        id: searchService
        destination: "com.omnight.phonehook"
        path: "/"
        iface: "com.omnight.phonehook"
    }

    onAccepted: {
        acceptDestinationProperties = { botId : selectedBotId }
        acceptDestination = Qt.resolvedUrl("PageSearchResults.qml")

        console.log('accepted', selectedBotId);
        searchService.call('search', [ {'tagWanted': selectedTag,
                                        'searchName': inputName.text.trim(),
                                        'searchArea': inputLocation.text.trim()},
                                        [ selectedBotId ] ]);
    }

    Component.onCompleted: {
        _bots.setBotSearchListTag('person_search');
        botView.model = _bots.botSearchList;
        selectedBotId = _bots.botSearchList.getValue(0, "id");
    }

    Column {
        id: contentColumn
        anchors.fill: parent
        anchors.margins: Theme.paddingLarge
        anchors.topMargin: header.height

        TextField {
            id: inputName
            width: parent.width
            placeholderText: "What?"
        }

        TextField {
            id: inputLocation
            width: parent.width
            placeholderText: "Where?"
        }

        Label {
            width: parent.width
            text: "Search for"
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
                    } else {
                        checked = !searchBusinessCheck.checked
                    }
                }
                text: 'People'
            }

            TextSwitch {
                id: searchBusinessCheck
                width: parent.width/2
                checked: false
                onCheckedChanged: {
                    if(checked) {
                        selectedTag = 'business_search';
                        searchPeopleCheck.checked = false;
                    } else {
                        checked = !searchPeopleCheck.checked
                    }
                }
                text: 'Businesses'
            }
        }



        Item {
            height: 20
            width: parent.width
        }

        Label {
            width: parent.width
            text: "Search at"
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
            text: "There are no installed sources with search capability"
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
