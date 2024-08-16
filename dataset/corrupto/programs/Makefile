BIN_FILES  = libfunciones.so reproductor notas fotos navegador

CC = gcc

CPPFLAGS = -I$(INSTALL_PATH)/include -Wall

LDFLAGS = -L$(INSTALL_PATH)/lib/
LDLIBS = -lpthread -lm -lrt

SOFLAGS = -fPIC -shared -Wall

all: $(BIN_FILES)
.PHONY : all

reproductor: reproductor.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

notas: notas.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

fotos: fotos.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS) -lpng

navegador: navegador.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS) -lpng

libfunciones.so: funciones.c mensaje.o
	$(CC) $(CFLAGS) $(SOFLAGS) -o $@ $^ $(LDLIBS)
	
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clean:
	rm -rf $(BIN_FILES) *.o *.so

.SUFFIXES:
.PHONY : clean
