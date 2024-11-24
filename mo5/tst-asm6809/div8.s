* Divide an 8-bit unsigned integer by another one,
* and compute the quotient and the remainder.
*
* Stack before the call (top to bottom):
* 0,S = 16-bit return value
* 2,S = 8-bit divisor
* 3,S = 8-bit dividend
*
* Stack after the call (top to bottom):
* 0,S = quotient
* 1,S = remainder

DIV8	PULS Y		; save the return address in Y
	LDA #8		; loop counter
	CLRB            ; remainder will be computed in B
	PSHS B		; reserve a location for the quotient
* Current stack state (top to bottom):
* 0,S = 8-bit quotient
* 1,S = 8-bit divisor
* 2,S = 8-bit dividend
DV8010	LSL 2,S		; shift dividend into carry
	ROLB		; new bit of dividend now in bit 0 of B
	CMPB 1,S	; does the divisor "fit" into B?
	BLO DV8020	; if not, no subtraction
	SUBB 1,S	; subtract from the remainder
	ORCC #1		; and set carry
	BRA DV8030	; 
DV8020	ANDCC #$FE	; reset carry
DV8030	ROL 0,S		; shift carry into quotient
	DECA		; another bit of the dividend to process?
	BNE DV8010	; if yes
	; remainder is in B
	; quotient is on top of the stack
	LDA 0,S		; load the quotient
	LEAS 3,S	; restore the stack
	PSHS B,A	; push the remainder, then the quotient
	PSHS Y		; push the return address
	RTS


* Stack before the call (top to bottom):
* 0,S = 16-bit return value
* 2,S = 8-bit unsigned integer to be printed
*
* Stack after the call (top to bottom):
* empty
PRT8_DEC	
	LDB 2,S			; load the integer to be printed in B
	* compute the 1st (least significant) digit
	LDA #10
	PSHS B,A		; push the B (dividend), then A (divisor)
	JSR DIV8		; divide
	PULS A,B		; A quotient, B remainder
	ADDB #$30		; convert to character encoding
	STB PRT8_DEC_STRING+2	; store the character
	TFR A,B			; transfer the quotient from A to B
	* compute the 2d digit
	LDA #10
	PSHS B,A		; push the B (dividend), then A (divisor)
	JSR DIV8		; divide
	PULS A,B		; A quotient, B remainder
	ADDB #$30		; convert to character encoding
	STB PRT8_DEC_STRING+1	; store the character
	TFR A,B			; transfer A to B
	* compute the 3d digit
	LDA #10
	PSHS B,A		; push the B (dividend), then A (divisor)
	JSR DIV8		; divide
	PULS A,B		; A quotient, B remainder
	ADDB #$30		; convert to character encoding
	STB PRT8_DEC_STRING+0	; store the character
	* print the result
	LDX #PRT8_DEC_STRING	; prepare the parameter pointer
	PSHS x			; push the pointer on the stack
	JSR print		; print the result
	* Current stack state (top to bottom):
	* 0,S = 16-bit return value
	* 2,S = 8-bit unsigned integer
	LDX 0,S			; save the return address in X
	LEAS 3,S		; restore the stack
	PSHS X			; restore the return address
	RTS

PRT8_DEC_STRING
	FILL $00,4	; string
