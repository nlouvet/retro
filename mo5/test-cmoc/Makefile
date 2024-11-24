
CVT2K7=/home/nlouvet/docsvn/github/mo5/cvt2k7/cvt2k7.py
CMOC=cmoc

all: test.k7

test.k7: test.bin
	$(CVT2K7) --type prgbin test.bin test.k7 test.bin

test.bin:test.c
	$(CMOC) --thommo --org=0x6000 -o test.bin test.c

clean:
	rm -f test.bin test.k7

