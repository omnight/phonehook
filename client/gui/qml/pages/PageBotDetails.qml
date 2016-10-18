import QtQuick 2.0
import Sailfish.Silica 1.0
import "../setting"
import org.nemomobile.dbus 1.0

Page {
    id: root

    property int botId: 0
    property variant botModel: null

    SilicaFlickable {
        anchors.fill: parent

        contentHeight: contentColumn.height + header.height + 40

        PageHeader {
            title: botModel.name
            id: header
        }


        PullDownMenu {

            MenuItem {
                text: qsTr("Report a Problem")
                onClicked: {
                    Qt.openUrlExternally("https://github.com/omnight/phonehook/issues")
                }
            }

            MenuItem {
                text: qsTr("Clear Cache")
                onClicked: {
                    onClicked: _bots.clearCache(botId)
                }
            }

            MenuItem {
                text: qsTr("Test Number")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("PageBotTest.qml"), { name: botModel.name, botId: botModel.id })
                }
            }

            MenuItem {
                text: qsTr("Remove")
                onClicked: {
                    pageStack.find(function(p) {
                        if(p.remorseDelete) {
                            p.remorseDelete();
                        }
                    })
                    pageStack.pop();
                }
            }

        }

        Component.onCompleted: {

            console.log('CREATING DETAILS PAGE')

            //console.log('model', botModel);
            botModel = _bots.getBotDetails(botId);
            _bots.setActiveBot(botId);
            params.model = _bots.paramList;
            logins.model = _bots.loginList;
            //enabledSwitch.checked = (botModel.enabled === 1);
        }

        Column {
            id: contentColumn
            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: 10
            anchors.margins: Theme.paddingLarge

            Label {
                font.pixelSize: Theme.fontSizeExtraSmall
                anchors.right: parent.right
                text: "Rev. " + botModel.revision
            }

            Label {
                width: parent.width
                text: qsTr("Bot Settings")
                font.weight: Font.Bold
                font.pixelSize: Theme.fontSizeMedium
                color: Theme.primaryColor
            }

            Label {
                text: qsTr("None")
                visible: params.model.count == 0 && !_bots.hasBlockTag(botId)
            }

            ListView {
                id: params
                width: parent.width
                interactive: false
                height: contentHeight

                delegate: Loader {
                    id: cpLoader
                    width: parent.width
                    Component.onCompleted: {
                        if(model.type === "string") source = "../setting/SettingString.qml";
                        if(model.type === "password") source = "../setting/SettingPassword.qml";
                        if(model.type === "bool") source = "../setting/SettingBool.qml";
                    }

                    Connections {
                        ignoreUnknownSignals: true
                        target: cpLoader.item

                        onValueChanged: {
                            _bots.setBotParam(model.bot_id,model.key, value);
                        }
                    }
                }
            }


            SettingBool {
                visible: _bots.hasBlockTag(botId)
                text: qsTr('Enable Auto-blocking')
                checked: _bots.isBlockSource(botId)
                onCheckedChanged: {
                    _bots.setBlockSource(botId, checked)
                }
            }

            Label {
                width: parent.width
                text: qsTr("Login")
                font.weight: Font.Bold
                font.pixelSize: Theme.fontSizeMedium
                color: Theme.primaryColor
                visible: logins.model.count > 0
            }


            ListView {
                id: logins
                width: parent.width
                interactive: false
                height: contentHeight

                delegate: Item {
                    width: parent.width
                    height: 70
                    Button {
                        anchors.centerIn: parent
                        text: model.name
                        onClicked: {

                            var login_data = JSON.stringify(model, function replacer(key, value) {
                                if (["model", "objectName","hasModelChildren","index"].indexOf(key) != -1) {
                                  return undefined;
                                }
                                return value;
                          });

                            console.log(login_data);

                            pageStack.push(Qt.resolvedUrl("PageOAuth.qml"),
                               {
                                   'bot_id': botModel.id,
                                   'login_data': model
                               });
                        }
                    }
                }
            }


            Item {
                height: 20
                width: parent.width
            }


            Label {
                width: parent.width
                text: qsTr("Description")
                font.weight: Font.Bold
                font.pixelSize: Theme.fontSizeMedium
                color: Theme.primaryColor
            }


            Text {
                width: parent.width
                color: Theme.secondaryColor
                text: botModel.description
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
            }

            Item {
                height: 20
                width: parent.width
            }


            Label {
                width: parent.width
                text: qsTr("Link")
                visible: botModel.link != ""
                font.weight: Font.Bold
                font.pixelSize: Theme.fontSizeMedium
                color: Theme.primaryColor
            }


            Text {
                width: parent.width
                color: Theme.secondaryColor
                text: botModel.link
                visible: botModel.link != ""
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                font.underline: true
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        Qt.openUrlExternally(botModel.link);
                    }
                }
            }

        }
    }
}
