#include "cmoc.h"

#define POKE(addr,val)     (*(unsigned char*) (addr) = (val))
#define POKEW(addr,val)    (*(unsigned*) (addr) = (val))

typedef unsigned char byte;

void CLS(void) {
  asm {
    ldb #12
    swi
    .byte 0x02
  }
}

void WAITCH(void) {
  asm {
@loop:
    swi
    .byte 0x0A
    beq @loop
  }
}

// makes PB0 an output
void init_pia(void) {
  asm {
    clr $A7E3	clear CRB
    ldb #$01
    stb $A7E1	store 0x01 into DDRB
    ldb #$04
    stb $A7E3	set CRB to access DRB
  }
}

void set_pb0_high(void) {
  asm {
    ldb #$01
    stb $A7E1	store 0x01 into DRB
  }
}

void set_pb0_low(void) {
  asm {
    ldb #$00
    stb $A7E1	store 0x00 into DRB
  }
}

// see https://github.com/jmatzen/leventhal-6809/blob/master/6809_DELAY.s
void delayms(byte n) {
  asm {
	LDA :n
	DECA
	LDB #50
    	MUL
	TFR D,X
	JSR @DLY0
	LDX #46
	JSR @DLY0
	BRA @END
@DLY0:	BRA @DLY1
@DLY1:	BRA @DLY2
@DLY2:	BRA @DLY3
@DLY3:	BRA @DLY4
@DLY4:	LEAX -1,X
	BNE @DLY0
	RTS
@END:
  }
}

int main() {
  unsigned int i, j;
  
  CLS();
  printf("Please press any key to start...\r\n");
  WAITCH();

  init_pia();
  
  for(i = 0; i < 1000; i++) {
    set_pb0_high();
    delayms(200);
    set_pb0_low();
    delayms(200);
  }

  printf("Finished\r\n");
  
  return 0;
}

