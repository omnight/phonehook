#ifndef DB_H
#define DB_H

#include <QObject>
#include <QSqlDatabase>

class db : public QObject
{
    Q_OBJECT
public:
    static db *Instance(QObject* parent=0);
    void initDbTables();

    ~db();

    QSqlDatabase getContactsDb();

signals:

public slots:

private:   
    static db* m_Instance;
    explicit db(QObject *parent = 0);

    QSqlDatabase m_db;
    QSqlDatabase m_db_contacts;

};

#endif // DB_H
