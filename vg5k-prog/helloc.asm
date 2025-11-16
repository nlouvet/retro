;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 3.8.0 #10562 (Linux)
;--------------------------------------------------------
	.module helloc
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _main
	.globl _print
	.globl _test
	.globl _hello
;--------------------------------------------------------
; special function registers
;--------------------------------------------------------
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _DATA
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _INITIALIZED
G$hello$0_0$0==.
_hello::
	.ds 7
;--------------------------------------------------------
; absolute external ram data
;--------------------------------------------------------
	.area _DABS (ABS)
;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
	.area _HOME
	.area _GSINIT
	.area _GSFINAL
	.area _GSINIT
;--------------------------------------------------------
; Home
;--------------------------------------------------------
	.area _HOME
	.area _HOME
;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area _CODE
	G$test$0$0	= .
	.globl	G$test$0$0
	C$helloc.c$1$0_0$1	= .
	.globl	C$helloc.c$1$0_0$1
;helloc.c:1: void test() __naked
;	---------------------------------
; Function test
; ---------------------------------
_test::
	C$helloc.c$5$1_0$1	= .
	.globl	C$helloc.c$5$1_0$1
;helloc.c:5: __endasm;
	call	#0x116d
	C$helloc.c$6$1_0$1	= .
	.globl	C$helloc.c$6$1_0$1
;helloc.c:6: }
	G$print$0$0	= .
	.globl	G$print$0$0
	C$helloc.c$8$1_0$3	= .
	.globl	C$helloc.c$8$1_0$3
;helloc.c:8: void print(char *ch) {
;	---------------------------------
; Function print
; ---------------------------------
_print::
	C$helloc.c$16$1_0$3	= .
	.globl	C$helloc.c$16$1_0$3
;helloc.c:16: __endasm;
	ld	iy,#2
	add	iy,sp ;Bypass the return address of the function
	ld	l,(iy) ;x
	ld	h,1(iy) ;y
	call	#0x36AA
	C$helloc.c$17$1_0$3	= .
	.globl	C$helloc.c$17$1_0$3
;helloc.c:17: }
	C$helloc.c$17$1_0$3	= .
	.globl	C$helloc.c$17$1_0$3
	XG$print$0$0	= .
	.globl	XG$print$0$0
	ret
	G$main$0$0	= .
	.globl	G$main$0$0
	C$helloc.c$21$1_0$4	= .
	.globl	C$helloc.c$21$1_0$4
;helloc.c:21: void main() {
;	---------------------------------
; Function main
; ---------------------------------
_main::
	C$helloc.c$23$1_0$4	= .
	.globl	C$helloc.c$23$1_0$4
;helloc.c:23: print(hello);
	ld	hl, #_hello
	push	hl
	call	_print
	pop	af
	C$helloc.c$24$1_0$4	= .
	.globl	C$helloc.c$24$1_0$4
;helloc.c:24: }
	C$helloc.c$24$1_0$4	= .
	.globl	C$helloc.c$24$1_0$4
	XG$main$0$0	= .
	.globl	XG$main$0$0
	ret
	.area _CODE
	.area _INITIALIZER
Fhelloc$__xinit_hello$0_0$0 == .
__xinit__hello:
	.ascii "Hello"
	.db 0x00
	.db 0x00
	.area _CABS (ABS)
