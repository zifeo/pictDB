
CFLAGS += -std=c99 -I/usr/local/opt/openssl/include -Wall -Wpedantic $$(pkg-config vips --cflags)
#LDFLAGS += -Llibmongoose
#LDLIBS += -lssl -lcrypto $$(pkg-config vips --libs) -lm -lmongoose -ljson-c
LDLIBS += -lssl -lcrypto $$(pkg-config vips --libs) -lm -ljson-c

#all: mongoose pictDBM pictDB_server
all: pictDBM

#mongoose:
#	@cd libmongoose && make

pictDBM: db_gbcollect.c db_read.c db_insert.c dedup.c pictDBM_tools.c image_content.c db_delete.c db_list.c db_create.c db_utils.c error.c pictDBM.c

pictDB_server: db_read.c db_insert.c dedup.c pictDBM_tools.c image_content.c db_delete.c db_list.c db_create.c db_utils.c error.c pictDB_server.c

clean:
	rm -f pictDBM *.o
