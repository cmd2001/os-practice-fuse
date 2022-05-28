#include "chat.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

void *chat_init(struct fuse_conn_info *conn, struct fuse_config *cfg) {
}

int find_user(const char* str, char** main_user, char** sub_user) {
  if (strcmp(str, "/") == 0) {
    *main_user = NULL;
    *sub_user = NULL;
    return 0;
  }
  int cnt = 0, fir = -1, sec = -1, thr = -1;
  int i;
  for (i = 0; i < strlen(str); i++) {
    if (str[i] == '/') {
      cnt++;
      if (fir == -1) {
        fir = i;
      } else if(sec == -1) {
        sec = i;
      } else if(thr == -1) {
        thr = i;
      }
    }
  }
  if (sec != -1) {
    if (thr == -1) {
      thr = strlen(str);
    }
  } else {
    sec = strlen(str);
  }
  *main_user = malloc(sizeof(char) * (sec - fir + 1));
  strncpy(*main_user, str + fir + 1, sec - fir - 1);
  (*main_user)[sec - fir - 1] = '\0';
  if (thr != -1) {
    *sub_user = malloc(sizeof(char) * (thr - sec + 1));
    strncpy(*sub_user, str + sec + 1, thr - sec - 1);
    (*sub_user)[thr - sec - 1] = '\0';
  } else {
    *sub_user = NULL;
  }
  return cnt;
}

int ignore_dot_file(const char* str) {
  int i = 0;
  for (i = 0; i < strlen(str); i++) {
    if (str[i] == '.') {
      return 1;
    }
  }
  return 0;
}

int chat_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
  printf("chat_getattr %s\n", path);
  if (ignore_dot_file(path)) {
    return -ENOENT;
  }
  char *main_user, *sub_user;
  const int cnt = find_user(path, &main_user, &sub_user);
  if (cnt > 2) {
    return -ENOENT;
  }
  stbuf->st_uid = getuid();
  stbuf->st_gid = getgid();
  if (cnt == 0) {
    stbuf->st_mode = S_IFDIR | 0755;
    stbuf->st_nlink = 2;
  } else if (cnt == 1) {
    int flag = -1;
    char** user_names;
    int n_users = msg_get_user_list(&user_names);
    for (int i = 0; i < n_users; i++) {
      if (strcmp(user_names[i], main_user) == 0) {
        flag = 0;
      }
      free(user_names[i]);
    }
    free(user_names);
    if (flag == 0) {
      stbuf->st_mode = S_IFDIR | 0755;
      stbuf->st_nlink = 2;
    } else {
      return -ENOENT;
    }
  } else {
    stbuf->st_mode = S_IFREG | 0755;
    stbuf->st_nlink = 1;
    char *data = NULL;
    int data_len = msg_get_user_to_user_message(main_user, sub_user, &data);
    stbuf->st_size = data_len;
    free(data);
  }
  free(main_user);
  free(sub_user);
  return 0;
}

int chat_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags) {
  printf("chat_readdir %s\n", path);
  if (ignore_dot_file(path)) {
    return -ENOENT;
  }
  char *main_user, *sub_user;
  const int cnt = find_user(path, &main_user, &sub_user);
  if (cnt > 2) {
    return -ENOENT;
  }
  filler(buf, ".", NULL, 0, 0);
  filler(buf, "..", NULL, 0, 0);
  if (cnt == 0) {
    char** user_names;
    int n_users = msg_get_user_list(&user_names);
    for (int i = 0; i < n_users; i++) {
      filler(buf, user_names[i], NULL, 0, 0);
      free(user_names[i]);
    }
    free(user_names);
  } else {
    char** user_names;
    int n_users = msg_get_user_contacted_users(main_user, &user_names);
    for (int i = 0; i < n_users; i++) {
      filler(buf, user_names[i], NULL, 0, 0);
      free(user_names[i]);
    }
    free(user_names);
  }
  free(main_user);
  free(sub_user);
  return 0;
}

int chat_open(const char *path, struct fuse_file_info *fi) {
  printf("chat_open %s\n", path);
  return 0;
}

int chat_flush(const char *path, struct fuse_file_info *fi) {
  printf("chat_flush %s\n", path);
  return 0;
}

int chat_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
  printf("chat_read %s\n", path);
  if (ignore_dot_file(path)) {
    return -ENOENT;
  }
  char *main_user, *sub_user;
  const int cnt = find_user(path, &main_user, &sub_user);
  if (cnt != 2) {
    return -ENOENT;
  }
  char *data = NULL;
  int data_len = msg_get_user_to_user_message(main_user, sub_user, &data);
  strncpy(buf, data, data_len);
  free(data);
  free(main_user);
  free(sub_user);
  return data_len;
}

int chat_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
  printf("main_write %s\n", path);
  if (ignore_dot_file(path)) {
    return -ENOENT;
  }
  char *main_user, *sub_user;
  const int cnt = find_user(path, &main_user, &sub_user);
  if (cnt != 2) {
    return -ENOENT;
  }
  char* data = malloc(sizeof(char) * (size + 1));
  strncpy(data, buf, size);
  data[size] = '\0';
  msg_create_message(main_user, sub_user, data);
  free(main_user);
  free(sub_user);
  free(data);
  return size;
}

int chat_mkdir(const char *path, mode_t mode) {
  printf("chat_mkdir %s\n", path);
  if (ignore_dot_file(path)) {
    return -ENOENT;
  }
  char *main_user, *sub_user;
  const int cnt = find_user(path, &main_user, &sub_user);
  if (cnt != 1) {
    return -ENOENT;
  } else {
    struct DB_User user;
    if (db_get_user_by_name(main_user, &user) == 0) {
      free(user.username);
      return 0;
    }
    if (db_create_user(main_user) != 0) {
      return -ENOENT;
    }
  }
  free(main_user);
  free(sub_user);
  return 0;
}