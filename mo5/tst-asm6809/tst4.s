	org	$6000
; main part of the program
main
	ldx #msg_dvd	; prepare the parameter pointer
	pshs x		; push the pointer on the stack
	jsr print
	ldb dvd
	pshs b
	jsr prt_byte_hex
	jsr prt_crlf
	
	ldx #msg_dvs	; prepare the parameter pointer
	pshs x		; push the pointer on the stack
	jsr print
	ldb dvs
	pshs b
	jsr prt_byte_hex
	jsr prt_crlf

	ldb dvd		; push the
	pshs b		;   dividend
	ldb dvs		; push the
	pshs b		;   divisor
	jsr div88	; division
	puls b		; pull the
	stb quo		;   quotient
	puls b		; pull the
	stb rem		;   remainder
	
	ldx #msg_quo	; prepare the parameter pointer
	pshs x		; push the pointer on the stack
	jsr print
	ldb quo
	pshs b
	jsr prt_byte_hex
	jsr prt_crlf
	
	ldx #msg_rem	; prepare the parameter pointer
	pshs x		; push the pointer on the stack
	jsr print
	ldb rem
	pshs b
	jsr prt_byte_hex
	jsr prt_crlf

	rts

counter	fcb	$00
dvd	fcb	33
dvs	fcb	5
quo	fcb	$00
rem	fcb	$00
msg_dvd		fcn	"dividend:  "
msg_dvs		fcn	"divisor:   "
msg_quo		fcn	"quotient:  "
msg_rem		fcn	"remainder: "

; Routine for printing a 0-terminating
; string whose address is on the stack.
; Stack before call (top to bottom):
; - 1: MSB return address
; - 2: LSB
; - 3: MSB string address
; - 4: LSB
; Stack after call: empty
print
	puls y		; save return address in y
	puls x 		; pull string address in x
	pshs y		; push back the return address
print_loop
	ldb ,x+
	beq print_endloop
	swi
	fcc $02
	bra print_loop
print_endloop
	rts

; Routine for printing a byte as an hexadecimal number.
; The byte to be printed must be provided on the stack.
; Stack before call (top to bottom):
; - 1: MSB return
; - 2: LSB address
; - 3: byte to be printed
; Stack after call: empty
prt_byte_hex
	puls y		; save return address in y
	puls b		; the byte to be printed
	pshs y		; restore the return address
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
	rts

; Routine for dividing an unsigned one-byte integer
; by another one. The divisor and the divisor must
; be provided on the stack, as explained below.
; Stack before call (top to bottom):
; - 1: MSB return
; - 2: LSB address
; - 3: divisor
; - 4: dividend
; Stack after call:
; - 1: quotient
; - 2: remainder
div88
	puls y		; save return address in y
	puls b 		; pull the divisor
	stb div88_dvs	; store the divisor
	puls a		; pull the dividend into a
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
	pshs b		; push the remainder
	pshs a		; push the quotient
	pshs y		; push back the return address
	rts		; return
; div88 variables
div88_dvs	fcc $00
div88_quo	fcc $00
div88_count	fcc $00

; Routine for printing a CRLF.
; All registers except CC are preserved.
prt_crlf
	ldb #$0D	; character CR = '\r'
	swi		; print the
	fcc $02		;   character
	ldb #$0A	; character CR = '\n'
	swi		; print the
	fcc $02		;   character
	rts
