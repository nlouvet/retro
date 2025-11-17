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
	C$helloc.c$19$1_0$4	= .
	.globl	C$helloc.c$19$1_0$4
;helloc.c:19: void main() {
;	---------------------------------
; Function main
; ---------------------------------
_main::
	ld	hl, #-7
	add	hl, sp
	ld	sp, hl
	C$helloc.c$20$2_0$4	= .
	.globl	C$helloc.c$20$2_0$4
;helloc.c:20: char hello[7] = "Hello\0";
	ld	hl, #0x0000
	add	hl, sp
	ex	de, hl
	ld	a, #0x48
	ld	(de), a
	ld	l, e
	ld	h, d
	inc	hl
	ld	(hl), #0x65
	ld	l, e
	ld	h, d
	inc	hl
	inc	hl
	ld	(hl), #0x6c
	ld	l, e
	ld	h, d
	inc	hl
	inc	hl
	inc	hl
	ld	(hl), #0x6c
	ld	hl, #0x0004
	add	hl, de
	ld	(hl), #0x6f
	ld	hl, #0x0005
	add	hl, de
	ld	(hl), #0x00
	ld	hl, #0x0006
	add	hl, de
	ld	(hl), #0x00
	C$helloc.c$22$1_0$4	= .
	.globl	C$helloc.c$22$1_0$4
;helloc.c:22: print(hello);
	push	de
	call	_print
	pop	af
	C$helloc.c$23$1_0$4	= .
	.globl	C$helloc.c$23$1_0$4
;helloc.c:23: }
	ld	hl, #7
	add	hl, sp
	ld	sp, hl
	C$helloc.c$23$1_0$4	= .
	.globl	C$helloc.c$23$1_0$4
	XG$main$0$0	= .
	.globl	XG$main$0$0
	ret
	.area _CODE
	.area _INITIALIZER
	.area _CABS (ABS)
