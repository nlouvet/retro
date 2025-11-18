#include <stdint.h>

void test() {
    __asm
    call #0x116d
    __endasm;
}

void crdo() {
    __asm
    call #0x3C57
    __endasm;
}


void print_int(uint16_t n) {
    __asm
    ld iy,#2
    add iy,sp    ; bypass the return address of the function 
    ld l,(iy)
    ld h,1(iy)
    call #0x0726
    __endasm;
}

void print_str(char *ch) {
    __asm
    ld iy,#2
    add iy,sp    ; bypass the return address of the function 
    ld l,(iy)
    ld h,1(iy)
    call #0x36AA
    __endasm;
}

void main() {
  char hello[7] = "Hello!";
  uint16_t A, B, C;

  print_str(hello); crdo();
  
  A = 2025;
  B = 11;
  print_str("  A = "); print_int(A); crdo();
  print_str("  B = "); print_int(B); crdo();
  print_str("A/B = "); print_int(A/B); crdo();
  print_str("A%B = "); print_int(A%B); crdo();
}
