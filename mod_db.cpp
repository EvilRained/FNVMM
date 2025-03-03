#include "mod_db.h"
#include <iostream>
#include <filesystem>
#include <sqlite3.h>


void initDatabase(sqlite3* db) {
    const char* sql = "CREATE TABLE IF NOT EXISTS config (name TEXT, esm TEXT, init PATH);";
    char* errMsg = nullptr;
    sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (errMsg) {
        std::cerr << "SQL error: " << errMsg << "\n";
        sqlite3_free(errMsg);
    }
}

void saveMod(sqlite3* db, const Mod& mod) {
    const char* sql = "INSERT INTO mods (name, esm, path) VALUES (?, ?, ?;"; // Plain INSERT
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, mod.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, mod.path.c_str(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}
