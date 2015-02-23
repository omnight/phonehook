/*
  Copyright (C) 2013 Jolla Ltd.
  Contact: Thomas Perl <thomas.perl@jollamobile.com>
  All rights reserved.

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.dbus 1.0

Page {
    id: topPage

    property variant remorseItem

    function remorseDelete() {
        remorseDeleteBot.execute(remorseItem,
                                 "Deleting " + remorseItem.name,
                                 function() {
                                     console.log('do delete', remorseItem.id);
                                     _bots.removeBot(remorseItem.id);
                                 }, 5000);
    }

    // To enable PullDownMenu, place our content in a SilicaFlickable
    SilicaFlickable {
        anchors.fill: parent

        PageHeader {
            id: header
            title: qsTr("Phonehook")
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
        }


        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView
        PullDownMenu {
            enabled: _bots.daemonActive

            MenuItem {
                text: qsTr("Settings")
                onClicked: {

                    onClicked: pageStack.push(Qt.resolvedUrl("PageAppSettings.qml"))
                }
            }

            MenuItem {
                text: "Add Source"
                onClicked: pageStack.push(Qt.resolvedUrl("PageServerBotList.qml"))
            }

            MenuItem {
                text: "Search"
                onClicked: pageStack.push(Qt.resolvedUrl("PageSearchStart.qml"))
            }

        }

        // Tell SilicaFlickable the height of its content.
        contentHeight: column.height

        // Place our content in a Column.  The PageHeader is always placed at the top
        // of the page, followed by our content.
        Column {
            id: column
            width: topPage.width
            spacing: Theme.paddingLarge
            anchors.margins: Theme.paddingLarge
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: header.bottom

            Label {
                width: parent.width
                text: "Status"
                font.weight: Font.Bold
                font.pixelSize: Theme.fontSizeMedium
                color: Theme.primaryColor
            }

            Text {
                color: "#FFFFFF"
                wrapMode: Text.Wrap
                textFormat: Text.RichText
                text: "Injector Service: <b>" + (_bots.injectorActive ? "Running" : "Not Running") + "</b>"
            }

            Text {
                id: hi
                color: "#FFFFFF"
                wrapMode: Text.Wrap
                textFormat: Text.RichText
                text: "Daemon: <b>" + (_bots.daemonActive ? "Running" : "Not Running") + "</b>"
            }


            Text {
                width: parent.width
                color: "#FFFF88"
                wrapMode: Text.Wrap
                font.pixelSize: Theme.fontSizeSmall
                visible: !_bots.lipstickPatchInstalled
                text: "Homescreen patch not installed. Using compability mode."
            }

            Text {
                width: parent.width
                color: "#FFFFFF"
                wrapMode: Text.Wrap
                visible: !_bots.injectorActive && _bots.lipstickPatchInstalled
                text: "An UI restart is required to complete the installation."
            }

            Button {
                visible: !_bots.injectorActive && _bots.lipstickPatchInstalled
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Restart"

                onClicked: {
                    enabled=false;
                    _bots.restartSystem();
                }
            }

            Button {
                visible: !_bots.daemonActive && _bots.injectorActive
                anchors.horizontalCenter: parent.horizontalCenter

                text: "Start Daemon"

                onClicked: {
                    _bots.startDaemon();
                }
            }

            Item {
                height: 20
                width: parent.width
            }

            Label {
                width: parent.width
                text: "Installed Sources"
                font.weight: Font.Bold
                font.pixelSize: Theme.fontSizeMedium
                color: Theme.primaryColor
            }

            Label {
                text: "None"
                visible: botView.model.count == 0
            }

            RemorseItem {
                anchors.leftMargin: -Theme.paddingLarge
                anchors.rightMargin: -Theme.paddingLarge
                id: remorseDeleteBot
            }

            SilicaListView {
                id: botView
                model: _bots.botList
                width: parent.width
                height: contentHeight || 0
                //anchors.fill: parent
                interactive: false
                enabled: _bots.daemonActive

                //property variant currentItem

                delegate:
                    BackgroundItem {
                        id: delegate
                        property int id
                        property string name

                        Label {
                            anchors.verticalCenter: parent.verticalCenter
                            text: model.name
                            color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
                        }

                        Label {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.right
                            text: 'Rev. ' + model.revision
                            font.pixelSize: Theme.fontSizeSmall
                            color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
                        }

                        onClicked: {
                            remorseItem = delegate;
                            delegate.id = model.id
                            delegate.name = model.name
                            pageStack.push(Qt.resolvedUrl("PageBotDetails.qml"), { botId: model.id })
                            console.log("clicked on bot")
                        }
                    }
                }
            }
        }
}


