	org	$6000
; main part of the program
main
	ldx #msg_dvd	; prepare the parameter pointer
	pshs x		; push the pointer on the stack
	jsr print
	ldb dvd
	pshs b
	jsr PRT8_DEC	; prt_byte_hex
	jsr prt_crlf
	
	ldx #msg_dvs	; prepare the parameter pointer
	pshs x		; push the pointer on the stack
	jsr print
	ldb dvs
	pshs b
	jsr PRT8_DEC	; prt_byte_hex
	jsr prt_crlf

	ldb dvd		; push the
	pshs b		;   dividend
	ldb dvs		; push the
	pshs b		;   divisor
	jsr DIV8	; division
	puls b		; pull the
	stb quo		;   quotient
	puls b		; pull the
	stb rem		;   remainder
	
	ldx #msg_quo	; prepare the parameter pointer
	pshs x		; push the pointer on the stack
	jsr print
	ldb quo
	pshs b
	jsr PRT8_DEC	; prt_byte_hex
	jsr prt_crlf
	
	ldx #msg_rem	; prepare the parameter pointer
	pshs x		; push the pointer on the stack
	jsr print
	ldb rem
	pshs b
	jsr PRT8_DEC	; prt_byte_hex
	jsr prt_crlf

	ldx #msg_tst	; prepare the parameter pointer
	pshs x		; push the pointer on the stack
	jsr print
	ldx #$A0B0
	pshs x
	jsr prt_2byte_hex
	jsr prt_crlf








	ldx #msg_dvd	; prepare the parameter pointer
	pshs x		; push the pointer on the stack
	jsr print
	ldx dvd2
	pshs x
	jsr PRT16_DEC	; prt_2byte_hex
	jsr prt_crlf
	
	ldx #msg_dvs	;. prepare the parameter pointer
	pshs x		; push the pointer on the stack
	jsr print
	ldx dvs2
	pshs x
	jsr PRT16_DEC
	jsr prt_crlf

	ldx dvd2	; push the
	pshs x		;   dividend
	ldx dvs2	; push the
	pshs x		;   divisor
	jsr DIV16	; division
	puls x		; pull the
	stx quo2	;   quotient
	puls x		; pull the
	stx rem2	;   remainder
	
	ldx #msg_quo	; prepare the parameter pointer
	pshs x		; push the pointer on the stack
	jsr print
	ldx quo2
	pshs x
	jsr PRT16_DEC
	jsr prt_crlf
	
	ldx #msg_rem	; prepare the parameter pointer
	pshs x		; push the pointer on the stack
	jsr print
	ldx rem2
	pshs x
	jsr PRT16_DEC
	jsr prt_crlf










	rts

counter	fcb	$00
dvd	fcb	33
dvs	fcb	5
quo	fcb	$00
rem	fcb	$00
dvd2	fdb	$FFFF
dvs2	fdb	$00BB
quo2	fdb	$0000
rem2	fdb	$0000
msg_dvd		fcn	"dividend:  "
msg_dvs		fcn	"divisor:   "
msg_quo		fcn	"quotient:  "
msg_rem		fcn	"remainder: "
msg_tst		fcn	"test: "

; Routine for dividing an unsigned one-byte integer
; by another one. The divisor and the divisor must
; be provided on the stack, as explained below.
; Stack before call (top to bottom):
;  0: MSB return
;  1: LSB address
;  2: divisor
;  3: dividend
; Stack after call:
;  0: quotient
;  1: remainder
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

; Routine for dividing an unsigned one-byte integer
; by another one. The divisor and the divisor must
; be provided on the stack, as explained below.
; Stack before call (top to bottom):
;  0: MSB return
;  1: LSB address
;  2: divisor
;  3: dividend
; Stack after call:
;  0: quotient
;  1: remainder
div16
	lda #16
	sta div16_count ; div16_count <- 16
	puls y		; save return address in y
	puls x 		; pull the divisor
	stx div16_dvs	; store the divisor
	puls x		; pull the dividend
	stx div16_reg   ; store the divisor
	clra		; the remainder will be computed
	clrb		;   in d, so we set it to 0
div16_loop
	asl div16_reg+1	; left shift the dividend
	rol div16_reg   ;   and the quotient
	rolb		; left shift the dividend
	rola		;   toward b
	cmpd div16_dvs  ; check if dividend < divisor
	blo div16_nosub ; branch if dividend < divisor
	subd div16_dvs	; d <- dividend - divisor
	inc div16_reg+1	; increment the quotient
div16_nosub
	dec div16_count ; decrement the counter
	bne div16_loop  ; loop until div16_count = 0
	ldx div16_reg   ; load the quotient into x
	*		; the remainder is already in d
	pshs d		; push the remainder
	pshs x		; push the quotient
	pshs y		; push back the return address
	rts		; return
; div88 variables
div16_dvs	RZB 2
div16_reg	RZB 2
div16_count	RZB 2

	include /print.s/
	include /div8.s/
	include /div16.s/

