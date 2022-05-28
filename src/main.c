#define FUSE_USE_VERSION 31
#include <fuse3/fuse.h>

#include "db.h"
#include "chat.h"

static const char* db_name = "data.db";

static const struct fuse_operations main_oper = {
  .init     = chat_init,
  .getattr  = chat_getattr,
  .readdir  = chat_readdir,
  .open     = chat_open,
  .write    = chat_write,
  .read     = chat_read,
  .flush    = chat_flush,
  .mkdir    = chat_mkdir,
};


int main(int argc, char *argv[]) {
  db_open_db(db_name);
  db_init_table();
  if (!fuse_main(argc, argv, &main_oper, NULL)){
    return -1;
  }
  return 0;
}