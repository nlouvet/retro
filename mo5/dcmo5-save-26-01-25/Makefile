# Project: dcmo5

CFLAGS = -Wall -ggdb -O2 $(shell sdl-config --cflags) $(shell pkg-config --cflags SDL_ttf)
LDFLAGS = $(shell sdl-config --libs) $(shell pkg-config --libs SDL_ttf)

objects = object/dcmo5main.o object/dc6809emul.o object/dcmo5boutons.o \
	object/dcmo5dialog.o object/dcmo5devices.o object/dcmo5emulation.o \
	object/dcmo5keyb.o object/dcmo5options.o object/dcmo5video.o \
	object/dcmo5pckeycode.o

all: dcmo5

dcmo5 : $(objects)
	cc $(objects) $(LDFLAGS) -o dcmo5

object/dcmo5main.o : source/dcmo5main.c source/dcmo5msg.c include/policettf.h include/dcmo5icon.h object
	cc -c source/dcmo5main.c -o object/dcmo5main.o $(CFLAGS)

object/dc6809emul.o : source/dc6809emul.c object
	cc -c source/dc6809emul.c -o object/dc6809emul.o $(CFLAGS)

object/dcmo5boutons.o : source/dcmo5boutons.c source/dcmo5boutons.h object
	cc -c source/dcmo5boutons.c -o object/dcmo5boutons.o $(CFLAGS)

object/dcmo5dialog.o : source/dcmo5dialog.c object
	cc -c source/dcmo5dialog.c -o object/dcmo5dialog.o $(CFLAGS)

object/dcmo5devices.o : source/dcmo5devices.c object
	cc -c source/dcmo5devices.c -o object/dcmo5devices.o $(CFLAGS)

object/dcmo5emulation.o : source/dcmo5emulation.c include/dcmo5rom.h object
	cc -c source/dcmo5emulation.c -o object/dcmo5emulation.o $(CFLAGS)

object/dcmo5keyb.o : source/dcmo5keyb.c source/dcmo5keyb.c object
	cc -c source/dcmo5keyb.c -o object/dcmo5keyb.o $(CFLAGS)

object/dcmo5options.o : source/dcmo5options.c object
	cc -c source/dcmo5options.c -o object/dcmo5options.o $(CFLAGS)

object/dcmo5video.o : source/dcmo5video.c object
	cc -c source/dcmo5video.c -o object/dcmo5video.o $(CFLAGS)

object/dcmo5pckeycode.o : source/dcmo5pckeycode.c object
	cc -c $< -o $@ $(CFLAGS)

object :
	mkdir -p object

clean :
	rm -fr dcmo5 object $(objects)
