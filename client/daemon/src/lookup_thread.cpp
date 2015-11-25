#include "lookup_thread.h"
#include "dbus_adapter.h"
#include <QDebug>
#include <QProcess>
#include "robot_base.h"
#include "dbus.h"
#include "setting.h"
#include "blocking.h"
#include <QSqlError>

lookup_thread::lookup_thread(QObject *parent) :
    QObject(parent) {
}


void lookup_thread::start(QMap<QString,QString> parameters, QList<int> botIds) {
    //connect(&thread, SIGNAL(started()), this, SLOT(threadStarted()));
    connect(this, SIGNAL(start_worker(QMap<QString,QString>,QList<int>)), &w, SLOT(threadStarted(QMap<QString,QString>,QList<int>)) );
    connect(&w, SIGNAL(finished()), this, SLOT(worker_finish()));
    connect(&w, SIGNAL(gotResult()), this, SLOT(worker_result()));
    thread.start();
    w.moveToThread(&thread);
    emit start_worker(parameters,botIds);
}

void lookup_thread::worker_result() {
    emit gotResult(this);
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

    // run bots with auto-block capability first
    QString botQuery = R"(
            SELECT bot.id, bot.name, bot.xml, bot.country FROM bot
            LEFT JOIN block ON block.bot_id = bot.id
            WHERE 1=1
    )";

    // test run? (you can test run disabled bots too...)
    if(botIds.length() > 0) {
        botQuery += " AND bot.id IN (";
        foreach(int id, botIds) {
            botQuery += QString::number(id) + ",";
        }

        botQuery.chop(1);
        botQuery += ")";
    } else {
        botQuery += " AND enabled = 1";
    }

    botQuery += R"(
        ORDER BY COALESCE(block.bot_id,9999),bot.id
    )";


    QString tagWanted = "lookup";
    if(parameters.contains("tagWanted")) {
        tagWanted = parameters["tagWanted"];
        // parameters.remove("tagWanted");
    }

    // delete previous vCard
    if(QFile::exists("/home/nemo/.phonehook/phonehook.vcf"))
        QFile::remove("/home/nemo/.phonehook/phonehook.vcf");

    vCardData.clear();
    vCardData["telnr"] = parameters["telnrInt"];

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


        QSqlQuery cacheQ;
        cacheQ.prepare("SELECT * FROM bot_result_cache WHERE bot_id=? AND telnr IN (?,?)");
        cacheQ.addBindValue(sq.value("id").toInt());
        cacheQ.addBindValue(parameters["telnr"]);
        cacheQ.addBindValue(parameters["telnrInt"]);

        cacheQ.exec();

        QDomDocument result;

        if(cacheQ.next()) {
            result.setContent( QString::fromUtf8( qUncompress( cacheQ.value("result").toByteArray() ) )  );
        } else {

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

            result = rb.run(parameters, tagWanted);

            bool block = result.documentElement().elementsByTagName("block").count() > 0;

            if(result.documentElement().elementsByTagName("nocache").count() == 0) {
                QSqlQuery cacheInsert;
                cacheInsert.prepare("INSERT INTO bot_result_cache (bot_id, telnr, result, block) VALUES(:id,:telnr,:result,:block)");
                cacheInsert.bindValue(":id", sq.value("id").toInt());
                cacheInsert.bindValue(":telnr", parameters["telnrInt"]);
                cacheInsert.bindValue(":result", qCompress( result.toString().toUtf8() ));
                cacheInsert.bindValue(":block", (int)block);
                if(!cacheInsert.exec()) {
                    qDebug() << sq.value("id").toInt();
                    qDebug() << parameters["telnrInt"];
                    qDebug() << result.toString();
                    qDebug() << block;
                    qDebug() << "cache insert error" << cacheInsert.lastError();
                }
            }
        }

        emit gotResult();

        qDebug() << "xml result";
        qDebug() << result.toString();

        // update contact card



        updateVCard(result);

        // save result in db cache

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

    // check if popup should timeout
    int popupTimeout = setting::get("popup_timeout","0").toInt() * 1000;

    if(popupTimeout != 0)
        dbus_adapter::Instance()->messageToPopup("hideIn", QString::number(popupTimeout));


    if(tagWanted == "lookup")
        dbus_adapter::Instance()->set_lookupState("finished");

    qDebug() << "lookup finished?";
    emit finished();


}



void lookup_worker::updateVCard(QDomDocument data) {

/*
 * method call sender=:1.333 -> dest=com.jolla.contacts.ui serial=52 path=/com/jolla/contacts/ui; interface=com.jolla.contacts.ui; member=importContactFile
   string "file:///home/nemo/vtest.vcf"
 */

// dbus-send --session --type=method_call --print-reply --dest=com.jolla.contacts.ui /com/jolla/contacts/ui com.jolla.contacts.ui.importContactFile string:"file://home/nemo/.phonehook/phonehook.vcf"

    QDomNodeList nodeList = data.elementsByTagName("field");

    for(int i=0; i < nodeList.count(); i++) {

        QDomNode node = nodeList.at(i);

        QDomNodeList titleList = node.toElement().elementsByTagName("stitle");
        QDomNodeList valueList = node.toElement().elementsByTagName("value");
        if(titleList.count() != 1 || valueList.count() != 1)
            continue;

        QString title = titleList.at(0).toElement().text();
        QString value = valueList.at(0).toElement().text();


        if(!vCardData.contains(title))
            vCardData[title] = value;
    }

    QFile vCardFile("/home/nemo/.phonehook/phonehook.vcf");
    if(vCardFile.open(QIODevice::WriteOnly)) {
        QTextStream stream(&vCardFile);
        stream.setCodec("UTF-8");
        stream << "BEGIN:VCARD" << endl <<
                  "VERSION:3.0" << endl;

        QMap<QString,QString>::iterator vi;

        for(vi = vCardData.begin(); vi != vCardData.end(); ++vi) {
            if(vi.key() == "name") {
                stream << "FN:" << vi.value() << endl;
            }

            if(vi.key() == "address") {
                stream << "ADR:" << vi.value().replace("\r\n", " ").replace("\n", " ").replace("\r", " ") << endl;
            }

            if(vi.key() == "email") {
                stream << "EMAIL:" << vi.value() << endl;
            }

            if(vi.key() == "profession") {
                stream << "TITLE:" << vi.value() << endl;
            }

            if(vi.key() == "telnr") {
                stream << "TEL:" << vi.value() << endl;
            }

        }

        stream << "END:VCARD";
        stream.flush();
        vCardFile.close();
    }

}
