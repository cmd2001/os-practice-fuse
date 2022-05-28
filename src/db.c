#include "db.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int db_open_db(const char *path) {
  return sqlite3_open(path, &db);
}

int db_init_table() {
  char *errmsg;
  if (sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS user(id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT)", NULL, NULL, &errmsg) != 0) {
    fprintf(stderr, "SQL error: %s\n", errmsg);
    sqlite3_free(errmsg);
    return -1;
  }
  if (sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS message(id INTEGER PRIMARY KEY AUTOINCREMENT, from_id INT, to_id INT, data TEXT)", NULL, NULL, &errmsg) != 0) {
    fprintf(stderr, "SQL error: %s\n", errmsg);
    sqlite3_free(errmsg);
    return -1;
  }
  return 0;
}

int db_create_user(const char* username) {
  char *errmsg;
  char *sql = sqlite3_mprintf("INSERT INTO user(username) VALUES('%q')", username);
  if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != 0) {
    fprintf(stderr, "SQL error: %s\n", errmsg);
    sqlite3_free(errmsg);
    sqlite3_free(sql);
    return -1;
  }
  sqlite3_free(sql);
  return 0;
}


int db_get_user_list(struct DB_User **dst) {
  char **result, *errmsg;
  int nrow, ncolumn;
  if (sqlite3_get_table(db, "SELECT * FROM user", &result, &nrow, &ncolumn, &errmsg) != 0) {
    fprintf(stderr, "SQL error: %s\n", errmsg);
    sqlite3_free(errmsg);
    return -1;
  } else {
    int i;
    struct DB_User *users = malloc(sizeof(struct DB_User) * nrow);
    for (i = 0; i < nrow; i++) {
      users[i].id = atoi(result[(i + 1) * ncolumn]);
      users[i].username = malloc(sizeof(char) * (strlen(result[(i + 1) * ncolumn + 1]) + 1));
      strcpy(users[i].username, result[(i + 1) * ncolumn + 1]);
    }
    *dst = users;
    sqlite3_free_table(result);
    return nrow;
  }
}

int db_get_user_by_name(const char* username, struct DB_User* dst) {
  char **result, *errmsg;
  int nrow, ncolumn;
  char *sql = sqlite3_mprintf("SELECT * FROM user WHERE username = '%s'", username);
  if (sqlite3_get_table(db, sql, &result, &nrow, &ncolumn, &errmsg) != 0) {
    fprintf(stderr, "SQL error: %s\n", errmsg);
    sqlite3_free(sql);
    sqlite3_free(errmsg);
    return -1;
  } else {
    if (nrow == 0) {
      return -1;
    }
    dst->id = atoi(result[(1) * ncolumn]);
    dst->username = malloc(sizeof(char) * (strlen(result[(1) * ncolumn + 1]) + 1));
    strcpy(dst->username, result[(1) * ncolumn + 1]);
    sqlite3_free_table(result);
    return 0;
  }
}

int db_get_user_contacted_users(const int user_id, struct DB_User **dst) {
  char **result, *errmsg;
  int nrow, ncolumn;
  char *sql = sqlite3_mprintf("SELECT * FROM user WHERE id IN (SELECT to_id FROM message WHERE from_id = %d UNION SELECT from_id FROM message WHERE to_id = %d)", user_id, user_id);
  if (sqlite3_get_table(db, sql, &result, &nrow, &ncolumn, &errmsg) != 0) {
    fprintf(stderr, "SQL error: %s\n", errmsg);
    sqlite3_free(sql);
    sqlite3_free(errmsg);
    return -1;
  } else {
    int i;
    struct DB_User *users = malloc(sizeof(struct DB_User) * nrow);
    for (i = 0; i < nrow; i++) {
      users[i].id = atoi(result[(i + 1) * ncolumn]);
      users[i].username = malloc(sizeof(char) * (strlen(result[(i + 1) * ncolumn + 1]) + 1));
      strcpy(users[i].username, result[(i + 1) * ncolumn + 1]);
    }
    *dst = users;
    sqlite3_free_table(result);
    return nrow;
  }
}

int db_create_message(const int from_user, const int sub_user, const char* data) {
  char *errmsg;
  printf("%d %d %s", from_user, sub_user, data);
  char *sql = sqlite3_mprintf("INSERT INTO message(from_id, to_id, data) VALUES(%d, %d, '%q')", from_user, sub_user, data);
  if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != 0) {
    fprintf(stderr, "SQL error: %s\n", errmsg);
    sqlite3_free(errmsg);
    sqlite3_free(sql);
    return -1;
  }
  sqlite3_free(sql);
  return 0;
}

int db_get_user_to_user_message(const int main_user, const int sub_user, struct DB_Message **dst) {
  char **result, *errmsg;
  int nrow, ncolumn;
  char *sql = sqlite3_mprintf("SELECT * FROM message WHERE from_id = %d AND to_id = %d UNION SELECT * FROM message WHERE from_id = %d AND to_id = %d", main_user, sub_user, sub_user, main_user);
  if (sqlite3_get_table(db, sql, &result, &nrow, &ncolumn, &errmsg) != 0) {
    fprintf(stderr, "SQL error: %s\n", errmsg);
    sqlite3_free(sql);
    sqlite3_free(errmsg);
    return -1;
  } else {
    int i;
    struct DB_Message *messages = malloc(sizeof(struct DB_Message) * nrow);
    for (i = 0; i < nrow; i++) {
      messages[i].id = atoi(result[(i + 1) * ncolumn]);
      messages[i].from_id = atoi(result[(i + 1) * ncolumn + 1]);
      messages[i].to_id = atoi(result[(i + 1) * ncolumn + 2]);
      messages[i].data = malloc(sizeof(char) * (strlen(result[(i + 1) * ncolumn + 3]) + 1));
      strcpy(messages[i].data, result[(i + 1) * ncolumn + 3]);
    }
    *dst = messages;
    sqlite3_free_table(result);
    return nrow;
  }
}