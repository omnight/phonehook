#ifndef CALLS_H
#define CALLS_H

#include <QObject>
#include "db_model.h"

class calls : public QObject
{
    Q_OBJECT

    Q_PROPERTY(PhSqlModel* call_log READ call_log NOTIFY call_log_changed)

private:


    PhSqlModel m_call_log;
    QSqlDatabase comms_db;

public:
    explicit calls(QObject *parent = 0);


    PhSqlModel *call_log() {
        if(!m_call_log.query().isValid())
            initCallLog();
        return &m_call_log;
    }

    Q_INVOKABLE void initCallLog();

    ~calls();

signals:
    void call_log_changed(PhSqlModel*);

public slots:

};

#endif // CALLS_H
