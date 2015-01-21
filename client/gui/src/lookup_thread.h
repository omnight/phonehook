#ifndef LOOKUP_THREAD_H
#define LOOKUP_THREAD_H

#include <QThread>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSql>
#include <QtSql/QSqlQuery>
#include <QDebug>

class lookup_thread : public QObject
{
    Q_OBJECT
public:
    explicit lookup_thread(QString number, int botId = 0, QObject *parent = 0);

    void start();

    QString number;
    int botId;

private:
    QThread thread;

signals:

public slots:
    void threadStarted();

};

#endif // LOOKUP_THREAD_H
