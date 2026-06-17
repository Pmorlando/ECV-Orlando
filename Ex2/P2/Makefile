export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
INCLUDE_DIRS = -I/usr/local/include/opencv4
LIB_DIRS = -L/usr/local/lib
CC=g++
CDEFS=
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= -L/usr/local/lib -lopencv_core -lopencv_flann -lopencv_video -lrt

all: prob2Sobel

clean:
	-rm -f *.o *.d
	-rm -f prob2Sobel

prob2Sobel.o: prob2Sobel.cpp
	$(CC) $(CFLAGS) -c prob2Sobel.cpp -o prob2Sobel.o

prob2Sobel: prob2Sobel.o
	$(CC) $(CFLAGS) -o prob2Sobel prob2Sobel.o `pkg-config --libs opencv4` $(LIBS)
