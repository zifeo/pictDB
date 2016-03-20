
CFLAGS += -std=c99 -I/usr/local/opt/openssl/include
LDLIBS += -lssl -lcrypto

all: pictDBM

pictDBM: db_list.c db_create.c db_utils.c error.c pictDBM.c

clean:
	rm -f pictDBM
