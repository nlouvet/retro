#ifndef __DC6809EMUL__
#define __DC6809EMUL__

extern int CC; // condition code
extern short PC, D, X, Y, U, S, DA,
    W;                              // 6809 two byte registers + W work register
extern char *PCHp, *PCLp, *Ap, *Bp; // pointers to register bytes
extern char *XHp, *XLp, *YHp, *YLp; // pointers to register bytes
extern char *UHp, *ULp, *SHp, *SLp; // pointers to register bytes
extern char *DPp, *DDp, *WHp, *WLp; // pointers to register bytes

void Init6809registerpointers();
void Irq();
int Run6809();

#endif