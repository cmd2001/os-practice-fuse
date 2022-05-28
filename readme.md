# OS-Practice-FUSE

基于FUSE的系统实践

基于libfuse3和sqlite3实现的本地聊天室

**可能存在潜在的内存泄漏**

(懒得管了 反正就是个玩具)

许可证选择了`The Unlicensed`，可以参考，但是最好别抄作业

## 功能

文件夹/文件名区分用户

可以在多个文件夹中装载同一个聊天室(只需要指定同一个sqlite3 database)，实现内容共享

## 运行环境

* Ubuntu 20.04 Focal
* x86_64 Linux 5.15.35-1-pve
* libfuse3-dev
* libsqlite3-dev

## 测试

### 环境配置

```sh
sudo apt install build-essential pkg-config
sudo apt install fuse3 sqlite3 libfuse3-dev libsqlite3-dev
make run
```

### 测试结果

打开另一个终端

```sh
Amagi@Playground ~/os-practice-fuse $ ls
chat.o  data.db  db.o  main  main.o  makefile  mnt  msg.o  src
Amagi@Playground ~/os-practice-fuse $ ls -l
total 48
-rw-rw-r-- 1 Amagi Amagi  7424 May 28 12:14 chat.o
-rw-r--r-- 1 Amagi Amagi 16384 May 28 12:14 data.db
-rw-rw-r-- 1 Amagi Amagi  8632 May 28 12:14 db.o
-rwxrwxr-x 1 Amagi Amagi 22840 May 28 12:14 main
-rw-rw-r-- 1 Amagi Amagi  3192 May 28 12:14 main.o
-rw-rw-r-- 1 Amagi Amagi   365 May 28 12:14 makefile
drwxr-xr-x 2 Amagi Amagi     0 Jan  1  1970 mnt
-rw-rw-r-- 1 Amagi Amagi  4728 May 28 12:14 msg.o
drwxrwxr-x 2 Amagi Amagi     9 May 28 11:59 src
Amagi@Playground ~/os-practice-fuse $ cd mnt
Amagi@Playground ~/os-practice-fuse/mnt $ mkdir 123
Amagi@Playground ~/os-practice-fuse/mnt $ mkdir 456
Amagi@Playground ~/os-practice-fuse/mnt $ ls -l
total 0
drwxr-xr-x 2 Amagi Amagi 0 Jan  1  1970 123
drwxr-xr-x 2 Amagi Amagi 0 Jan  1  1970 456
Amagi@Playground ~/os-practice-fuse/mnt $ ls -l 123
total 0
Amagi@Playground ~/os-practice-fuse/mnt $ ls -l 456
total 0
Amagi@Playground ~/os-practice-fuse/mnt $ echo 'Test' > 123/456
Amagi@Playground ~/os-practice-fuse/mnt $ echo 'tseT' > 456/123
Amagi@Playground ~/os-practice-fuse/mnt $ ls -l 123 
total 0
-rwxr-xr-x 1 Amagi Amagi 18 Jan  1  1970 456
Amagi@Playground ~/os-practice-fuse/mnt $ ls -l 456
total 0
-rwxr-xr-x 1 Amagi Amagi 18 Jan  1  1970 123
Amagi@Playground ~/os-practice-fuse/mnt $ cat 123/456
<<< Test
>>> tseT
Amagi@Playground ~/os-practice-fuse/mnt $ cat 456/123
>>> Test
<<< tseT
```
