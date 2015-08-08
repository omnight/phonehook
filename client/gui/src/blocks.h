#ifndef BLOCKS_H
#define BLOCKS_H

#include <QObject>
#include <QSqlDatabase>
#include "db_model.h"

class blocks : public QObject
{
    Q_OBJECT
public:
    explicit blocks(QObject *parent = 0);

    Q_PROPERTY(PhSqlModel* contacts READ contacts NOTIFY contacts_changed)
    Q_PROPERTY(PhSqlModel* db_blocks READ db_blocks NOTIFY blocks_changed)
    Q_PROPERTY(PhSqlModel* history READ history NOTIFY history_changed)
    Q_PROPERTY(PhSqlModel* sources READ sources NOTIFY sources_changed)

    Q_INVOKABLE void initContacts();
    Q_INVOKABLE void initBlocks();
    Q_INVOKABLE void initHistory(int blockId);
    Q_INVOKABLE void initSources();

    Q_INVOKABLE void addBlockedContact(int contactId);
    Q_INVOKABLE void addManualBlock(QString name, QString number, bool isHidden);

    Q_INVOKABLE void deleteBlock(int blockId);

    Q_INVOKABLE QString contactName(int contactId);
    Q_INVOKABLE QString contactNameFromNumber(QString number);

    PhSqlModel *contacts() {
        if(!m_contacts.query().isValid())
            initContacts();
        return &m_contacts;
    }
    PhSqlModel *db_blocks() {
        if(!m_blocks.query().isValid())
            initBlocks();
        return &m_blocks;
    }

    PhSqlModel *sources() {
        if(!m_sources.query().isValid())
            initSources();
        return &m_sources;
    }

    PhSqlModel *history() {
        return &m_history;
    }

    QSqlDatabase contacts_db;

    ~blocks();

private:
    PhSqlModel m_contacts;
    PhSqlModel m_blocks;
    PhSqlModel m_history;
    PhSqlModel m_sources;

    QMap<int,QString> contactNameCache;
    QMap<QString,int> contactNumberCache;

signals:
    void contacts_changed(PhSqlModel*);
    void blocks_changed(PhSqlModel*);
    void history_changed(PhSqlModel*);
    void sources_changed(PhSqlModel*);

public slots:

};

#endif // BLOCKS_H
