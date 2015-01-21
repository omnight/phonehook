#ifndef LOOKUP_THREAD_H
#define LOOKUP_THREAD_H

#include <QThread>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSql>
#include <QtSql/QSqlQuery>
#include <QDebug>


class lookup_worker : public QObject {
    Q_OBJECT
public:
    explicit lookup_worker(QObject *parent = 0): QObject(parent) {

    }

public slots:
    void threadStarted(QString number, int botId);

signals:
    void finished();

};


class lookup_thread : public QObject
{
    Q_OBJECT
public:
    explicit lookup_thread(QObject *parent = 0);

    ~lookup_thread();

    void start(QString number, int botId);

private:
    lookup_worker w;
    QThread thread;

public slots:
    void worker_finish();

signals:
    void start_worker(QString number, int botId);

};

#endif // LOOKUP_THREAD_H
