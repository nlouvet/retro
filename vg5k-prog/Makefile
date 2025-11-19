all: squash.k7 hello.k7 helloc.k7

###################################################################
# Conversion of basic source code, to its tokenized form.

squash.k7: squash.bas
	./VG5KConv2k7.py --type bastok squash.bas squash.k7 SQUASH

squash.bas: squash.txt
	./VG5KToken.py squash.txt squash.bas

###################################################################
# Assembling a simple source code file.

hello.k7: hello.bin
	./VG5KConv2k7.py --type binary --start 0x7000 hello.bin hello.k7 hello

hello.bin: hello.asm
	z80asm hello.asm -o hello.bin

###################################################################
# Compiling a C source code with SDCC.

helloc.k7: helloc.bin
	./VG5KConv2k7.py --type binary --start 0x7000 helloc.bin helloc.k7 HELLOC

# --debug --nostdinc --no-std-crt0 --nostdlib \

helloc.bin: helloc.c crt0.s
	sdasz80 -xlos -g crt0.s
	sdcc -o helloc.rel -c --std-c11 -mz80 \
		--debug --no-std-crt0 --nostdlib \
		helloc.c
	sdcc -o helloc.ihx -mz80 -Wl -y --code-loc 0x7000 --data-loc 0x7100 --std-c11 \
		--debug --no-std-crt0  --nostdlib \
		crt0.rel helloc.rel
	sdobjcopy -I ihex -O binary helloc.ihx helloc.bin

clean:
	rm -f *.bas *.k7 *.bin
	rm -f *.ihx *.adb *.cdb *.lk *.lst *.map *.noi *.rel *.sym
