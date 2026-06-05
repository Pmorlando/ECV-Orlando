export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig

CC= g++
CFLAGS= -O3 -I/usr/local/include/opencv4 
LIBS = $(shell pkg-config --libs opencv4)

PRODUCT= facefind
CFILES= facefind.cpp

OBJS= $(CFILES:.cpp=.o)
	

all: $(PRODUCT)

$(PRODUCT): $(OBJS)
	$(CC) $(CFLAGS) -o $(PRODUCT) $(OBJS) $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(PRODUCT)

