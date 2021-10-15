NAME=noad
TARGET=libnss_$(NAME).so.2
OBJECTS=nss_$(NAME).o
SOURCES=nss_$(NAME).c
BITS=64
CFLAGS=-m$(BITS) -DDEBUG=1
LDFLAGS=-m$(BITS) -Wl,-export-dynamic -Wl,-version-script=map-file

all: $(TARGET)

$(TARGET): $(OBJECTS)
	gcc -shared -o libnss_$(NAME).so.2 -Wl,-soname,$(TARGET) $(OBJECTS) $(LDFLAGS)

nss_$(NAME).o: nss_$(NAME).c
	gcc -c $(CFLAGS) -fPIC -o nss_$(NAME).o nss_$(NAME).c

clean:
	-rm nss_$(NAME).o libnss_$(NAME).so*

install: all $(TARGET)
	sudo cp $(TARGET) /lib$(BITS)

fetch:
	wget `cat hosts.txt.url` -O hosts.txt
