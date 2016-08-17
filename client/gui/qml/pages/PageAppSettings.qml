import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: apps

    SilicaFlickable {

        contentHeight: contentColumn.height + header.height + 40
        anchors.fill: parent

        PageHeader {
            id: header
            title: qsTr("App Settings")
        }

        Column {
            id: contentColumn

            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: 10

            anchors.margins: Theme.paddingLarge


            Label {
                text: qsTr("Basic")
                font.pixelSize: Theme.fontSizeLarge
            }

            TextSwitch {
                width: parent.width
                text: qsTr("Auto Update Sources")
                checked: _setting.get("auto_update_enabled", "true") == "true"
                onCheckedChanged: {
                    _setting.put("auto_update_enabled", checked)
                }
            }

            TextSwitch {
                width: parent.width
                text: qsTr("Show only for unknown contacts")
                checked: _setting.get("activate_only_unknown", "true") == "true"
                onCheckedChanged: {
                    _setting.put("activate_only_unknown", checked)
                }
            }

            TextSwitch {
                width: parent.width
                text: qsTr("Old popup design")
                checked: _setting.get("old_popup_design", "false") == "true"
                onCheckedChanged: {
                    _setting.put("old_popup_design", checked)
                }
            }
    /*
            TextSwitch {
                width: parent.width
                text: "Show for incoming SMS"
                checked: _setting.get("activate_on_sms", "false") == "true"
                onCheckedChanged: {
                    _setting.put("activate_on_sms", checked)
                }
            }
    */
            TextSwitch {
                width: parent.width
                text: qsTr("Enable while roaming")
                checked: _setting.get("enable_roaming", "false") == "true"
                onCheckedChanged: {
                    _setting.put("enable_roaming", checked)
                }
            }

            Item {
                height: Theme.paddingLarge
                width: parent.width
            }

            Label {
                text: qsTr("Popup Timeout")
                font.pixelSize: Theme.fontSizeLarge
                anchors.left: parent.left
            }

            Slider {
                width: parent.width
                value: _setting.get("popup_timeout", "0")
                valueText: value == 0 ? qsTr("Disabled") : value + qsTr(" s")
                stepSize: 10
                maximumValue: 120
                onValueChanged: {
                    _setting.put("popup_timeout", value)
                }
            }

            Item {
                height: Theme.paddingLarge
                width: parent.width
            }

            Label {
                text: qsTr("Advanced")
                font.pixelSize: Theme.fontSizeLarge
                width: parent.width
            }

/*            TextSwitch {
                width: parent.width
                text: qsTr("Source Test Mode")
                checked: _setting.get("source_test", "false") == "true"
                onCheckedChanged: {
                    _setting.put("source_test", checked)
                }
            }*/

            TextSwitch {
                width: parent.width
                text: qsTr("Kill dialer when blocking call")
                checked: _setting.get("kill_voicecall_ui", "false") == "true"
                onCheckedChanged: {
                    _setting.put("kill_voicecall_ui", checked)
                }
            }



            Button {
                height: 70
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Restart Daemon")
                onClicked: _bots.startDaemon()
            }


        }

    }





}
