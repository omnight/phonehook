import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.dbus 1.0

Rectangle {

	opacity: 0.7
	color: "#000000"
	width: Screen.width
    height: results.height
	id: rewt
	anchors.top: parent.top
	anchors.topMargin: 100
	
	Component.onCompleted: {
		parent = notificationLayer;
	}
	
	DBusInterface {
        id: dbi
        busType: DBusInterface.SessionBus
        destination: "com.omnight.phonehook"
        path: "/"
        iface: "com.omnight.phonehook"
		signalsEnabled: true		

		signal qmlResult(string data);
		signal lookupStateChanged(string state);
		
		onLookupStateChanged: {
			console.log('GOT LOOKUP STATE!!!', state);
			statusField.text = state;
		}
		
		onQmlResult: {
			console.log('got data', data);
			statusField.text = 'got data';
			var arr = JSON.parse(data);
			for(var i=0; i < arr.length; i++)
			infoModel.append(arr[i]);
		}
		
    }
	
	ListView {
		id: results
		model: infoModel
		width: parent.width		
		anchors.top: rewt.top
		
		onCountChanged: {
			if(count > 0)
				height = Qt.binding(function() { return Math.min(Screen.height / 2, contentHeight); })
		}
		
		
		delegate: Column {
			width: parent.width * 0.9
			anchors.horizontalCenter: parent.horizontalCenter
			
			Text {
				id: text_header
				font.pixelSize: 20
				font.weight: Font.Bold
				text: model.title
				width: parent.width
				color: '#FFFF00'			
				//horizontalAlignment: Text.AlignHCenter
			}
			Text {			
				font.pixelSize: 18
				id: text_content
				text: model.value
				width: parent.width
				color: '#FFFFFF'
				wrapMode: Text.WordWrap
				fontSizeMode: Text.Fit
				maximumLineCount: 5
			}			
		}
	}
	
	ListModel {
		id: infoModel
	}
	
	Rectangle {
		visible: results.count > 0
		width: 50
		height: 50
		color: "#FF0000"
		anchors.top: parent.top
		anchors.right: parent.right
		anchors.topMargin: 10
		anchors.rightMargin: 10
		
		MouseArea {
			anchors.fill: parent
			onClicked: {
				infoModel.clear();
				statusField.text = "";				
			}
		}
	}	
	
	Text {	
		width: parent.width
		anchors.top: results.bottom
		id: statusField
		color: "#FFFFFF"
		horizontalAlignment: Text.AlignHCenter
		text: ""
	}
		

/*
	MouseArea {
		anchors.fill: parent
		onClicked: {
		console.log('clicked');
			infoModel.append([{"title":"test","value":"hej"}]);
			main.cDBI.call("Hello", []);
		}
	}	
	*/
}
