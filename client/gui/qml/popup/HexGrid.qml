import QtQuick 2.0

Item {
    property Component hex_comp:  null
    property variant gridpcs: ({})
    property int hexW: 50

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
                var old = oldGrid[x+","+y];
                if(old) delete oldGrid[x+","+y];

                var opacity_t = (y == 0 || yLeft == 0) ? Math.random() : 1;
                opacity_t = (y == 1 || yLeft == 1) ? 0.7 + Math.random() * 0.3 : opacity_t;

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

                hexo.max_opacity = opacity_t;
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
