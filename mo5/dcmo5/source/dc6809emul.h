#ifndef __DC6809EMUL__
#define __DC6809EMUL__

#include <stdint.h>

extern int CC; // condition code
extern int16_t PC, D, X, Y, U, S, DA, W; // 6809 two byte registers + W work register
extern int8_t *PCHp, *PCLp, *Ap, *Bp;    // pointers to register bytes
extern int8_t *XHp, *XLp, *YHp, *YLp;    // pointers to register bytes
extern int8_t *UHp, *ULp, *SHp, *SLp;    // pointers to register bytes
extern int8_t *DPp, *DDp, *WHp, *WLp;    // pointers to register bytes

void Init6809registerpointers();
void Irq();
unsigned int Run6809(uint16_t *);

#endif