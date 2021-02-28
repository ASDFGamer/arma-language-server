#include "migrator.hpp"

sqlite::result sqfvm::lsp::migrator::migrate_v1(sqlite::database& db)
{
    auto result = db.execute(
        "CREATE TABLE IF NOT EXISTS tFiles ("
        "    filepath TEXT NOT NULL PRIMARY KEY,"
        "    timestamp INT64 NOT NULL,"
        "    state INT NOT NULL"
        "); ");
    if (result != sqlite::result::OK) { return result; }
    return sqlite::result::OK;
}
