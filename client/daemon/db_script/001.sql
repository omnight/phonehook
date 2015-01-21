CREATE TABLE DB_VERSION (version integer);

INSERT INTO DB_VERSION VALUES(0);

CREATE TABLE BOT (
    id INTEGER PRIMARY KEY,
    name TEXT,
    revision INTEGER,
    enabled NUMERIC,
    xml TEXT
);

CREATE TABLE BOT_PARAM (
    bot_id INTEGER,
    title TEXT,
    key TEXT,
    type TEXT,
    value TEXT
);

CREATE TABLE SETTING (
    key TEXT PRIMARY KEY,
    value TEXT
);
