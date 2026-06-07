#pragma once
#include "../lib/sqlite/sqlite3.h"
#include "./db.h"
#include <iostream>

bool authenticate(std::string username, std::string passwordHash) {
  sqlite3 *db = nullptr;
  const char *dbPath = "database/users.db";
  int result = sqlite3_open(dbPath, &db);

  if (result != SQLITE_OK) {
    sqlite3_close(db);
    return false;
  }

  const char *sql = "SELECT password_hash FROM users WHERE username = ? LIMIT 1;";
  sqlite3_stmt *statement = nullptr;
  result = sqlite3_prepare_v2(db, sql, -1, &statement, nullptr);

  if (result != SQLITE_OK) {
    sqlite3_close(db);
    return false;
  }

  sqlite3_bind_text(statement, 1, username.c_str(), -1, SQLITE_TRANSIENT);
  result = sqlite3_step(statement);

  if (result != SQLITE_ROW) {
    sqlite3_finalize(statement);
    sqlite3_close(db);
    return false;
  }

  const unsigned char *savedPasswordHashText = sqlite3_column_text(statement, 0);

  if (savedPasswordHashText == nullptr) {
    sqlite3_finalize(statement);
    sqlite3_close(db);
    return false;
  }

  std::string savedPasswordHash = reinterpret_cast<const char *>(savedPasswordHashText);

  sqlite3_finalize(statement);
  sqlite3_close(db);

  return passwordHash == savedPasswordHash;
}