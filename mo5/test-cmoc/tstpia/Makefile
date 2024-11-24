
CVT2K7=/home/nlouvet/docsvn/github/mo5/cvt2k7/cvt2k7.py
CMOC=cmoc

all: tstpia.k7

tstpia.k7: tstpia.bin
	$(CVT2K7) --type prgbin --exec 6000 tstpia.bin tstpia.k7 tstpia.bin

tstpia.bin:tstpia.c
	$(CMOC) --thommo --org=0x6000 -o tstpia.bin tstpia.c

clean:
	rm -f tstpia.bin tstpia.k7

