all: tst1.k7 tst2.k7 tst3.k7 tst4.k7 tst5.k7

%.k7: %.bin
	../cvt2k7/cvt2k7.py --type plainbin --start 6000 --exec 6000 $^ $@ $^

%.bin: %.s
	asm6809 $^ -o $@

clean:
	rm -f *.bin *.k7
