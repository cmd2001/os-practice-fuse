#ifndef CHAT_H
#define CHAT_H

#define FUSE_USE_VERSION 31
#include <fuse3/fuse.h>

#include "msg.h"

int find_user(const char* str, char** main_user, char** sub_user);

int ignore_dot_file(const char* str);

void *chat_init(struct fuse_conn_info *conn, struct fuse_config *cfg);

int chat_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi);

int chat_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags);

int chat_open(const char *path, struct fuse_file_info *fi);

int chat_flush(const char *path, struct fuse_file_info *fi);

int chat_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);

int chat_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);

int chat_mkdir(const char *path, mode_t mode);

#endif