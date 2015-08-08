import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    id: root
    property string key:""
    signal remove_hex(string key)
    scale: 0.5
    property real max_opacity: 1
    property bool isExclam: false

    function fadeOut() {
        creatorAnimation.stop()
        destroyerAnimation.start()

    }

    function fadeIn() {
        destroyerAnimation.stop()
        creatorAnimation.stop()

        if(shader.opacity != max_opacity) {
            creatorAnimation.start();
        }
    }


    Component.onDestruction: remove_hex(key)

    SequentialAnimation {
        id: destroyerAnimation
        running:false
        PauseAnimation {
            duration: Math.random() * 500
        }

        ParallelAnimation {
            NumberAnimation {
                target: shader
                property: "opacity"
                from: shader.opacity
                to: 0
                duration: 1000
            }
            PropertyAnimation {
                target: root
                property: "scale"
                to: 0.5
                duration: 1000
            }
        }


        ScriptAction {
            script: root.destroy()
        }
    }

    SequentialAnimation {
        id: creatorAnimation
        running:false
        PauseAnimation {
            duration: Math.random() * 300
        }

        ParallelAnimation {
            PropertyAnimation {
                target: root
                property: "scale"
                to: 1
                duration: 300
            }

            NumberAnimation {
                target: shader
                property: "opacity"
                from: shader.opacity
                to: max_opacity
                duration: 300
            }

            PropertyAnimation {
                target: shader
                property: "innerColor"
                from: "#000"
                to: Qt.darker(Theme.highlightColor,2)
                duration: 300
            }
        }

        PropertyAnimation {
            target: shader
            property: "innerColor"
            from:Qt.darker(Theme.highlightColor,2)
            to: isExclam ? Qt.darker(Theme.highlightColor,2) : Qt.darker(Theme.highlightColor,4)
            duration: 500
        }


    }


    Image {
        id: exclam
        visible: false
        source: "images/exclamation.png"
    }

    ShaderEffect {
        anchors.fill: parent
        blending: true
        id: shader

        opacity: 0
        property color innerColor: "#000" // isExclam ? Qt.darker(Theme.highlightColor,2) : Qt.darker(Theme.highlightColor,4)
        property color borderColor:isExclam ? Qt.darker(Theme.highlightColor, 1.5) : Qt.darker(Theme.highlightColor, 2.5)
        property variant source: isExclam ? exclam : null

        property bool showExclam: isExclam

        fragmentShader: "
#define M_PI 3.1415926535897932384626433832795
varying highp vec2 qt_TexCoord0;

uniform lowp float qt_Opacity;
uniform lowp vec4 innerColor;
uniform lowp vec4 borderColor;
uniform lowp sampler2D source;
uniform bool showExclam;

void main( void ) {

    float border = 0.01;

    float dist = distance(vec2(0.5,0.5),qt_TexCoord0);
    vec4 iColor = innerColor * min(1., (dist/2.)+0.9);

    float borderSlopeX = border / sin(M_PI/6.);

    vec2 rx = qt_TexCoord0.xy;
    float gap = (2.-sqrt(3.))/4.;
    float incline = sqrt(3.);

    if(rx.y < 0.25+border) {
        if(abs(rx.x-0.5) < abs(incline*rx.y) - borderSlopeX)
            gl_FragColor = iColor * qt_Opacity;
        else if(abs(rx.x-0.5) < abs(incline*rx.y))
            gl_FragColor = borderColor * qt_Opacity;
            else gl_FragColor = vec4( 0., 0., 0., 0. );

    } else if(rx.y >= 0.75-border) {

        if(abs(rx.x-0.5) < abs(incline*(rx.y-1.)) - borderSlopeX)
            gl_FragColor = iColor  * qt_Opacity;
            else if(abs(rx.x-0.5) < abs(incline*(rx.y-1.)))
            gl_FragColor = borderColor  * qt_Opacity;
            else gl_FragColor = vec4( 0.,0.,0.0,0.0 );
    }

    if(rx.y >= 0.25 && rx.y < 0.75) {
            if(rx.x > gap+border && rx.x < 1.-gap-border && gl_FragColor != borderColor)
            gl_FragColor = iColor  * qt_Opacity;
        else if(rx.x > gap && rx.x < 1.-gap)
            gl_FragColor = borderColor  * qt_Opacity;
        else gl_FragColor = vec4( 0.,0.,0.0,0.0 );
    }

    if(showExclam) {
        lowp vec4 p;
        p = texture2D(source, qt_TexCoord0);
        gl_FragColor = vec4(p.a) * p + vec4(1.0 - p.a) * gl_FragColor;
    }

}

"
    }
}
