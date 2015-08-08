import QtQuick 2.0
import Sailfish.Silica 1.0
import QtWebKit.experimental 1.0
import org.nemomobile.dbus 1.0

Page {

    property int bot_id
    property variant login_data

    DBusInterface {
        id: loginService
        destination: "com.omnight.phonehook"
        path: "/"
        iface: "com.omnight.phonehook"
    }

    SilicaWebView {
        id: brw
        anchors.fill: parent

        experimental.preferences.navigatorQtObjectEnabled: true;

        experimental.onMessageReceived: {           
            loginService.call('loginSuccess', [bot_id, {
                                                'tagWanted': login_data.login_success_tag,
                                                'login_http': message.data,
                                                'login_url': url.toString()
                                               }]);
            pageStack.pop();
        }

        Component.onCompleted: {
            experimental.deleteAllCookies();
            url = login_data.url;
        }        

        onLoadingChanged: {
            if (loadRequest.status === WebView.LoadSucceededStatus) {
                console.log('url changed', loadRequest.url.toString());
                if(loadRequest.url.toString().indexOf(login_data.login_success_url) == 0) {
                    _bots.copyCookies(bot_id);

                    if(login_data.login_success_tag) {
                        experimental.evaluateJavaScript("navigator.qt.postMessage(document.documentElement.outerHTML)");
                    } else {
                        pageStack.pop();
                    }
                }
            }
        }
    }
}


