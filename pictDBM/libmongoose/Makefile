// Provided file © EPFL

CC       = clang
CPPFLAGS =

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CFLAGS += -DLINUX
endif
ifeq ($(UNAME_S),Darwin)
	CFLAGS += -DOSX -I/usr/local/opt/openssl/include/
endif

CFLAGS   += -std=c99
CFLAGS   += -fPIC
RM       = /bin/rm -f

TARGETS = libmongoose.so
OBJS = *.o
RUBS = $(OBJS) *~ core

all:: $(TARGETS)
	@echo All done.

libmongoose.so: mongoose.o
	$(CC) -shared -o $@ $^

clean:
	-@$(RM) $(RUBS)
	@echo Cleaned.

new: mrproper all

mrproper: clean
	-@$(RM) $(TARGETS)
