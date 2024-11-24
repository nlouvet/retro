CRB	equ	$A7E3
ORB	equ	$A7E1

	org	$6000
	clr $A7E3	; clear CRB
	ldb #$01	;
	stb $A7E1	; store 0x01 into DDRB
	ldb #$04	;
	stb $A7E3	; set CRB to access DRB
loop
	ldb #$01
	stb $A7E1	; store 0x01 into DRB
	lda #250
	bsr delay
	ldb #$00
	stb $A7E1	; store 0x00 into DRB
	lda #250
	bsr delay
	bra loop



delay
	deca
	ldb #50
    	mul
	tfr d,x
	bsr delay_dly0
	ldx #46
	bsr delay_dly0
	rts
delay_dly0
	bra delay_dly1
delay_dly1
	bra delay_dly2
delay_dly2
	bra delay_dly3
delay_dly3
	bra delay_dly4
delay_dly4
	leax -1,X
	bne delay_dly0
	rts

