CC = gcc 
DEBUG = -g3 -O0 
CFLAGS = -I . $(DEBUG)
CDFLAGS = -arch x86_64
OUT = bin

LDFLAGS = -lPoKeys -L./src -lusb-1.0 -L/usr/lib/ -L/usr/local/include -Llibs \
		  -lconfig -luv -lzlog -lpthread -lProSimDataSource

SOURCES = ./src/main.c \
		  ./src/config/config.c \
		  ./src/device/pokey/pokey.c \
		  ./src/encoder/encoder.c \
		  ./src/pin/pin.c

OBJECTS = $(SOURCES:.c=.o)

pokey: $(OBJECTS)
	@mkdir -p $(OUT)
	$(CC) $(OBJECTS) -o $(OUT)/pokey $(CFLAGS) $(LDFLAGS) $(CDFLAGS)
	dsymutil $(OUT)/pokey
	install_name_tool -change libProSimDataSource.so ../libs/libProSimDataSource.so ./bin/pokey

clean:
	-rm -rf bin/pokey.dSYM/
	-rm -f src/*.o src/*.a src/pokey
	-rm -f src/encoder/*.o
	-rm -f src/pin/*.o
	-rm -f src/device/pokey/pokey.o
	-rm -f src/config/config.o

all: clean pokey

install: clean all
	mkdir -p logs
	touch logs/log
	brew install --HEAD libuv
	$(shell mkdir -p $(OUT))

