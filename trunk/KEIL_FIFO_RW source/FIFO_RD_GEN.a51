; FIFO_Read.a51 generated from FIFO_RW.c
;
; void FIFO_Read_generic( BYTE fifo_adr, BYTE n, BYTE * ptr );

$NOMOD51

$include (c8051f320.inc)	; Include register definition file.

; memory space code
MEM_IDATA	EQU		000h
MEM_XDATA	EQU		001h
MEM_PDATA	EQU		0FEh
MEM_CODE	EQU		0FFh

?PR?_FIFO_Read_generic?FIFO_RD_GEN		SEGMENT CODE
	PUBLIC	_FIFO_Read_generic
	RSEG	?PR?_FIFO_Read_generic?FIFO_RD_GEN
_FIFO_Read_generic:

;---- Variable 'fifo_adr' assigned to Register 'R7'
;---- Variable 'n' assigned to Register 'R5'
;---- Variable 'ptr' assigned to Register R2:R3 -> 'DPTR'

	MOV		A,R5				; if (n == 0)
	JZ		rdFx_ret			;    return;

	MOV		A,R3				; R3 = memory type
type_idata:						; R1 = ptr;
	CJNE	A,#MEM_IDATA,type_pdata
	MOV		A,R7				; fifo_adr
	DJNZ	R5,rdFi_multi		; if ( --n == 0 )
	ORL  	A,#080H				; USB0ADR = fifo_adr | 0x80;
	MOV		USB0ADR,A			; Set FIFO address and initiate first read
	SJMP	rdFi_last

rdFi_multi:						; USB0ADR = fifo_adr | 0xC0;
	ORL		A,#0C0H				; Set auto-read and initiate first read
	MOV		USB0ADR,A			; set FIFO address
rdFi_loop:
	MOV		A,USB0ADR			; Wait for BUSY->'0' (data ready)
	JB		ACC.7,rdFi_loop
	MOV		@R1,USB0DAT			; *ptr++ = USB0DAT;
	INC		R1
	DJNZ	R5,rdFi_loop		; loop n-1 times

rdFi_last:
	MOV		A,USB0ADR			; Wait for data ready
	JB		ACC.7,rdFi_last
	CLR		A					; clear auto-read flag
	MOV		USB0ADR,A
	MOV		@R1,USB0DAT			; *ptr = USB0DAT;
rdFi_ret:
	RET

type_pdata:						; R1 = ptr;
	CJNE	A,#MEM_PDATA,type_xdata
	MOV		A,R7				; fifo_adr
	DJNZ	R5,rdFp_multi		; if ( --n == 0 )
	ORL		A,#080H				; USB0ADR = fifo_adr | 0x80;
	MOV		USB0ADR,A			; Set FIFO address and initiate first read
	SJMP	rdFp_last

rdFp_multi:						; USB0ADR = fifo_adr | 0xC0;
	ORL		A,#0C0H				; Set auto-read and initiate first read
	MOV		USB0ADR,A			; set FIFO address
rdFp_loop:
	MOV		A,USB0ADR			; Wait for BUSY->'0' (data ready)
	JB		ACC.7,rdFp_loop
	MOV		A,USB0DAT			; *ptr++ = USB0DAT;
	MOVX	@R1,A
	INC		R1
	DJNZ	R5,rdFp_loop		; loop n-1 times

rdFp_last:
	MOV		A,USB0ADR			; Wait for data ready
	JB		ACC.7,rdFp_last
	CLR		A					; clear auto-read flag
	MOV		USB0ADR,A
	MOV		A,USB0DAT			; *ptr = USB0DAT;
	MOVX	@R1,A
rdFp_ret:
	RET


type_xdata:
	CJNE	A,#MEM_XDATA,rdFx_ret
	MOV		DPL,R1				; DPTR = R2:R1;
	MOV		DPH,R2
	MOV		A,R7				; fifo_adr
	DJNZ	R5,rdFx_multi		; if ( --n == 0 )
	ORL		A,#080H				; USB0ADR = fifo_adr | 0x80;
	MOV		USB0ADR,A			; Set FIFO address and initiate first read
	SJMP	rdFx_last

rdFx_multi:						; USB0ADR = fifo_adr | 0xC0;
	ORL		A,#0C0H				; Set auto-read and initiate first read
	MOV		USB0ADR,A			; set FIFO address
rdFx_loop:
	MOV		A,USB0ADR			; Wait for BUSY->'0' (data ready)
	JB		ACC.7,rdFx_loop
	MOV		A,USB0DAT			; *ptr++ = USB0DAT;
	MOVX	@DPTR,A
	INC		DPTR
	DJNZ	R5,rdFx_loop		; loop n-1 times

rdFx_last:
	MOV		A,USB0ADR			; Wait for data ready
	JB		ACC.7,rdFx_last
	CLR		A					; clear auto-read flag
	MOV		USB0ADR,A
	MOV		A,USB0DAT			; *ptr = USB0DAT;
	MOVX	@DPTR,A
rdFx_ret:
	RET
; END OF _FIFO_Read_generic

END
