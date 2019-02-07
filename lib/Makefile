#
# CMPSC311 - Utility Library 
# Code for the CMPSC311 Projects
#

# Variables
ARCHIVE=ar
CC=gcc
LINK=gcc
CFLAGS=-c -fPIC -Wall -I. -I/opt/local/include
LINKFLAGS=-L. -g -pthread -L/opt/local/lib
LIBFLAGS=-shared -Wall -pthread 
LINKLIBS=-lcmpsc311 -lgcrypt 
DEPFILE=Makefile.dep
TARGETDIR=~/bin

# Files to build

CMPSC311_LIBOBJS=	cmpsc311_log.o \
					cmpsc311_util.o \
					cmpsc311_hashtable.o \
					cmpsc311_cache.o \
					cmpsc311_network.o \
					cmpsc311_opcode.o \
					cmpsc311_capture.o \
					cmpsc311_unittest.o \
					cmpsc311_uncapture.o
	
CMPSC311MINI_LIBOBJS=	cmpsc311_log.o \
						cmpsc311_util.o \
						cmpsc311_network.o \

INCLUDES=	cmpsc311_cache.h \
			cmpsc311_capture.h \
			cmpsc311_hashtable.h \
			cmpsc311_log.h \
			cmpsc311_network.h \
			cmpsc311_opcode.h \
			cmpsc311_unittest.h \
			cmpsc311_util.h

LIBS=       libcmpsc311.a \
			libcmpsc311mini.a

TARGETS=    utest311 \
			capread311
                    
# Suffix rules
.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS)  -o $@ $<

# Productions

all : $(TARGETS) 
    
utest311 : utest311.o $(LIBS)
	$(LINK) $(LINKFLAGS) utest311.o $(LINKLIBS) -o $@ 
	
capread311 : cmpsc311_uncapture.o $(LIBS)
	$(LINK) $(LINKFLAGS) cmpsc311_uncapture.o $(LINKLIBS) -lcurl -o $@ 	

# Library Make

libcmpsc311.a : $(CMPSC311_LIBOBJS)
	$(ARCHIVE) rc $@ $^
	$(ARCHIVE) s $@ $^

libcmpsc311mini.a : $(CMPSC311MINI_LIBOBJS) 
	$(ARCHIVE) rc $@ $^
	$(ARCHIVE) s $@ $^

# Do dependency generation
depend : $(DEPFILE)

$(DEPFILE) : $(CMPSC311_LIBOBJS:.o=.c)
	gcc -MM $(CFLAGS) $(CMPSC311_LIBOBJS:.o=.c) utest311.c > $(DEPFILE)
        
# Cleanup 
clean:
	rm -f $(TARGETS) $(LIBS) $(CMPSC311_LIBOBJS) utest311.o

# Install to local environment
install:
	mkdir -p $(TARGETDIR)/lib
	install -C $(LIBS) $(TARGETDIR)/lib
	mkdir -p $(TARGETDIR)/include
	install -C $(INCLUDES) $(TARGETDIR)/include
  
# Dependancies
include $(DEPFILE)
