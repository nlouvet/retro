all: squash.k7

squash.k7: squash.bas
	./VG5KConv2k7.py --type bastok squash.bas squash.k7 SQUASH

squash.bas: squash.txt
	./VG5KToken.py squash.txt squash.bas

clean:
	rm -f *.bas *.k7
