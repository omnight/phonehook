CREATE TABLE BOT_RESULT_CACHE ( bot_id INTEGER, telnr TEXT, expire_date TEXT, result BLOB);
CREATE TABLE BOT_RESPONSE_CACHE ( bot_id INTEGER, httpRequestKey TEXT, expire_date TEXT, result BLOB);