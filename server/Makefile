BIN_FILES = server

CC = gcc

CPPFLAGS = -I$(INSTALL_PATH)/include -Wall

LDFLAGS = -L$(INSTALL_PATH)/lib/
LDLIBS = -lpthread -lm -lrt

all: $(BIN_FILES)
.PHONY : all

server: server.o funciones.o mensaje.o utils.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clean:
	rm -rf $(BIN_FILES) *.o

.SUFFIXES:
.PHONY : clean
