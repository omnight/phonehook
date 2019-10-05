import QtQuick 2.5
import Sailfish.Silica 1.0

Rectangle {
    id: btn
    color: p.pressed ? Theme.highlightColor :  colorAlpha(Qt.darker(Theme.highlightColor, 2), .5)
    width: (parent.width - Theme.paddingLarge) / 2
    height: width


    property string icon
    property string link
    property string text

    Image {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: Theme.paddingLarge*2
        width: parent.width*.4
        height: parent.width*.4
        source: btn.icon
        sourceSize.height: height
        sourceSize.width: width
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
        layer.enabled: true
        layer.samplerName: "source"
    }

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Theme.paddingLarge
        color: Theme.primaryColor
        font.weight: Font.Bold
        font.pixelSize: smallestScreen ? Theme.fontSizeSmall : Theme.fontSizeMedium
        text: btn.text
    }

    MouseArea {
        id: p
        anchors.fill: parent
        onClicked: {
            onClicked: pageStack.push(Qt.resolvedUrl(btn.link))
        }
    }

}
