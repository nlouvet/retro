void test() __naked
{
    __asm
    call #0x116d
    __endasm;
}

void print(char *ch) {
    __asm
    ld iy,#2
    add iy,sp ;Bypass the return address of the function 

    ld l,(iy)   ;x
    ld h,1(iy)  ;y
    call #0x36AA
    __endasm;
}

char hello[7] = "Hello\0";

void main() {
  /*test();*/
  print(hello);
}
