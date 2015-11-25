#ifndef LOOKUP_THREAD_H
#define LOOKUP_THREAD_H

#include <QThread>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSql>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include "phonenumber.h"
#include <QDomDocument>

class lookup_worker : public QObject {
    Q_OBJECT

private:

    QMap<QString,QString> vCardData;
    void updateVCard(QDomDocument data);

public:
    explicit lookup_worker(QObject *parent = 0): QObject(parent) {

    }

public slots:
    void threadStarted(QMap<QString,QString> parameters, QList<int> botIds);

signals:
    void finished();
    void gotResult();

};


class lookup_thread : public QObject
{
    Q_OBJECT
public:
    explicit lookup_thread(QObject *parent = 0);

    ~lookup_thread();

    void start(QMap<QString,QString> parameters, QList<int> botIds);

private:
    lookup_worker w;
    QThread thread;

public slots:
    void worker_finish();
    void worker_result();

signals:
    void start_worker(QMap<QString,QString> parameters, QList<int> botIds);
    void gotResult(lookup_thread *sender);
};

#endif // LOOKUP_THREAD_H
