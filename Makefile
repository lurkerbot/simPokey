

CC = gcc 
DEBUG = -g3 -O0 
CFLAGS = -I . $(DEBUG)
CDFLAGS = -arch x86_64

LDFLAGS = -lPoKeys -L./src -lusb-1.0 -L/usr/lib/ -lconfig -lzlog -lpthread
SOURCES = ./src/main.c \
		  ./src/config.c \
		  ./src/devices.c \
		  ./src/encoder/encoder.c \
		  ./src/pin/pin.c
		  
OBJECTS = $(SOURCES:.c=.o)


pokey: $(OBJECTS)
	$(CC) $(OBJECTS) -o src/pokey $(CFLAGS) $(LDFLAGS) $(CDFLAGS)
	dsymutil ./src/pokey

clean:
	rm -rf src/pokey.dSYM/
	rm -f src/*.o src/*.a src/pokey
	rm -f src/encoder/*.o
	rm -f src/pin/*.o

all: clean pokey

install: clean all
	mkdir -p logs
	touch logs/log