import QtQuick 2.0
import Sailfish.Silica 1.0

TextField {
    signal valueChanged(string value);

    width: parent.width
    placeholderText: model.title
    text: model.value

    echoMode: TextInput.Password
    onTextChanged: {
        valueChanged(text);
    }
}
