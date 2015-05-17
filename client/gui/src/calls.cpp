#include "calls.h"


calls::calls(QObject *parent) :
    QObject(parent)
{
    comms_db = QSqlDatabase::addDatabase("QSQLITE", "calls");
    comms_db.setDatabaseName("/home/nemo/.local/share/commhistory/commhistory.db");

    comms_db.open();
}

calls::~calls() {
    comms_db.close();
}


void calls::initCallLog() {

    if(!m_call_log.query().isValid()) {
        m_call_log.setQuery("SELECT MAX(starttime) recent_time, COUNT(*) call_count, remoteuid FROM Events WHERE isMissedCall=1 AND remoteUid <> '' GROUP BY remoteuid ORDER BY MAX(starttime) DESC",
                           comms_db);
    } else {
        m_call_log.refresh();
    }

    qDebug() << m_call_log.count();
    emit call_log_changed(&m_call_log);

}
