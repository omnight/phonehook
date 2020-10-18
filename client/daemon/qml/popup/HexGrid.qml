import QtQuick 2.5
import Sailfish.Silica 1.0

Item {
    property Component hex_comp:  null
    property variant gridpcs: ({})
    property bool largeScreen: Screen.width > 1080
    property bool mediumScreen: (Screen.width > 720 && Screen.width <= 1080)
    property bool smallScreen: (Screen.width >= 720 && Screen.width < 1080)
    property bool smallestScreen: Screen.width < 720
    property int sizeRatio: smallestScreen ? 1 : smallScreen ? 1.5 : 2
    property int hexW: 50 * sizeRatio
    property bool showWarning: false

    id: mw

    property int targetW: 0
    property int targetH: 0

    onTargetHChanged: hexFit()
    onTargetWChanged: hexFit()


    function removeHex(key) {
        var grid = gridpcs;
        delete grid[key];
        gridpcs = grid;
    }

    function hexFit() {
        if(!hex_comp)
            hex_comp = Qt.createComponent("Hex.qml");

        if(hex_comp.status == Component.Loading) {
            hex_comp.statusChanged.connect(hexFit)
            return;
        }

        if(targetW > width) width = targetW;
        if(targetH > height) height = targetH;

        var newGrid = ({});
        var oldGrid = gridpcs;

        var y=0,x=0,yLeft=-1;

        for(var xc = 0; xc <= targetW + hexW*0.5; xc+=hexW*0.866) {       // sqrt(3)/2
            for(var yc = hexW*0.5; yc <= targetH - hexW*0.75; yc+=hexW*0.75) {
                yLeft = Math.floor(((targetH - hexW*0.75) - yc) / (hexW*0.75));
                //console.log(((targetH - hexW*0.5) - yc), (hexW*0.75), yLeft)
//                //console.log(x,y,xc,yc);
                var old = oldGrid ? oldGrid[x+","+y] : undefined;
                if(old) delete oldGrid[x+","+y];

                var opacity_t = (y == 0 || yLeft == 0) ? 0.5 + Math.random() * 0.5 : 1;
                opacity_t = (y == 1 || yLeft == 1) ? 0.9 + Math.random() * 0.1 : opacity_t;

                var warn = (y == 0 && showWarning);

                var hexo;

                if(!old) {
                    hexo = hex_comp.createObject(mw, { x: xc - (y%2)*(hexW*0.866/2),
                                                       y: yc,
                                                       width: hexW,
                                                       height: hexW,
                                                       key: x+","+y});
                    hexo.remove_hex.connect(removeHex);
                } else {
                    hexo = old;
                }

//                console.log(yc, yLeft);

                //console.log('x', xc, 'y', yc);

                hexo.max_opacity = warn ? 1 : opacity_t;
                hexo.isExclam = warn;
                hexo.fadeIn();

                newGrid[x+","+y] = hexo;

                y++;
            }
            y=0;
            x++;
        }

        for(var p_key in oldGrid) {
            var p = oldGrid[p_key];
            p.fadeOut();
            newGrid[p_key] = p;
        }

        gridpcs = newGrid;

    }
}
