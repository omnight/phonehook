import QtQuick 2.0

Item {
    id: root
    property string key:""
    signal remove_hex(string key)

    property real max_opacity: 1

    function fadeOut() {
        creatorAnimation.stop()
        destroyerAnimation.start()

    }

    function fadeIn() {
        destroyerAnimation.stop()
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
        NumberAnimation {
            target: shader
            property: "opacity"
            from: shader.opacity
            to: 0
            duration: 1000
        }
        ScriptAction {
            script: root.destroy()
        }
    }

    SequentialAnimation {
        id: creatorAnimation
        running:false
        PauseAnimation {
            duration: Math.random() * 500
        }

        NumberAnimation {
            target: shader
            property: "opacity"
            from: shader.opacity
            to: max_opacity
            duration: 1000
        }
    }

    ShaderEffect {
        anchors.fill: parent
        blending: true
        id: shader

        opacity: 0


        fragmentShader: "
#define M_PI 3.1415926535897932384626433832795
varying highp vec2 qt_TexCoord0;

uniform lowp float qt_Opacity;

void main( void ) {

    float border = 0.01;
    vec4 innerColor = vec4(0.95,0.95,0.95,1.);

    float dist = distance(vec2(0.5,0.5),qt_TexCoord0);
    innerColor = innerColor * min(1., (dist/2.)+0.9);

    vec4 borderColor = vec4(1.,1.,1.,1.);
    float borderSlopeX = border / sin(M_PI/6.);

    vec2 rx = qt_TexCoord0.xy;
    float gap = (2.-sqrt(3.))/4.;
    float incline = sqrt(3.);

    if(rx.y < 0.25+border) {
        if(abs(rx.x-0.5) < abs(incline*rx.y) - borderSlopeX)
            gl_FragColor = innerColor * qt_Opacity;
        else if(abs(rx.x-0.5) < abs(incline*rx.y))
            gl_FragColor = borderColor * qt_Opacity;
            else gl_FragColor = vec4( 0., 0., 0., 0. );

    } else if(rx.y >= 0.75-border) {

        if(abs(rx.x-0.5) < abs(incline*(rx.y-1.)) - borderSlopeX)
            gl_FragColor = innerColor  * qt_Opacity;
            else if(abs(rx.x-0.5) < abs(incline*(rx.y-1.)))
            gl_FragColor = borderColor  * qt_Opacity;
            else gl_FragColor = vec4( 0.,0.,0.0,0.0 );
    }

    if(rx.y >= 0.25 && rx.y < 0.75) {
            if(rx.x > gap+border && rx.x < 1.-gap-border && gl_FragColor != borderColor)
            gl_FragColor = innerColor  * qt_Opacity;
        else if(rx.x > gap && rx.x < 1.-gap)
            gl_FragColor = borderColor  * qt_Opacity;
        else gl_FragColor = vec4( 0.,0.,0.0,0.0 );
    }



}

"
    }
}
