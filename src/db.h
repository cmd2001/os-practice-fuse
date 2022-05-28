#ifndef DB_H
#define DB_H

#include <sqlite3.h>

static sqlite3 *db;

int db_open_db(const char *path);

int db_init_table();

int db_create_user(const char* username);

struct DB_User {
  int id;
  char *username;
};
int db_get_user_list(struct DB_User **dst);

int db_get_user_by_name(const char* username, struct DB_User* dst);

int db_get_user_contacted_users(const int user_id, struct DB_User **dst);

struct DB_Message {
  int id;
  int from_id;
  int to_id;
  char *data;
};

int db_create_message(const int from_user, const int sub_user, const char* data);

int db_get_user_to_user_message(const int main_user, const int sub_user, struct DB_Message **dst);


#endif