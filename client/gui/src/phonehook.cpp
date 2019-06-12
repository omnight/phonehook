

//#ifdef QT_QML_DEBUG
#include <QtQuick>
//#endif

#include <QtDBus>
#include <sailfishapp.h>

#include "dbus.h"
#include "bots.h"
#include "db_model.h"
#include "blocks.h"
#include "calls.h"
#include "setting.h"

int main(int argc, char *argv[])
{
    // SailfishApp::main() will display "qml/template.qml", if you need more
    // control over initialization, you can use:
    //
    //   - SailfishApp::application(int, char *[]) to get the QGuiApplication *
    //   - SailfishApp::createView() to get a new QQuickView * instance
    //   - SailfishApp::pathTo(QString) to get a QUrl to a resource file
    //
    // To display the view, call "show()" (will show fullscreen on device).


    QGuiApplication* app = SailfishApp::application(argc, argv);
    bots botDb;
    blocks blocks;
    calls calls;

    QQuickView* view = SailfishApp::createView();

    qmlRegisterType<PhSqlModel>("com.omnight.querymodel", 1, 0, "botmodel");


    view->rootContext()->setContextProperty("cppproperty", view);
    view->rootContext()->setContextProperty("_bots", &botDb);
    view->rootContext()->setContextProperty("_blocks", &blocks);
    view->rootContext()->setContextProperty("_calls", &calls);
    view->rootContext()->setContextProperty("_setting", setting::Instance());

    view->setSource(SailfishApp::pathTo("qml/phonehook.qml") );


    view->showFullScreen();

    return app->exec();
}

