CFLAGS += -std=c99 -I/usr/local/opt/openssl/include -Wall -Wpedantic $$(pkg-config --cflags vips) $$(pkg-config --cflags json-c)
LDLIBS += -lssl -lcrypto -lm $$(pkg-config --libs vips) $$(pkg-config --libs json-c)

pictDB_server: LDLIBS += -lmongoose
pictDB_server: LDFLAGS += -Llibmongoose

all: mongoose pictDBM pictDB_server

mongoose:
	@cd libmongoose && make

pictDBM: db_gbcollect.o db_read.o db_insert.o dedup.o pictDBM_tools.o image_content.o db_delete.o db_list.o db_create.o db_utils.o error.o pictDBM.o

pictDB_server: db_read.o db_insert.o dedup.o pictDBM_tools.o image_content.o db_delete.o db_list.o db_create.o db_utils.o error.o pictDB_server.o

clean:
	rm -f pictDBM *.o pictDBM pictDB_server
