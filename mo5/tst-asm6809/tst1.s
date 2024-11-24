	org	$6000

; main part of the program
start
	ldx #msg1
	bsr print
	
	ldx #msg2
	bsr print	
	
	rts

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

