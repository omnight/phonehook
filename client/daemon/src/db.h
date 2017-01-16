#ifndef DB_H
#define DB_H

#include <QObject>
#include <QSqlDatabase>
#include "macros.h"

class db : public QObject
{
    Q_OBJECT

    SINGLETON(db)

public:
    void initDbTables();

    ~db();

    QSqlDatabase getContactsDb();

signals:

public slots:

private:   
    explicit db(QObject *parent = 0);

    QSqlDatabase m_db;
    QSqlDatabase m_db_contacts;

};

#endif // DB_H
