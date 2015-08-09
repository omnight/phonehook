-- Add foreign key to BOT_RESULT_CACHE
BEGIN TRANSACTION;

    CREATE TEMPORARY TABLE BOT_RESULT_CACHE_BACKUP (
        bot_id INTEGER,
        telnr TEXT,
        expire_date TEXT,
        result BLOB
    );

    INSERT INTO BOT_RESULT_CACHE_BACKUP SELECT * FROM BOT_RESULT_CACHE;

    DROP TABLE BOT_RESULT_CACHE;

    CREATE TABLE BOT_RESULT_CACHE (
        id INTEGER PRIMARY KEY,
        bot_id INTEGER REFERENCES BOT(id) ON DELETE CASCADE,
        telnr TEXT,
        cache_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        result BLOB,
        block INTEGER
    );

    INSERT INTO bot_result_cache(bot_id,telnr,cache_date,result) SELECT * FROM bot_result_cache_backup;
    DROP TABLE bot_result_cache_backup;

COMMIT;

-- Re-create BOT_RESPONSE_CACHE
BEGIN TRANSACTION;

    DROP TABLE BOT_RESPONSE_CACHE;

    CREATE TABLE BOT_RESPONSE_CACHE (
        id INTEGER PRIMARY KEY,
        bot_id INTEGER REFERENCES BOT(id) ON DELETE CASCADE,
        httpRequestKey TEXT,
        cache_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        result BLOB
    );

COMMIT;

ALTER TABLE BLOCK_HISTORY ADD COLUMN number TEXT;


CREATE TABLE BOT_LOGIN(
    id INTEGER PRIMARY KEY,
    bot_id INTEGER REFERENCES BOT(id) ON DELETE CASCADE,
    name TEXT,
    url TEXT,
    login_success_url TEXT,
    login_success_tag TEXT
);
