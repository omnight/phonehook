#include "calls.h"


calls::calls(QObject *parent) :
    QObject(parent)
{
    comms_db = QSqlDatabase::addDatabase("QSQLITE", "calls");
    comms_db.setDatabaseName("/home/nemo/.local/share/commhistory/commhistory.db");

    comms_db.open();

    setFilter("missed");

}

calls::~calls() {
    comms_db.close();
}


void calls::initCallLog() {

    if(!m_call_log.query().isValid()) {
        m_call_log.setQuery(R"(
            SELECT MAX(starttime) recent_time, COUNT(*) call_count, remoteuid FROM Events
                WHERE )" + m_sqlFilter + R"(
                AND remoteUid <> ''
            GROUP BY remoteuid ORDER BY MAX(starttime) DESC
        )", comms_db);
    } else {
        m_call_log.refresh();
    }

    qDebug() << m_call_log.count();
    emit call_log_changed(&m_call_log);

}


QString calls::filter() {
    return m_filter;
}

void calls::setFilter(QString filter) {

    if(m_filter == filter) {
        return ;
    }

    if(filter == "incoming")
        m_sqlFilter = "type=3 and direction=1";

    if(filter == "outgoing")
        m_sqlFilter = "type=3 and direction=2";

    if(filter == "missed")
        m_sqlFilter = "isMissedCall=1";


    m_filter = filter;

    emit filterChanged(m_filter);

    initCallLog();
}
