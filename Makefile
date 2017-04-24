CC = gcc
CXX = g++ 
DEBUG = -g3 -O0 
CFLAGS = -I. -I./src $(DEBUG) -stdlib=libc++
CXXFLAGS = -stdlib=libc++ -std=c++14
CDFLAGS = -arch x86_64
OUT = bin

LDFLAGS = -lPoKeys -L./src -lusb-1.0 -L/usr/lib/ -L/usr/local/include -Llibs \
		  -lconfig -luv -lzlog -lpthread -lProSimDataSource -lcli -stdlib=libc++ -lc++

SOURCES = ./src/main.c \
		  ./src/config/config.c \
		  ./src/device/pokey/pokey.c \
		  ./src/encoder/encoder.c \
		  ./src/pin/pin.c \
		  ./src/cli/cli.c 

CXXSOURCES = $(wildcard src/*.cpp)

OBJECTS = $(SOURCES:.c=.o) $(CXXSOURCES:.cpp=.o)

pokey: $(OBJECTS) 	 
	@mkdir -p $(OUT)
	$(CXX) $(CXXSOURCES) -o $(OUT)/pokey $(CXXFLAGS) $(LDFLAGS) $(CDFLAGS)
	$(CC) $(OBJECTS) -o $(OUT)/pokey $(CFLAGS) $(LDFLAGS) $(CDFLAGS)
	dsymutil $(OUT)/pokey
	install_name_tool -change libProSimDataSource.so ../libs/libProSimDataSource.so ./bin/pokey

clean:
	-rm -rf bin/pokey.dSYM/
	-rm -f src/*.o src/*.a src/pokey
	-rm -f src/encoder/*.o
	-rm -f src/pin/*.o
	-rm -f src/cli/*.o
	-rm -f src/device/pokey/pokey.o
	-rm -f src/config/config.o

all: clean pokey

install: clean all
	mkdir -p logs
	touch logs/log
	brew install --HEAD libuv
	brew install libcli
	$(shell mkdir -p $(OUT))

