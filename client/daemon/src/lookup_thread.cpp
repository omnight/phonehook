#include "lookup_thread.h"
#include "dbus_adapter.h"
#include <QDebug>
#include <QProcess>
#include "robot_base.h"
#include "dbus.h"
#include "normalize_phonenumber.h"

lookup_thread::lookup_thread(QObject *parent) :
    QObject(parent) {
}


void lookup_thread::start(QString number, int botId) {
    //connect(&thread, SIGNAL(started()), this, SLOT(threadStarted()));
    connect(this, SIGNAL(start_worker(QString,int)), &w, SLOT(threadStarted(QString,int)) );
    connect(&w, SIGNAL(finished()), this, SLOT(worker_finish()));
    thread.start();
    w.moveToThread(&thread);
    emit start_worker(number,botId);
}

void lookup_thread::worker_finish() {
    qDebug() << "thread finished?";
    this->deleteLater();
}

lookup_thread::~lookup_thread() {
    qDebug() << "lookup_thread is destroy";
    thread.exit();
    thread.wait();
}

void lookup_worker::threadStarted(QString number, int botId) {

    robot_base rb(botId);

    QString botQuery = "SELECT id, name, xml FROM bot WHERE 1=1";

    // test run? (you can test run disabled bots too...)
    if(botId != 0) botQuery += " AND id = " + QString::number(botId);
    else           botQuery += " AND enabled = 1";

    qDebug() << "bot query" << botQuery;

    QSqlQuery sq(botQuery);

    sq.exec();

    while(sq.next()) {

        qDebug() << "got bot!";

        dbus_adapter::Instance()->set_lookupState("running:" + sq.value("name").toString());

        if(!rb.parse(sq.value("xml").toString())) {
            qDebug() << "xml parsing failed. skip robot.";
            continue;
        }

        QMap<QString,QString> params;
        params.insert("telnr", number);           // this parameter is always given
        params.insert("countryIso", normalize_phonenumber::mobilecc_to_iso32662( dbus::Instance()->mobileCountryCode() ) );

        QSqlQuery sqp("SELECT * FROM bot_param WHERE bot_id = " + sq.value("id").toString() );
        sqp.exec();

        while(sqp.next()) {
            params.insert(sqp.value("key").toString(), sqp.value("value").toString());
        }

        sqp.finish();


        QDomDocument result = rb.run(params);

        qDebug() << "xml result";
        qDebug() << result.toString();

        QJsonArray displayArray;

        // output of this robot is input to next robot
        if(result.documentElement().hasChildNodes()) {
            QDomElement root = result.documentElement();

            QDomNodeList fields = root.elementsByTagName("field");
            for(int i=0; i < fields.length(); i++) {
                if(!fields.at(i).isElement()) continue;
                QDomNodeList attr = fields.at(i).toElement().childNodes();

                QJsonObject displayData;

                for(int j=0; j < attr.length(); j++) {
                    if(!attr.at(j).isElement()) continue;
                    QDomElement e = attr.at(j).toElement();
                    displayData[ e.tagName() ] = e.text();
                }

                displayData[ "source" ] = sq.value("name").toString();

                if(displayData.contains("paramname") &&
                   displayData.contains("value") &&
                   !params.contains( displayData["paramname"].toString() )) {
                    params.insert(displayData["paramname"].toString(), displayData["value"].toString());
                }

                displayArray.append(displayData);

            }
        }

        QJsonDocument displayDataDoc(displayArray);
        dbus_adapter::Instance()->sendQml( displayDataDoc.toJson() );
    }

    sq.finish();
//    db.close();

    dbus_adapter::Instance()->set_lookupState("finished");

    qDebug() << "lookup finished?";
    emit finished();


}
