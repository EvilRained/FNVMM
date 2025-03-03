#ifndef MOD_DB_H
#define MOD_DB_H

#include <string>
#include <sqlite3.h>

struct Mod {
    int id = 0;
    std::string name;
    std::string esm;
    std::string path;

};

void initDatabase(sqlite3* db);
void saveMod(sqlite3* db, const Mod& mod);

#endif // MOD_DB_H
