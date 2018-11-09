LINK=gcc
CC=gcc
CFLAGS=-c -ggdb -Wall -I. -I/opt/local/include -I../lib -I../common -I../proto
LINKFLAGS=-L. -g -ggdb -pthread -L/opt/local/lib -L../lib
LIBFLAGS=-shared -Wall -pthread
LINKLIBS=-lcmpsc311 -lgcrypt -lm -lrt
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

client: main.o client.o ../proto/proto.o ../common/ccommon.o $(LIBS)
	$(LINK) $(LINKFLAGS) client.o main.o ../proto/proto.o ../common/ccommon.o $(LINKLIBS) -o $@

clean:
	rm -f $(TARGETS) $(CLIENT_OBJS)