LINK=gcc
CC=gcc
CFLAGS=-c -fPIC -ggdb -Wall -I. -I/opt/local/include -I../lib -I../common -I../proto
LINKFLAGS=-L. -fPIC -no-pie -g -ggdb -pthread -L/opt/local/lib -L../lib
LIBFLAGS=-shared -Wall -pthread
LINKLIBS=-lgcrypt -lm
LIB_DIR=../lib/
LOCAL_LIBS=cmpsc311
TARGETS=flooder

FLOODER_OBJS= flooder.o \
				main.o \
				../common/ccommon.o \
				../proto/proto.o \

# Suffix rules
.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -o $@ $<
all: $(TARGETS)

$(LOCAL_LIBS):
	make -C ../lib/

flooder: main.o flooder.o ../proto/proto.o ../common/ccommon.o $(LOCAL_LIBS)
	$(LINK) $(LINKFLAGS) flooder.o main.o ../proto/proto.o ../common/ccommon.o $(patsubst %,-l%,$(LOCAL_LIBS)) $(LINKLIBS) -o $@

clean:
	rm -f $(TARGETS) $(FLOODER_OBJS) $(LIB_DIR)$(patsubst %,lib%.a, $(LOCAL_LIBS))
