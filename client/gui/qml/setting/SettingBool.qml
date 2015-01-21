import QtQuick 2.0
import Sailfish.Silica 1.0

TextSwitch {
    signal valueChanged(string value);

    id: enabledSwitch
    text: model.title
    checked: model.value === "1"

    onCheckedChanged: {
        valueChanged( checked ? "1" : "0" );
    }

}
