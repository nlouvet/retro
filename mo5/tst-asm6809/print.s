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
1	; beginning of the printing loop
	ldb ,x+
	beq 2f
	swi
	fcc $02
	bra 1b
2	; end of the printing loop
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
	blt 1f		; no -> branch forward to label 1
	addb #7		; correction A -> F
1	swi		; print the
	fcc $02		;   first letter
	; print the less significand hex digit
	tfr a,b		; b <- a
	andb #$0F	; lower quartet
	addb #$30	; $30 is the ascii code of '0'
	cmpb #$3A	; is a correction necessary?
	blt 2f		; no -> branch forward to label 2
	addb #7		; correction A -> F
2	swi		; print the
	fcc $02		;   character in b
	rts

; Routine for printing a 2-byte word as an hexadecimal number.
; The bytes to be printed must be provided on the stack.
; Stack before call (top to bottom):
; - 0: MSB return
; - 1: LSB address
; - 2: MSB byte to be printed
; - 3: LSB byte to be printed
; Stack after call: empty
prt_2byte_hex
	puls y			; save return address in y
	puls a			; MSB byte to be printed
	puls b			; LSB byte to be printed
	pshs y			; restore the return address
	pshs b			; prepare the stack for printing LSB
	pshs a			; prepare the stack for printing MSB
	jsr prt_byte_hex	; print the MSB
	jsr prt_byte_hex	; print the LSB
	rts
