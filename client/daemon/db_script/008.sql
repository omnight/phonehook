CREATE TABLE BLOCK (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    type INTEGER,
    bot_id INTEGER REFERENCES bot(id) ON DELETE CASCADE,
    contact_id INTEGER,
    name TEXT,
    number TEXT);

CREATE UNIQUE INDEX IF NOT EXISTS idx_block_contact ON block (contact_id);
CREATE UNIQUE INDEX IF NOT EXISTS idx_block_bot ON block (bot_id);

-- TYPE 0 -> number (manual)
-- TYPE 1 -> contact (contacts db)
-- TYPE 2 -> bot (wait for bot to finish execution)

CREATE TABLE BLOCK_HISTORY (
    block_id INTEGER REFERENCES block(id) ON DELETE CASCADE,
    date TIMESTAMP DEFAULT (datetime(CURRENT_TIMESTAMP, 'localtime'))
);

-- Add foreign key to BOT_PARAMS
BEGIN TRANSACTION;

    CREATE TEMPORARY TABLE BOT_PARAM_BACKUP (
        bot_id INTEGER,
        title TEXT,
        key TEXT,
        type TEXT,
        value TEXT
    );

    INSERT INTO BOT_PARAM_BACKUP SELECT * FROM BOT_PARAM;

    DROP TABLE BOT_PARAM;

    CREATE TABLE BOT_PARAM (
        bot_id INTEGER REFERENCES BOT(id) ON DELETE CASCADE,
        title TEXT,
        key TEXT,
        type TEXT,
        value TEXT
    );

    INSERT INTO bot_param SELECT * FROM bot_param_backup;
    DROP TABLE bot_param_backup;

COMMIT


-- Add foreign key to BOT_COOKIE_CACHE

BEGIN TRANSACTION;

    CREATE TEMPORARY TABLE BOT_COOKIE_CACHE_BACKUP (
        bot_id INTEGER,
        key TEXT,
        domain TEXT,
        path TEXT,
        expire TEXT,
        value TEXT
    );

    INSERT INTO BOT_COOKIE_CACHE_BACKUP SELECT * FROM BOT_COOKIE_CACHE;

    DROP TABLE BOT_COOKIE_CACHE;

    CREATE TABLE BOT_COOKIE_CACHE (
        bot_id INTEGER REFERENCES BOT(id) ON DELETE CASCADE,
        key TEXT,
        domain TEXT,
        path TEXT,
        expire TEXT,
        value TEXT
    );

    INSERT INTO bot_cookie_cache SELECT * FROM bot_cookie_cache_backup;
    DROP TABLE bot_cookie_cache_backup;

COMMIT

-- Add foreign key to BOT_TAG
BEGIN TRANSACTION;

    CREATE TEMPORARY TABLE BOT_TAG_BACKUP (
        bot_id INTEGER,
        tag TEXT
    );

    INSERT INTO BOT_TAG_BACKUP SELECT * FROM BOT_TAG;

    DROP TABLE BOT_TAG;

    CREATE TABLE BOT_TAG (
        bot_id INTEGER REFERENCES BOT(id) ON DELETE CASCADE,
        tag TEXT
    );

    INSERT INTO bot_tag SELECT * FROM bot_tag_backup;
    DROP TABLE bot_tag_backup;

COMMIT
