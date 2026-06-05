export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig

INCLUDE_DIRS = -I/usr/local/include/opencv4
LIB_DIRS = -L/usr/local/lib
CC = g++
CDEFS =
CFLAGS = -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS = -lrt
CPPLIBS = $(shell pkg-config --libs opencv4)
LIB_DIRS = -L/usr/local/lib
CC=g++

CDEFS=
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= -lrt
CPPLIBS= $(shell pkg-config --libs opencv4)

HFILES= 
CFILES= 
CPPFILES= prob5.cpp

SRCS= ${HFILES} ${CFILES}
CPPOBJS= ${CPPFILES:.cpp=.o}

all:	prob5 

clean:
	-rm -f *.o *.d
	-rm -f prob5

distclean:
	-rm -f *.o *.d

prob5: prob5.o
	@$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $@.o `pkg-config --libs opencv4` $(CPPLIBS)
	@echo "Great Success"

depend:

.c.o:
	$(CC) $(CFLAGS) -c $<

.cpp.o:
	@$(CC) $(CFLAGS) -c $<
