CFLAGS += -std=c99 -I/usr/local/opt/openssl/include -Wall -Wpedantic $$(pkg-config vips --cflags)
LDLIBS += -lssl -lcrypto $$(pkg-config vips --libs) -lm -ljson-c

pictDB_server: LDLIBS += -lmongoose
pictDB_server: LDFLAGS += -Llibmongoose

all: mongoose pictDBM pictDB_server

mongoose:
	@cd libmongoose && make

pictDBM: db_read.c db_insert.c dedup.c pictDBM_tools.c image_content.c db_delete.c db_list.c db_create.c db_utils.c error.c pictDBM.c

pictDB_server: db_read.c db_insert.c dedup.c pictDBM_tools.c image_content.c db_delete.c db_list.c db_create.c db_utils.c error.c pictDB_server.c

clean:
	rm -f pictDBM *.o