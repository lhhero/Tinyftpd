CC=gcc
CFLAGS=-Wall -g
BIN=Tinyftpd
OBJS=main.o sysutil.o session.o ftpproto.o privparent.o str.o tunable.o parseconf.o
LIBS=-lcrypt

$(BIN):$(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
.PHONY:clean
clean:
	rm -f *.o $(BIN)