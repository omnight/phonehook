#include <QSqlRecord>

#include "util.h"
#include "macros.h"

util::util(QObject *parent) : QObject(parent)
{

}

QVariant util::queryToArray(QString query, QVariantMap params, QSqlDatabase *db) {


    QSqlQuery *q;

    if(db) {
        q = new QSqlQuery(*db);
    } else
        q = new QSqlQuery();

    q->prepare(query);


    for (QVariantMap::iterator i = params.begin(); i != params.end(); ++i) {
        q->bindValue(":"+i.key(), i.value());
    }

    SQL_EXEC_CHECK((*q))

    if(q->lastError().isValid()) {
        delete q;
        return QVariant();
    }

    QVariantList results;

    while(q->next()) {
        QVariantMap result;
        for(int i=0; i < q->record().count(); i++) {
            result[q->record().fieldName(i)] = q->record().value(i);
        }
        results.append(result);
    }

    delete q;
    return results;
}
