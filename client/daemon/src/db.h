#ifndef DB_H
#define DB_H

#include <QObject>
#include <QSqlDatabase>

class db : public QObject
{
    Q_OBJECT
public:
    explicit db(QObject *parent = 0);

    void initDbTables();

signals:

public slots:

private:
    QSqlDatabase m_db;

};

#endif // DB_H
