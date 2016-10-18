import QtQuick 2.0
import Sailfish.Silica 1.0
import QtWebKit.experimental 1.0
import org.nemomobile.dbus 1.0

Dialog {

    id: pageRoot
    property int bot_id
    property variant login_data
    property string title

    Component.onCompleted: {

        brw.experimental.deleteAllCookies();


        if(login_data.url) {        // backward compability
            brw.url = login_data.url
            login_data.done_url = login_data.done_url || login_data.login_success_url
        } else {                    // new method - script runs both before and after showing webpage
            loginService.call('login', [bot_id, {
                'tagWanted': login_data.login_success_tag,
            }]);
            robotRunning.running = true;
        }

    }


    DBusInterface {
        id: loginService
        destination: "com.omnight.phonehook"
        path: "/"
        iface: "com.omnight.phonehook"
        signalsEnabled: true

        function searchState(state) {
        }

        function searchResult(result) {

            robotRunning.running = false;

            console.log(result);
            result = JSON.parse(result);

            for(var i=0; i < result.length; i++) {

                if(result[i].tagname != 'result')
                    continue;

                if(result[i].success) {
                    pageStack.pop()
                    break;
                }

                login_data.done_url = result[i].done_url;
                login_data.next_tag = result[i].next_tag;

                if(result[i].html) {
                    brw.loadHtml(result[i].html, "https://www.google.com");
                } else {
                    brw.url = result[i].url;
                }

                console.log('set title', pageRoot.title, result[i].name)
                pageRoot.title = result[i].name;

                break;
            }
        }
    }

    SilicaWebView {
        id: brw
        anchors.fill: parent

        header: PageHeader {
            id: brwHeader
            title: pageRoot.title
            //height: btn.height

            Label{
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 10
                text: qsTr("Cancel")
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        pageStack.pop()
                    }
                }
            }
        }

        experimental.preferences.navigatorQtObjectEnabled: true;

        experimental.itemSelector:Component {
            Item {
                Component.onCompleted: {
                    var dialog = pageStack.push(Qt.resolvedUrl("ItemSelector2.qml"), {
                                                       model: model.items
                                                });
                    dialog.onRejected.connect(function() {
                        model.reject();
                    });
                    dialog.onAccepted.connect(function() {
                        model.accept(dialog.selectedIndex);
                    });
                }
            }
        }

        experimental.onMessageReceived: {           
            loginService.call('login', [bot_id, {
                                                'tagWanted': login_data.login_success_tag,
                                                'html': message.data,
                                                'url': url.toString()
                                               }]);

        }

        /*_page.onResourceRequested: function (resource) {
            log('resource requested: ' + resource.url);
        }

        _page.onResourceReceived: function (resource) {
            log('resource received: ' + resource.status + ' ' + resource.statusText + ' ' +
                resource.contentType + ' ' + resource.url);
        }*/

        Component.onCompleted: {

        }        

        onLoadingChanged: {
            if (loadRequest.status === WebView.LoadSucceededStatus) {
                console.log('url changed', loadRequest.url.toString());
                if(loadRequest.url.toString().indexOf(login_data.done_url) == 0) {
                    _bots.copyCookies(bot_id);

                    if(login_data.next_tag) {
                        experimental.evaluateJavaScript("navigator.qt.postMessage(document.documentElement.outerHTML)");
                    } else {
                        pageStack.pop();
                    }
                }
            }
        }
    }

    BusyIndicator {
        id: robotRunning
        size: BusyIndicatorSize.Large
        anchors.centerIn: parent
        visible: running
        running: false
    }
}


