#include "setting.h"
#include <QSqlQuery>
#include <QVariant>

setting::setting(QObject *parent) :
    QObject(parent)
{
}


QString setting::get(QString key, QString defValue) {

    QSqlQuery ptq;
    ptq.prepare("SELECT value FROM setting WHERE key = ?");
    ptq.addBindValue(key);
    ptq.exec();


    if(ptq.next()) {
        return ptq.value("value").toString();
    }

    return defValue;
}
