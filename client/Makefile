LINK=gcc
CC=gcc
CFLAGS=-c -ggdb -Wall -I. -I/opt/local/include -I../lib -I../common -I../proto
LINKFLAGS=-L. -g -ggdb -pthread -L/opt/local/lib -L../lib
LINKLIBS=-lgcrypt -lm
LIB_DIR=../lib/
LOCAL_LIBS=cmpsc311
TARGETS=client

CLIENT_OBJS= client.o \
				main.o \
				../common/ccommon.o \
				../proto/proto.o

# Suffix rules
.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -o $@ $<
all: $(TARGETS)

$(LOCAL_LIBS):
	make -C ../lib/

client: main.o client.o ../proto/proto.o ../common/ccommon.o $(LOCAL_LIBS)
	$(LINK) $(LINKFLAGS) client.o main.o ../proto/proto.o ../common/ccommon.o $(patsubst %,-l%,$(LOCAL_LIBS)) $(LINKLIBS) -o $@

clean:
	rm -f $(TARGETS) $(CLIENT_OBJS) $(LIB_DIR)$(patsubst %,lib%.a, $(LOCAL_LIBS))