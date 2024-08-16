BIN_FILES  = libfunciones.so client

CC = gcc

CPPFLAGS = -I$(INSTALL_PATH)/include -Wall

LDFLAGS = -L$(INSTALL_PATH)/lib/
LDLIBS = -lpthread -lm -lrt

SOFLAGS = -fPIC -shared -Wall

all: $(BIN_FILES)
.PHONY : all

client: client.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

libfunciones.so: funciones.c mensaje.o
	$(CC) $(CFLAGS) $(SOFLAGS) -o $@ $^ $(LDLIBS)
	
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clean:
	rm -rf $(BIN_FILES) *.o *.so

.SUFFIXES:
.PHONY : clean
