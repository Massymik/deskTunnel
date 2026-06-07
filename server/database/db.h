#pragma once

#include "../lib/sqlite/sqlite3.h"
#include <iostream>
#include <string>

inline bool createDB() {
  sqlite3 *db = nullptr;
  char *errorMessage = nullptr;
  int result = sqlite3_open("./database/users.db", &db);

  if (result != SQLITE_OK) {
    sqlite3_close(db);
    return false;
  }

  const char *sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL
        );
    )";

  result = sqlite3_exec(db, sql, nullptr, nullptr, &errorMessage);

  if (result != SQLITE_OK) {
    sqlite3_free(errorMessage);
    sqlite3_close(db);
    return false;
  }

  sqlite3_close(db);
  return true;
}

inline bool createUser(const std::string &username, const std::string &passwordHash) {
  sqlite3 *db = nullptr;
  int result = sqlite3_open("./database/users.db", &db);

  if (result != SQLITE_OK) {
    sqlite3_close(db);
    return false;
  }

  const char *query = "INSERT INTO users (username, password_hash) VALUES (?, ?);";
  sqlite3_stmt *statement = nullptr;
  result = sqlite3_prepare_v2(db, query, -1, &statement, nullptr);

  if (result != SQLITE_OK) {
    sqlite3_close(db);
    return false;
  }

  sqlite3_bind_text(statement, 1, username.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, passwordHash.c_str(), -1, SQLITE_TRANSIENT);
  result = sqlite3_step(statement);

  if (result != SQLITE_DONE) {
    sqlite3_finalize(statement);
    sqlite3_close(db);
    return false;
  }

  sqlite3_finalize(statement);
  sqlite3_close(db);

  return true;
}

bool userExist(std::string username) {
  sqlite3 *db = nullptr;
  int result = sqlite3_open("./database/users.db", &db);

  if (result != SQLITE_OK) {
    sqlite3_close(db);
    return false;
  }

  const char *query = "SELECT username FROM users WHERE username = ? LIMIT 1;";
  sqlite3_stmt *statement = nullptr;
  result = sqlite3_prepare_v2(db, query, -1, &statement, nullptr);

  if (result != SQLITE_OK) {
    sqlite3_close(db);
    return false;
  }

  sqlite3_bind_text(statement, 1, username.c_str(), -1, SQLITE_TRANSIENT);
  result = sqlite3_step(statement);
  bool exists = (result == SQLITE_ROW);

  sqlite3_finalize(statement);
  sqlite3_close(db);

  return exists;
}