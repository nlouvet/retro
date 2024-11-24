	org	$6000

; main part of the program
start
	lda #4
	sta cmpt
tmpo1	lda #250
	bsr delay	
	lda cmpt
	deca
	sta cmpt
	tst cmpt
	bne tmpo1
	ldx #msg1
	bsr print
	lda 250
	bsr delay
	lda 250
	bsr delay
	lda 250
	bsr delay
	lda 250
	bsr delay
	ldx #msg2
	bsr print	
	bra start
end
	rts

cmpt	fcb 16
msg1	fcn "Toto fait du velo !!!\r\n"
msg2	fcn "Tutu fait du pedalo :D\r\n"


; routine for printing a 0-terminating string
; the x register must contain the address of
; the first character of the string before call
print
	ldb ,x+
	cmpb #$00
	beq end_print
	swi
	fcc $02
	bra print
end_print
	rts

; ms delay
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



