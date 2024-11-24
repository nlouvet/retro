	org	$6000
; main part of the program
main
	ldx #msg1
	jsr print

	ldb #$FF
	stb loop_idx
main_loop
	ldb loop_idx
	jsr prt_byte_hex
	
	dec loop_idx
	bne main_loop

	jsr prt_crlf

	lda dvd
	ldb dvs
	jsr div88
	sta quo
	stb rem
	
	ldb quo
	jsr prt_byte_hex
	jsr prt_crlf
	
	ldb rem
	jsr prt_byte_hex
	jsr prt_crlf
	
	ldb #255
	jsr prt_byte_dec
	jsr prt_crlf
	
	rts

dvd		fcb $255
dvs		fcb $255
quo		fcb $00
rem		fcb $00

loop_idx	fcb $00
msg1		fcn "Values: "
msg2		fcn "\r\n"

; Routine for printing a CRLF.
; All registers except CC are preserved.
prt_crlf
	pshs b
	ldb #$0D	; character CR = '\r'
	swi		; print the
	fcc $02		;   character
	ldb #$0A	; character CR = '\n'
	swi		; print the
	fcc $02		;   character
	puls b,pc	; restore register b and return

; Routine for printing a byte as an hexadecimal number
; the byte to be printed must be in register B.
; All registers except CC are preserved.
prt_byte_hex
	pshs a,b	; save a and b that are used
	; print the most significand hex digit
	tfr b,a		; a <- b
	lsrb		; shift B
	lsrb            ;   by 4 bits
	lsrb		;   on the right
	lsrb		; higher quartet
	addb #$30	; $30 is the ascii code of '0'
	cmpb #$3A	; is a correction necessary?
	blt prt_byte_hex_0
	addb #7		; correction A -> F
prt_byte_hex_0
	swi		; print the
	fcc $02		;   first letter
	; print the less significand hex digit
	tfr a,b		; b <- a
	andb #$0F	; lower quartet
	addb #$30	; $30 is the ascii code of '0'
	cmpb #$3A	; is a correction necessary?
	blt prt_byte_hex_1
	addb #7		; correction A -> F
prt_byte_hex_1
	swi		; print the
	fcc $02		;   character in b
	puls a,b,pc	; restore a, b and return

; Routine for dividing an unsigned one-byte integer
; provided in A by a second byte unsigned one-byte
; integer provided in B. After calling the routine,
; A contains the quotient, and B the remainder.
; Registers A, B and CC are the only modified ones.
div88
	sta div88_dvd	; dividend
	stb div88_dvs	; divisor
	ldb #8
	stb div88_count ; div88_count <- 8
	clrb		; remainder will be computed in b
	clr div88_quo   ; the quotient in div88_quo
div88_loop
	asl div88_quo	; left shift the quotient
	asla		; left shift the dividend
	rolb
	cmpb div88_dvs  ; check if dividend < divisor
	blo div88_nosub ; branch if dividend < divisor
	subb div88_dvs	; b <- dividend - divisor
	inc div88_quo	; increment the quotient
div88_nosub
	dec div88_count ; decrement the counter
	bne div88_loop  ; loop until div88_count = 0
	lda div88_quo   ; load the quotient into a
	*		; the remainder is already in b
	rts		; return
; div88 variables
div88_dvd	fcc $00
div88_dvs	fcc $00
div88_quo	fcc $00
div88_count	fcc $00

; Simple routine for printing an unsigned one-byte
; provided in B. All registers except CC are preserved.
prt_byte_dec
	pshs a,b,x
	stb prt_byte_dec_var	; store the parameter b
	ldb #3			; initialize the counter
	stb prt_byte_dec_cnt	; to the value 3
	ldx #prt_byte_dec_str+3	; init. the string pointer
prt_byte_dec_0
	lda prt_byte_dec_var	; dividend
	ldb prt_byte_dec_ten	; divisor
	jsr div88
	sta prt_byte_dec_var	; store the quotient
	*			; b contains the remainder
	addb #$30		; $30 is the ascii code of '0'
	stb ,-x			; dec. the pointer, then store the digit
	dec prt_byte_dec_cnt	; update the iteration counter
	bne prt_byte_dec_0	; loop until the counter is 0
	jsr print
	puls a,b,x,pc		; restore saved registers and return
; prt_byte_dec variables
prt_byte_dec_cnt	fcb $00		; iteration counter
prt_byte_dec_str	fill $00,4	; string to be printed
prt_byte_dec_ten	fcb $0A		; constant 10
prt_byte_dec_var	fcb $00		; the integer to be printed

; Routine for printing a 0-terminating
; before call, register X must contain the address of
; the first character to be printed.
; All registers except CC are preserved.
print
	pshs b
print_loop
	ldb ,x+
	cmpb #$00
	beq print_end
	swi
	fcc $02
	bra print_loop
print_end
	puls b,pc

