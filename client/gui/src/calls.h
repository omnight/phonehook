#ifndef CALLS_H
#define CALLS_H

#include <QObject>
#include "db_model.h"

class calls : public QObject
{
    Q_OBJECT

    Q_PROPERTY(PhSqlModel* call_log READ call_log NOTIFY call_log_changed)
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)
private:


    PhSqlModel m_call_log;
    QSqlDatabase comms_db;

    QString m_sqlFilter;
    QString m_filter;

public:
    explicit calls(QObject *parent = 0);


    PhSqlModel *call_log() {
        if(!m_call_log.query().isValid())
            initCallLog();
        return &m_call_log;
    }

    Q_INVOKABLE void initCallLog();    

    void setFilter(QString filter);
    QString filter();

    ~calls();

signals:
    void call_log_changed(PhSqlModel*);
    void filterChanged(QString filter);



public slots:


};

#endif // CALLS_H
