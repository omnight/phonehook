import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: apps

    PageHeader {
        id: header
        title: "App Settings"
    }

    Column {
        id: contentColumn
        anchors.fill: parent
        anchors.margins: Theme.paddingLarge
        anchors.topMargin: header.height

        TextSwitch {
            width: parent.width
            text: "Show only for unknown contacts"
            checked: _bots.querySetting("activate_only_unknown", "true") == "true"
            onCheckedChanged: {
                _bots.setSetting("activate_only_unknown", checked)
            }
        }

        TextSwitch {
            width: parent.width
            text: "Show for incoming SMS"
            checked: _bots.querySetting("activate_on_sms", "false") == "true"
            onCheckedChanged: {
                _bots.setSetting("activate_on_sms", checked)
            }
        }

        TextSwitch {
            width: parent.width
            text: "Enable while roaming"
            checked: _bots.querySetting("enable_roaming", "false") == "true"
            onCheckedChanged: {
                _bots.setSetting("enable_roaming", checked)
            }
        }

        Item {
            height: Theme.paddingLarge
            width: parent.width
        }

        Label {
            text: "Advanced"
            font.pixelSize: Theme.fontSizeLarge
            width: parent.width
        }

        TextSwitch {
            width: parent.width
            text: "Force Compability Mode"
            checked: _bots.querySetting("compability_mode", "false") == "true"
            onCheckedChanged: {
                _bots.setSetting("compability_mode", checked)
            }
        }

        TextSwitch {
            width: parent.width
            text: "Source Test Mode"
            checked: _bots.querySetting("source_test", "false") == "true"
            onCheckedChanged: {
                _bots.setSetting("source_test", checked)
            }
        }



    }






}
