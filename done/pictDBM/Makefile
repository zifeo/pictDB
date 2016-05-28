
CFLAGS += -std=c99 -I/usr/local/opt/openssl/include -Wall -Wpedantic $$(pkg-config vips --cflags)
LDLIBS += -lssl -lcrypto $$(pkg-config vips --libs) -lm -ljson-c

pictDB_server: LDLIBS += -lmongoose
pictDB_server: LDFLAGS += -Llibmongoose

all: mongoose pictDBM pictDB_server

mongoose:
	@cd libmongoose && make

pictDBM: db_read.o db_insert.o dedup.o pictDBM_tools.o image_content.o db_delete.o db_list.o db_create.o db_utils.o error.o pictDBM.o

pictDB_server: db_read.o db_insert.o dedup.o pictDBM_tools.o image_content.o db_delete.o db_list.o db_create.o db_utils.o error.o pictDB_server.o

clean:
	rm -f pictDBM *.o
