
;========================================================================
; T=0 character I/O routines for 9600bps at 3.58 MHz
;========================================================================

#include <config.h>

	.arch	ARCH
IO_PIN=6
PINB=0x16
DDRB=0x17
PORTB=0x18
	.text
	.global recbytet0, sendbytet0, delay1etu
	.comm	direction,1,1
;========================================================================
; Wait loops.
; 70 cycles delay for intrabit delay

intrabitdelay:
	ldi		r22, 21			; 1 - 	LOAD IMMEDIATE

; Wait t17*3+7 cycles

delay:
	dec		r22			; 1 - 	DECREMENT
	brne		delay			; 1/2 - BRANCH IF NOT EQUAL
	ret					; 4 - 	RETURN

delay1etu:
	ldi		r22, 121		; 1 - 	LOAD IMMEDIATE
	rjmp		delay			; 2 - 	RELATIVE JUMP

;========================================================================
; Receive a byte with T=0 error correction.
; result r25(=0):r24
recbytet0:
	push		r23			; 2 - getbit
	push		r22			; 2 - delay
	push		r21			; 2 - loop counter
	push		r20			; 2 - parity counter

	; Set direction bit, to indicate, that we received a byte
	ldi		r22, 1			; LOAD IMMEDIATE
	sts		direction,r22		; STORE DIRECT TO DATA SPACE ($0016) 

restartrecbyte:
	; Setup IN direction
	cbi		DDRB, 6			; 2 -	CLEAR BIT IN I/O REGISTER
	cbi		PORTB, 6		; 2 -	CLEAR BIT IN I/O REGISTER

; Wait for start bit.
waitforstart:
	; Bit begins here.
	sbic		PINB, IO_PIN		; 1/2! - SET BIT IN I/O REGISTER IF CLEARED
	rjmp		waitforstart		; 2/0  - RELATIVE JUMP
	sbic		PINB, IO_PIN		; 1/2! - Recheck for spike
	rjmp		waitforstart		; 2/0  - RELATIVE JUMP
	; Sample start bit
	clr		r24			; 1 - CLEAR REGISTER
	clr		r25			; 1 - Clear zero byte for ADC
	ldi		r22, 31			; 1 - LOAD IMMEDIATE
	rcall		delay			; 100 - RELATIVE CALL SUBROUTINE
	rcall		getbit			; 3 (16bit PC) - RELATIVE CALL SUBROUTINE
	;brcs		waitforstart		; 1/2 - Go on, even if not valid a start bit? BRANCH IF CARRY SET
	nop					; 1 - For brcs NO OPERATION
; Receive now 9 bits
	ldi		r21, 0x09		; 1 - LOAD IMMEDIATE
	clr		r20			; 1 - CLEAR REGISTER
	ldi		r22, 66			; 1 - LOAD IMMEDIATE
	nop					; 1 - NO OPERATION
	nop					; 1 - NO OPERATION
rnextbit:
	rcall		delay			; 205/202 - RELATIVE CALL SUBROUTINE
	rcall		getbit			; 3 - RELATIVE CALL SUBROUTINE
	add		r20, r23		; 1 - ADD
	clc					; 1 - CLEAR CARRY
	sbrc		r23, 0			; 1/2 - SKIP BIT IF REGISTER CLEARED
	sec					; 1/0 - SET CARRY
	ror		r24			; 1 - ROTATE RIGHT THROUGH CARRY
	ldi		r22, 65			; 1 - LOAD IMMEDIATE
	dec		r21			; 1 - DECREMENT
	brne		rnextbit		; 1/2 - BRANCH IF NOT EQUAL
; Check parity
	rol		r24			; 1 - We've rotated one to much
	sbrc		r20, 0			; 1/2 - SKIP BIT IF REGISTER CLEARED
	rjmp		regetbyte		; 2/0 - RELATIVE JUMP

	; Wait halve etu
	ldi		r22, 76			; 1 - LOAD IMMEDIATE
	rcall		delay			; 235 - Precise enough

	clr		r25			; CLEAR REGISTER
	pop		r20			; 2 - parity counter - POP REGISTER FROM STACK
	pop		r21			; 2 - loop counter   - POP REGISTER FROM STACK
	pop		r22			; 2 - delay          - POP REGISTER FROM STACK
	pop		r23			; 2 - getbit	     - POP REGISTER FROM STACK
	ret					; RETURN

regetbyte:
	; Wait halve etu
	ldi		r22, 76			; 1   - LOAD IMMEDIATE
	rcall		delay			; 235 - Precise enough - RELATIVE CALL
	; Set OUT direction
	sbi		DDRB, 6			; 2 - SET BIT IN I/O REGISTER
	; Signal low
	cbi		PORTB, 6		; 2 - CLEAR BIT IN I/O REGISTER
	ldi		r22, 182		; 2 - LOAD IMMEDIATE
	rcall		delay			; 553 - about 1.5 etu - RELATIVE CALL
	rjmp		restartrecbyte		; 2 - RELATIVE JUMP

;========================================================================
; Read a bit.
; Uses r23, r25
; Returns bit in r23.0.
; 5 cycles before first bit
; 8 cycles after last bit.
getbit:
	clr		r23				; 1 - CLEAR REGISTER
	clc						; 1 - CLEAR CARRY
	; At start + 112 cycles
	sbic		PINB, IO_PIN			; 1/2 - SKIP IF BIT IN I/O REGISTER CLEARED
	sec						; 1/0 - SET CARRY
	adc		r23, r25			; 1 - ADD WITH CARRY
	rcall		intrabitdelay			; 70 - RELATIVE CALL
	clc						; 1 - CLEAR CARRY
	; At start + 186 cycles
	sbic		PINB, IO_PIN			; 1/2 - SKIP IF BIT IN REGISTER CLEARED
	sec						; 1/0 - SET CARRY
	adc		r23, r25			; 1 - ADD WITH CARRY
	rcall		intrabitdelay			; 70 - RELATIVE CALL
	clc						; 1 - CLEAR CARRY
	; At start + 260 cycles
	sbic		PINB, IO_PIN			; 1/2 - SKIP IF BIT IN REGISTER CLEARED
	sec						; 1/0 - SET CARRY
	adc		r23, r25			; 1 - ADD WITH CARRY
	; Get second bit of the sum.
	lsr		r23				; 1 - LOGICAL SHIFT RIGHT
	ret						; 4	(with 16bit PC) - RETURN
;========================================================================
; Send a byte with T=0 error correction.
; byte r25(=0):r24
sendbytet0:
	push		r22				; 2 - delay - PUSH REGISTER ON STACK
	push		r23				; 2 - parity counter - PUSH REGISTER ON STACK

	lds		r22,direction			; LOAD DATA FROM DIRECT SPACE
	tst		r22				; TEST FOR ZERO OR MINUS
	breq		resendbytet0			; BRANCH IF EQUAL
	rcall		delay1etu			; RELATIVE CALL
	rcall		delay1etu			; RELATIVE CALL
	; Clear direction bit, to indicate, that we sent a byte
	ldi		r22, 0				; LOAD IMMEDIATE
	sts		direction,r22			; STORE TO DATA SPACE

resendbytet0:
	; Set OUT direction
	sbi		PORTB, 6			; 2 - SET BIT IN I/O REGISTER
	sbi		DDRB, 6				; 2 - SET BIT IN I/O REGISTER
	; Send start bit
	cbi		PORTB, IO_PIN			; 2 - CLEAR BIT
	ldi		r22, 119			; 1 - LOAD IMMEDIATE
	rcall		delay				; 364 - RELATIVE CALL
	; Send now 8 bits
	ldi		r25, 0x08			; 1 - LOAD IMMEDIATE
	clr		r23				; 1 - CLEAR REGISTER
snextbit:
	ror		r24				; 1 - ROTATE RIGHT THROUGH CARRY
	brcs		sendbit1			; 1/2 - BRANCH IF CARRY SET
	cbi		PORTB, IO_PIN			; 2 - CLEAR BIT
	rjmp		bitset				; 2 - RELATIVE JUMP
sendbit1:
	sbi		PORTB, IO_PIN			; 2 - SET BIT IN I/O REGISTER
	inc		r23				; 1 - INCREMENT
bitset:
	ldi		r22, 118			; 1 - LOAD IMMEDIATE
	rcall		delay				; 361 - RELATIVE CALL
	nop						; 1 - NO OPERATION
	dec		r25				; 1 - DECREMENT
	brne		snextbit			; 1/2 - BRANCH IF NOT EQUAL
	; Send parity
	sbrc		r23, 0				; 1/2 - SKIP IF BIT IN REGISTER CLEARED
	rjmp		sendparity1			; 2 - RELATIVE JUMP
	nop						; 1 - NO OPERATION
	nop						; 1 - NO OPERATION
	cbi		PORTB, IO_PIN			; 2 - CLEAR BIT
	rjmp		delayparity			; 2 - RELATIVE JUMP
sendparity1:
	nop						; 1 - NO OPERATION
	sbi		PORTB, IO_PIN			; 2 - SET BIT
	nop						; 1 - NO OPERATION
	nop						; 1 - NO OPERATION
delayparity:
	ldi		r22, 112			; 1 - LOAD IMMEDIATE
	rcall		delay				; 343 - RELATIVE CALL
	; Stop bit
	sbi		PORTB, IO_PIN			; 2 - SET BIT
	ldi		r22, 119			; 1 - LOAD IMMEDIATE
	rcall		delay				; 364 - RELATIVE CALL
	; Set IN direction
	cbi		DDRB, 6				; 2 - CLEAR BIT
	cbi		PORTB, 6			; 2 - CLEAR BIT
	; Look for error signal
	clc						; 1 - CLEAR CARRY
	sbic		PINB, IO_PIN			; 1/2 - SKIP BIT IF I/O REGISTER CLEARED
	sec						; 1/0 - SET CARRY
	brcs		retsendbytet0			; 1/2 - BRANCH IF CARRY SET
	; Resend byte
	; Bring byte to starting position
	ror		r24				; 1 - ROTATE RIGHT THROUGH CARRY
	; Wait for end of error signal
waitforendoferror:
	sbic		PINB, IO_PIN			; 1/2! - SKIP BIT IF I/O REGISTER CLEARED
	rjmp		waitforendoferror		; 2/0 - RELATIVE JUMP
	; Wait then a halve etu
	ldi		r22, 58				; 1 - LOAD IMMEDIATE
	rcall		delay				; 181 - RELATIVE CALL
	rjmp		resendbytet0			; 2 - RELATIVE JUMP
	; return
retsendbytet0:
	ldi		r22, 116			; 1 - LOAD IMMEDIATE
	rcall		delay				; 355 - RELATIVE CALL
	pop		r23				; 2 - parity counter - POP REGISTER OFF STACK
	pop		r22				; 2 - delay - POP REGISTER OFF STACK
	ret						; 4
;========================================================================

