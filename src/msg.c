#include "msg.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


int msg_get_user_list(char*** dst) {
  struct DB_User *db_users;
  int cnt = db_get_user_list(&db_users);
  if (cnt < 0) {
    return -1;
  }
  char **result = malloc(sizeof(char*) * (cnt));
  int i;
  for (i = 0; i < cnt; i++) {
    result[i] = malloc(sizeof(char) * (strlen(db_users[i].username) + 1));
    strcpy(result[i], db_users[i].username);
    free(db_users[i].username);
  }
  free(db_users);
  *dst = result;
  return cnt;
}

int msg_get_user_contacted_users(const char* username, char*** dst) {
  struct DB_User current_user;
  if (db_get_user_by_name(username, &current_user) < 0) {
    return -1;
  }
  struct DB_User *db_users;
  int cnt = db_get_user_contacted_users(current_user.id, &db_users);
  if (cnt < 0) {
    return -1;
  }
  char **result = malloc(sizeof(char*) * (cnt));
  int i;
  for (i = 0; i < cnt; i++) {
    result[i] = malloc(sizeof(char) * (strlen(db_users[i].username) + 1));
    strcpy(result[i], db_users[i].username);
    free(db_users[i].username);
  }
  free(current_user.username);
  free(db_users);
  *dst = result;
  return cnt;
}

int msg_create_message(const char* from_username, const char* to_username, const char* data) {
  struct DB_User from_user, to_user;
  if (db_get_user_by_name(from_username, &from_user) < 0) {
    return -1;
  }
  if (db_get_user_by_name(to_username, &to_user) < 0) {
    return -1;
  }
  if (db_create_message(from_user.id, to_user.id, data) < 0) {
    return -1;
  }
  free(from_user.username);
  free(to_user.username);
  return 0;
}

int msg_get_user_to_user_message(const char* main_username, const char* sub_username, char** dst) {
  struct DB_User main_user, sub_user;
  if (db_get_user_by_name(main_username, &main_user) < 0) {
    return -1;
  }
  if (db_get_user_by_name(sub_username, &sub_user) < 0) {
    return -1;
  }
  struct DB_Message *db_messages;
  int cnt = db_get_user_to_user_message(main_user.id, sub_user.id, &db_messages);
  if (cnt < 0) {
    return -1;
  }
  int i, total_len = 0;
  for (i = 0; i < cnt; i++) {
    total_len += strlen(db_messages[i].data) + 4; // '<<< '
  }
  char *result = malloc(sizeof(char) * (total_len + 1));
  int offset = 0;
  for (i = 0; i < cnt; i++) {
    strcpy(result + offset, main_user.id == db_messages[i].from_id ? "<<< " : ">>> ");
    offset += 4;
    strcpy(result + offset, db_messages[i].data);
    offset += strlen(db_messages[i].data);
    free(db_messages[i].data);
  }
  result[total_len] = '\0';
  free(db_messages);
  free(main_user.username);
  free(sub_user.username);
  *dst = result;
  return total_len;
}
