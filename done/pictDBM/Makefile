
CFLAGS += -std=c99 -I/usr/local/opt/openssl/include -Wall -Wpedantic $$(pkg-config vips --cflags)
LDLIBS += -lssl -lcrypto $$(pkg-config vips --libs) -lm

all: pictDBM

pictDBM: db_insert.c dedup.c pictDBM_tools.c image_content.c db_delete.c db_list.c db_create.c db_utils.c error.c pictDBM.c

clean:
	rm -f pictDBM *.o
