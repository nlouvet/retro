* Divide a 16-bit unsigned integer by another one,
* and compute the quotient and the remainder.
*
* Stack before the call (top to bottom):
* 0,S = 16-bit return value
* 2,S = 16-bit divisor
* 4,S = 16-bit dividend
*
* Stack after the call (top to bottom):
* 0,S = 16-bit quotient
* 2,S = 16-bit remainder
*
DIV16	PULS Y		; Y is used to store the return address
	LDB #16		;
	PSHS B		; 8-bit loop counter
	CLRA		;
	CLRB		;
	PSHS B,A	; remainder will be computed in D
	* Current stack state (top to bottom):
	* 0,S = 16-bit quotient
	* 2,S = 8-bit loop counter
	* 3,S = 16-bit divisor
	* 5,S = 16-bit dividend
D16010	LSL	5+1,S	; shift LSB of dividend into carry
	ROL	5,S	; shift carry and MSB of dividend, into carry
	ROLB		; new bit of dividend now in bit 0 of B
	ROLA		;
	CMPD	3,S	; does the divisor "fit" into D?
	BLO	D16020	; if not
	SUBD	3,S	;
	ORCC	#1	; set carry
	BRA	D16030	;
D16020	ANDCC	#$FE	; reset carry
D16030	ROL	0+1,S	; shift carry into quotient
	ROL	0,S	;
	DEC	2,S	; another bit of the dividend to process?
	BNE	D16010	; if yes
	* placing the results on the stack
	STD	5,S	; the remainder is stored at the bottom
	LDX	0,S	; 
	STX	3,S	; the quotient is placed above
	STY	1,S	; the return address on top
	LEAS	1,S	; restoring the correct stack top address
	RTS		;

* Print a 16-bit unsigned integer in decimal.
*
* Stack before the call (top to bottom):
* 0,S = 16-bit return address
* 2,S = 16-bit unsigned integer to be printed
*
* Stack after the call (top to bottom):
* empty
*
PRT16_DEC
	LDB #4			; the counter is
	PSHS B			; initialized to 4
	* Current stack state (top to bottom):
	* 0,S = 8-bit counter
	* 1,S = 16-bit return address
	* 3,S = 16-bit number to be printed
	LDD 3,S			; load the number to be printed into D
P16010	* beginning of the loop
	TST 0,S			; testing the counter
	BLT P16020		; stay in the loop while counter >= 0
	PSHS D			; push the dividend
	LDY #10			; divisor 10
	PSHS Y			; push the divisor 10
	JSR DIV16		; perform the division
	PULS Y			; pull the quotient
	PULS D			; pull the remainder
	ADDB #$30		; convert the lower byte of D to character
	LDX #P16_DEC_STR	; load the string address
	LDA 0,S			; load the counter current value
	STB A,X			; store the character at address X+A
	TFR Y,D			; transfer the quotient from Y to D
	DEC 0,S			; decrement the counter
	BRA P16010		; back to the beginning of the loop
	* print the number
P16020	LDX #P16_DEC_STR	; prepare the parameter pointer
	PSHS X			; push the pointer on the stack
	JSR print		; print the result
	* preparing the stack before return
	LDY 1,S			; we just keep the return
	STY 3,S			; address of the stack
	LEAS 3,S		;
	RTS			;
P16_DEC_STR
	FILL $00,6	; string

