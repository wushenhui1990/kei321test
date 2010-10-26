; FIFO_RW.a51 generated from FIFO_RW.c
;
; void FIFO_Write_generic( BYTE fifo_adr, BYTE n, BYTE * ptr );

$NOMOD51

$include (c8051f320.inc)	; Include register definition file.

; memory space code
MEM_IDATA	EQU		000h
MEM_XDATA	EQU		001h
MEM_PDATA	EQU		0FEh
MEM_CODE	EQU		0FFh

?PR?_FIFO_Write_generic?FIFO_WT_GEN	SEGMENT CODE
	PUBLIC	_FIFO_Write_generic
	RSEG	?PR?_FIFO_Write_generic?FIFO_WT_GEN
_FIFO_Write_generic:

;---- Variable fifo_adr assigned to Register R7
;---- Variable n assigned to Register R5
;---- Variable ptr assigned to Register R1:R2:R3 -> DPTR or R1

	MOV		A,R5				; if (n == 0)
	JZ		wtFx_ret			;    return;

	MOV		A,R7				; Set address (mask out bits7-6)
	ANL		A,#03FH
	MOV		USB0ADR,A

	MOV		A,R3				; R3 = memory type
type_idata:						; R1 = ptr;
	CJNE	A,#MEM_IDATA,type_pdata
wtFi_loop1:
	MOV		USB0DAT,@R1			; USB0DAT = *ptr++
	INC		R1
wtFi_loop2:
	MOV		A,USB0ADR			; Wait for BUSY->'0' (data ready)
	JB		ACC.7,wtFi_loop2
	DJNZ	R5,wtFi_loop1		; loop n times
	RET

type_pdata:						; R1 = ptr;
	CJNE	A,#MEM_PDATA,type_xdata
wtFp_loop1:
	MOVX	A,@R1				; USB0DAT = *ptr++
	MOV		USB0DAT,A
	INC		R1
wtFp_loop2:
	MOV		A,USB0ADR			; Wait for BUSY->'0' (data ready)
	JB		ACC.7,wtFp_loop2
	DJNZ	R5,wtFp_loop1		; loop n times
	RET

type_xdata:
	MOV		DPL,R1				; DPTR = R2:R1;
	MOV		DPH,R2
	CJNE	A,#MEM_XDATA,type_code
wtFx_loop1:
	MOVX	A,@DPTR				; USB0DAT = *ptr++
	MOV		USB0DAT,A
	INC		DPTR
wtFx_loop2:
	MOV		A,USB0ADR			; Wait for BUSY->'0' (data ready)
	JB		ACC.7,wtFx_loop2
	DJNZ	R5,wtFx_loop1		; loop n times
	RET

type_code:
	CJNE	A,#MEM_CODE,wtFx_ret
;	MOV		DPL,R1				; DPTR = R2:R1;
;	MOV		DPH,R2
wtFc_loop1:
	CLR		A
	MOVC	A,@A+DPTR			; USB0DAT = *ptr++
	MOV		USB0DAT,A
	INC		DPTR
wtFc_loop2:
	MOV		A,USB0ADR			; Wait for BUSY->'0' (data ready)
	JB		ACC.7,wtFc_loop2
	DJNZ	R5,wtFc_loop1		; loop n times

wtFx_ret:
	RET
; END OF _FIFO_Write_generic

	END
