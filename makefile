all: db msg chat main
	gcc db.o msg.o chat.o main.o `pkg-config sqlite3 fuse3 --cflags --libs` -o main
run: all
	$(shell if [ ! -e mnt ];then mkdir -p mnt; fi)
	./main -d mnt
db:
	gcc src/db.c -c -o db.o
msg:
	gcc src/msg.c -c -o msg.o
chat:
	gcc src/chat.c -c -o chat.o
main:
	gcc src/main.c -c -o main.o
clean:
	rm db.o msg.o main.o chat.o main
clear:
	rm data.db