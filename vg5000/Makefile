# Project: dcvg5k

CC = gcc -Wall -g

objects = object/dcvg5kmain.o object/Z80.o object/dasmz80.o \
	object/dcef9345.o object/dcvg5kdesass.o object/dcvg5kboutons.o \
	object/dcvg5kdialog.o object/dcvg5kdevices.o object/dcvg5kemulation.o \
	object/dcvg5kkeyb.o object/dcvg5koptions.o object/dcvg5kvideo.o

dcvg5k : $(objects)
	$(CC) -o dcvg5k $(objects) -lSDL -lSDL_ttf

object/dcvg5kmain.o : source/dcvg5kmain.c source/dcvg5kmsg.h include/policettf.h include/dcvg5kicon.h
	$(CC) -c source/dcvg5kmain.c -o object/dcvg5kmain.o -O2

object/Z80.o : Z80/Z80.c
	$(CC) -c Z80/Z80.c -o object/Z80.o -O2

object/dasmz80.o : Z80/dasmz80.c
	$(CC) -c Z80/dasmz80.c -o object/dasmz80.o -O2

object/%.o : source/%.c
	$(CC) -c $^ -o $@ -O2

clean :
	rm dcvg5k $(objects)
