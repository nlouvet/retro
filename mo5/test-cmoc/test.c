#include "cmoc.h"

#define POKE(addr,val)     (*(unsigned char*) (addr) = (val))
#define POKEW(addr,val)    (*(unsigned*) (addr) = (val))

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

void PLOT(unsigned int x, unsigned int y) {
  asm {
    ldx :x
    ldy :y
    swi
    .byte 0x10
  }
}

void LINE(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1) {
  POKEW(0x2032, x0);
  POKEW(0x2034, y0);
  asm {
    ldx :x1
    ldy :y1
    swi
    .byte 0x0E
  }
}

void primes(unsigned int n) {
  unsigned int i, j;
  printf("Prime numbers less than %d\r\n", n);
  for(i = 1; i < n; i++) {
    for(j = 2; j < i; j++) {
      if(i % j == 0) break;
    }
    if(j == i) printf("%d ", i);
  }
  printf("\r\n");
}

#define NMAX 2000

void sieve(unsigned int n) {
  unsigned char s[NMAX];
  unsigned int i, j;
  
  if(n > NMAX) n = NMAX;
  
  printf("Sieving prime numbers less than %d...\r\n", n);
  
  s[0] = 0;
  for(i = 0; i < n; i++) s[i] = 1;
  for(i = 2; i < n; i++) {
    if(s[i] == 1) {
      for(j = i*i; j < n; j += i) {
        s[j] = 0;
      }
    }
  }
  
  printf("Prime numbers found:\r\n");
  
  for(i = 0; i < n; i++) {
    if(s[i] == 1) {
      printf("%d ", i);
    }
  }
  printf("End of the list.\r\n");
}

int main() {
  int i, n = 10;
  unsigned int a = 2024;
  char *pline;

  CLS();
  printf("Please enter something: ");
  pline = readline();
  printf("\n");
  printf("You entered: \r\n\r\n%s\r\n\r\n", pline);
  printf("Please press any key...");
  WAITCH();
  
  CLS();
  sieve(1000);
  printf("Please press any key...");
  WAITCH();

  CLS();
  primes(1000);
  printf("Please press any key...");
  WAITCH();
  
  CLS();
  for(i = 0; i < 200; i+=5) {
    LINE(0, i, 319, 199-i);
  }
  WAITCH();
    
  CLS();
  printf("Finished\r\n");
  
  return 0;
}

