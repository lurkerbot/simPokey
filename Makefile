CC = gcc
CFLAGS = -I .
CDFLAGS = -fPIC -fPIC
DEBUG = -g 
LDFLAGS = -lPoKeys -L./src -lusb-1.0 -L/usr/lib/ -lconfig -lzlog -lpthread
SOURCES = ./src/main.c \
		  ./src/config.c \
		  ./src/devices.c \
		  ./src/encoder/encoder.c \
		  ./src/pin/pin.c
		  
OBJECTS = $(SOURCES:.c=.o)

pokey: $(OBJECTS)
	$(CC) $(OBJECTS) -o src/pokey $(CFLAGS) $(LDFLAGS) $(DEBUG) $(CDFLAGS) -Wno-gnu-binary-literal

clean:
	rm -f src/*.o src/*.a src/pokey
	rm -f src/encoder/*.o
	rm -f src/pin/*.o

all: clean pokey

install: clean all
	mkdir -p logs
	touch logs/log