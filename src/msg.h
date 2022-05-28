#ifndef MSG_H
#define MSG_H

#include "db.h"

int msg_get_user_list(char*** dst);

int msg_get_user_contacted_users(const char* username, char*** dst);

int msg_create_message(const char* from_username, const char* to_username, const char* data);

int msg_get_user_to_user_message(const char* main_username, const char* sub_username, char** dst);


#endif