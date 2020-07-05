import QtQuick 2.5
import Sailfish.Silica 1.0

Page {
    id: root

    anchors.fill: parent

    property variant remorseItem
    function remorseDelete() {
        remorseDeleteBlock.execute(remorseItem,
                                   qsTr("Deleting %1").arg(remorseItem.name),
                                   function () {
                                       _blocks.deleteBlock(remorseItem.id)
                                   }, 5000)
    }

    Label {
        text: qsTr("No blocks activated")
        anchors.horizontalCenter: root.horizontalCenter
        anchors.verticalCenter: root.verticalCenter
        font.pixelSize: Theme.fontSizeLarge
        visible: blockView.model.count === 0
    }

    SilicaFlickable {
        anchors.fill: parent

        contentHeight: header.height + blockView.height + 50

        PullDownMenu {
            enabled: true

            MenuItem {
                text: qsTr("Add Block Source")
                onClicked: {
                    _blocks.initSources()
                    pageStack.push(Qt.resolvedUrl("PageBlockAddSource.qml"))
                }
            }

            MenuItem {
                text: qsTr("Block Contact")
                onClicked: pageStack.push(Qt.resolvedUrl(
                                              "PageBlockContact.qml"))
            }

            MenuItem {
                text: qsTr("Block Number")
                onClicked: pageStack.push(Qt.resolvedUrl(
                                              "PageBlockAddManual.qml"))
            }

            MenuItem {
                text: qsTr("Refresh")
                onClicked: _blocks.initBlocks()
            }
        }

        PageHeader {
            id: header
            title: qsTr("Call Blocks")
        }

        RemorseItem {
            id: remorseDeleteBlock
        }

        SilicaListView {
            id: blockView
            model: _blocks.db_blocks
            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: contentHeight || 0
            interactive: false
            property Item contextMenu

            delegate: Item {
                id: myListItem
                property bool menuOpen: blockView.contextMenu != null
                                        && blockView.contextMenu.parent === myListItem
                property int type: model.type
                property string number: model.number
                property int id: model.id

                property string name: model.name || model.bot_name
                                      || (model.contact_id ? _blocks.contactName(
                                                                 model.contact_id) : ' ??')

                width: ListView.view.width
                height: menuOpen ? blockView.contextMenu.height
                                   + contentItem.height : contentItem.height

                BackgroundItem {
                    id: contentItem

                    Image {
                        id: blockTypeImage
                        anchors.left: parent.left
                        anchors.leftMargin: Theme.paddingLarge
                        anchors.verticalCenter: parent.verticalCenter
                        height: 48 * sizeRatio
                        width: height
                        sourceSize.height: height
                        sourceSize.width: width
                        fillMode: Image.PreserveAspectCrop
                        source: model.type === 0 ? '../images/edit-6.svg' : model.type
                                                   === 1 ? '../images/contacts.svg' : model.type
                                                           === 2 ? '../images/online.svg' : ''
                        layer.effect: ShaderEffect {
                            property color color: Theme.primaryColor

                            fragmentShader: "
                            varying mediump vec2 qt_TexCoord0;
                            uniform highp float qt_Opacity;
                            uniform lowp sampler2D source;
                            uniform highp vec4 color;
                            void main() {
                            highp vec4 pixelColor = texture2D(source, qt_TexCoord0);
                            gl_FragColor = vec4(mix(pixelColor.rgb/max(pixelColor.a, 0.00390625), color.rgb/max(color.a, 0.00390625), color.a) * pixelColor.a, pixelColor.a) * qt_Opacity;
                            }
                            "
                        }
                    }

                    Column {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: blockTypeImage.right
                        anchors.right: blockCountLabel.left
                        anchors.leftMargin: Theme.paddingMedium
                        Label {
                            text: myListItem.name
                            color: contentItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                        }
                        Label {
                            visible: model.number !== null
                            text: model.number
                            font.pixelSize: Theme.fontSizeExtraSmall
                            color: Theme.secondaryColor
                        }
                    }

                    Label {
                        id: blockCountLabel
                        anchors.right: parent.right
                        text: model.block_count
                        font.bold: true
                        anchors.rightMargin: Theme.paddingLarge
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    onPressAndHold: {
                        remorseItem = myListItem
                        if (!blockView.contextMenu)
                            blockView.contextMenu = contextMenuComponent.createObject(
                                        blockView)
                        blockView.contextMenu.open(myListItem)
                    }

                    onClicked: {
                        _blocks.initHistory(model.id)
                        pageStack.push(Qt.resolvedUrl("PageBlockHistory.qml"))
                    }
                    Component {
                        id: contextMenuComponent
                        ContextMenu {
                            MenuItem {
                                text: qsTr("Edit")
                                enabled: remorseItem.type === 0
                                onClicked: {
                                    blockView.contextMenu.destroy()
                                    pageStack.push(Qt.resolvedUrl(
                                                       "PageBlockEdit.qml"), {
                                                       "contact_id": remorseItem.id,
                                                       "contact_name": remorseItem.name,
                                                       "contact_nbr": remorseItem.number
                                                   })
                                }
                            }
                            MenuItem {
                                text: qsTr("Delete")
                                onClicked: {
                                    remorseDelete()
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
