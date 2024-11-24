	org	$6000
; main part of the program
main
	ldb #5
main_loop
	beq main_end_loop
	
	ldx #msg1	; prepare the parameter pointer
	pshs x		; push the pointer on the stack
	jsr print
	puls x

	ldx #msg2	; prepare the parameter pointer
	pshs x		; push the pointer on the stack
	jsr print
	puls x

	decb
	bra main_loop
main_end_loop
	rts

msg1
	fcn "Hello world!!! "
msg2
	fcn "42 is the answer!\r\n"

; routine for printing a 0-terminating
; string whose address is on the stack
print
	pshs cc,b
	ldx 4,s	; load in x the address of the string
print_loop
	ldb ,x+
	cmpb #$00
	beq print_end
	swi
	fcc $02
	bra print_loop
print_end
	puls cc,b
	rts

