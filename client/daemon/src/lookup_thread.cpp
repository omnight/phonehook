#include "lookup_thread.h"
#include "dbus_adapter.h"
#include <QDebug>
#include <QProcess>
#include "robot_base.h"
#include "dbus.h"

lookup_thread::lookup_thread(QObject *parent) :
    QObject(parent) {
}


void lookup_thread::start(QMap<QString,QString> parameters, QList<int> botIds) {
    //connect(&thread, SIGNAL(started()), this, SLOT(threadStarted()));
    connect(this, SIGNAL(start_worker(QMap<QString,QString>,QList<int>)), &w, SLOT(threadStarted(QMap<QString,QString>,QList<int>)) );
    connect(&w, SIGNAL(finished()), this, SLOT(worker_finish()));
    thread.start();
    w.moveToThread(&thread);
    emit start_worker(parameters,botIds);
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



void lookup_worker::threadStarted(QMap<QString,QString> parameters, QList<int> botIds) {

    QString botQuery = "SELECT id, name, xml, country FROM bot WHERE 1=1";

    // test run? (you can test run disabled bots too...)
    if(botIds.length() > 0) {
        botQuery += " AND id IN (";
        foreach(int id, botIds) {
            botQuery += QString::number(id) + ",";
        }

        botQuery.chop(1);
        botQuery += ");";
    } else {
        botQuery += " AND enabled = 1";
    }


    QString tagWanted = "lookup";
    if(parameters.contains("tagWanted")) {
        tagWanted = parameters["tagWanted"];
        // parameters.remove("tagWanted");
    }

    qDebug() << "bot query" << botQuery;

    QSqlQuery sq(botQuery);

    sq.exec();

    while(sq.next()) {

        qDebug() << "got bot!" << sq.value("name").toString();

        if(botIds.length() == 0) {
            if(tagWanted == "lookup" &&
               !sq.value("country").isNull() &&
                sq.value("country").toString().toLower() != "_all" &&
                sq.value("country").toString().toLower() != parameters["countryIso"].toLower()) {
                qDebug() << "incompatible country code for lookup. was" << sq.value("country").toString() << "needed" << parameters["countryIso"];
                continue;
            }
        }

        robot_base rb(sq.value("id").toInt());

        if(!rb.parse(sq.value("xml").toString())) {
            qDebug() << "xml parsing failed. skip robot.";
            continue;
        }

        if(tagWanted == "lookup")
            dbus_adapter::Instance()->set_lookupState("running:" + sq.value("name").toString());

        QSqlQuery sqp("SELECT * FROM bot_param WHERE bot_id = " + sq.value("id").toString() );
        sqp.exec();

        while(sqp.next()) {
            parameters.insert(sqp.value("key").toString(), sqp.value("value").toString());
        }

        sqp.finish();

        QDomDocument result = rb.run(parameters, tagWanted);

        qDebug() << "xml result";
        qDebug() << result.toString();

        QJsonArray displayArray;

        // output of this robot is input to next robot
        if(result.documentElement().hasChildNodes()) {
            QDomElement root = result.documentElement();

            QDomNodeList fields = root.childNodes();
            for(int i=0; i < fields.length(); i++) {
                if(!fields.at(i).isElement()) continue;

                QDomNodeList attr = fields.at(i).toElement().childNodes();

                QJsonObject displayData;

                for(int j=0; j < attr.length(); j++) {
                    if(!attr.at(j).isElement()) continue;
                    QDomElement e = attr.at(j).toElement();


                    // multiple values with same key --
                    // first value is inserted with key name
                    // other values are inserted as array (
                    if(displayData.contains(e.tagName())) {
                        QJsonArray a;
                        QJsonObject ae;

                        if(displayData.contains(e.tagName() + "_array")) {
                            a = displayData.value( e.tagName() + "_array" ).toArray();
                        } else {
                            ae[e.tagName()] = displayData.value(e.tagName()).toString();
                            a.append( ae );
                        }

                        if(e.tagName() == "number")
                            ae[e.tagName()] = phonenumber::convertNumber( e.text(),
                                                                          sq.value("country").toString(),
                                                                          phonenumber::mobilecc_to_iso32662( dbus::Instance()->mobileCountryCode() ));
                        else
                            ae[e.tagName()] = e.text();

                        a.append(ae);
                        displayData[ e.tagName() + "_array" ]  = a;

                    } else {
                        if(e.tagName() == "number")
                            displayData[e.tagName()] = phonenumber::convertNumber( e.text(),
                                                                          sq.value("country").toString(),
                                                                          phonenumber::mobilecc_to_iso32662( dbus::Instance()->mobileCountryCode() ) );
                        else
                            displayData[ e.tagName() ] = e.text();
                    }
                }

                if(tagWanted == "lookup")
                    displayData[ "source" ] = sq.value("name").toString();

                displayData[ "tagname" ] = fields.at(i).toElement().tagName();

                if(displayData.contains("paramname") &&
                   displayData.contains("value") &&
                   !parameters.contains( displayData["paramname"].toString() )) {
                    parameters.insert(displayData["paramname"].toString(), displayData["value"].toString());
                }

                displayArray.append(displayData);

            }
        }

        QJsonDocument displayDataDoc(displayArray);

        if(tagWanted == "lookup") {
            dbus_adapter::Instance()->sendQml( displayDataDoc.toJson() );
        } else {
            qDebug() << "sending result signal";
            qDebug() << displayDataDoc.toJson();
            QMetaObject::invokeMethod(dbus_adapter::Instance(),
                                      "searchResult", Qt::QueuedConnection,
                                      Q_ARG(QString,displayDataDoc.toJson()));
        }
    }

    sq.finish();
//    db.close();


    // check if popup should timeout
    QSqlQuery ptq("SELECT value FROM setting WHERE key = 'popup_timeout'");
    ptq.exec();

    int popupTimeout = 0;
    while(ptq.next()) {
        popupTimeout = ptq.value("value").toInt() * 1000;
    }

    ptq.finish();

    if(popupTimeout != 0)
        dbus_adapter::Instance()->messageToPopup("hideIn", QString::number(popupTimeout));


    if(tagWanted == "lookup")
        dbus_adapter::Instance()->set_lookupState("finished");

    qDebug() << "lookup finished?";
    emit finished();


}
