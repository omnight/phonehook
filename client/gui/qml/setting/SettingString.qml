import QtQuick 2.5
import Sailfish.Silica 1.0

TextField {
    signal valueChanged(string value);

    width: parent.width
    placeholderText: model.title
    text: model.value 

    onTextChanged: {
        console.log(model.key, 'value changed');
        valueChanged(text);
    }

    Component.onCompleted: {
        //console.log('model is', JSON.stringify(model));
    }

}
