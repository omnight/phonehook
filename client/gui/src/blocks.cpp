#include "blocks.h"
#include <QSqlError>
blocks::blocks(QObject *parent) :
    QObject(parent)
{


    contacts_db = QSqlDatabase::addDatabase("QSQLITE", "contacts");
    contacts_db.setDatabaseName("/home/nemo/.local/share/system/Contacts/qtcontacts-sqlite/contacts.db");

    contacts_db.open();

    //initContacts();

}

blocks::~blocks() {
    contacts_db.close();
}

void blocks::initContacts() {
    qDebug() << "init contacts!";

    m_contacts.setQuery("SELECT contactId, displayLabel FROM Contacts WHERE hasPhoneNumber=1 ORDER BY displayLabel",
                       contacts_db);

    qDebug() << m_contacts.count();

    emit contacts_changed(&m_contacts);
}


QString blocks::contactName(int contactId) {

    if(contactId == 0) return "";

    if(contactNameCache.contains(contactId))
        return contactNameCache[contactId];

    QSqlQuery sq(contacts_db);

    sq.prepare("SELECT displayLabel FROM Contacts WHERE contactId=?");
    sq.addBindValue(contactId);

    sq.exec();
    if(sq.next()) {
        QString contactName = sq.value("displayLabel").toString();
        contactNameCache[contactId] = contactName;
        return contactName;
    }
    return "Unknown";
}


QString blocks::contactNameFromNumber(QString number) {

    if(contactNumberCache.contains(number))
        return contactName(contactNumberCache[number]);

    QSqlQuery sq(contacts_db);

    sq.prepare("SELECT contactId FROM PhoneNumbers WHERE normalizedNumber=?");
    sq.addBindValue(number.right(8));

    if(!sq.exec()) qDebug() << sq.lastError();
    if(sq.next()) {
        int contact_id = sq.value("contactId").toInt();
        contactNumberCache[number] = contact_id;
        return contactName(contact_id);
    } else {
        contactNumberCache[number] = 0;
        return "";
    }

}


void blocks::addBlockedContact(int contactId) {

    QSqlQuery sq;
    sq.prepare("INSERT INTO block (type, contact_id) VALUES(1,?)");
    sq.addBindValue(contactId);
    qDebug() << "added block! " << sq.exec();

    initBlocks();

}

void blocks::addManualBlock(QString name, QString number, bool isHidden) {
    QSqlQuery sq;
    sq.prepare("INSERT INTO block (type, name, number) VALUES(0,?,?)");

    if(isHidden) {
        sq.addBindValue("Hidden Number");
        sq.addBindValue("");
    } else {
        sq.addBindValue(name);
        sq.addBindValue(number);
    }
    qDebug() << "added block! " << sq.exec();

    initBlocks();
}


void blocks::initBlocks() {
    if(!m_blocks.query().isValid()) {
        qDebug() << "init blocks!";
        m_blocks.setQuery(R"(
          SELECT block.*, COALESCE(x.cnt,0) block_count, x.date, bot.name bot_name FROM block LEFT JOIN
          (SELECT block_id, MAX(date) date, COUNT(*) cnt FROM block_history GROUP BY block_id) x ON x.block_id = block.id
          LEFT JOIN bot ON block.bot_id = bot.id AND block.type = 2
          ORDER BY x.date DESC
        )");
    } else {
        m_blocks.refresh();
    }

    emit blocks_changed(&m_blocks);
}

void blocks::deleteBlock(int blockId) {
    QSqlQuery sq;
    sq.prepare("DELETE FROM block WHERE id=?");
    sq.addBindValue(blockId);
    qDebug() << "delete block! " << sq.exec();

    initBlocks();
}


void blocks::initHistory(int blockId) {
    m_history.setQuery("SELECT date,number FROM block_history WHERE block_id=" + QString::number(blockId)
                       + " ORDER BY date DESC");
    emit history_changed(&m_history);
}

void blocks::initSources() {
    m_sources.setQuery(R"(
        SELECT bot.name, bot.id FROM bot
        JOIN bot_tag ON bot.id = bot_tag.bot_id AND bot_tag.tag = 'block'
        LEFT JOIN block ON block.bot_id = bot.id AND block.type=2
        WHERE block.bot_id IS NULL
    )");

    emit sources_changed(&m_sources);
}
