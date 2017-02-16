
		bits	32
		global	swapstack
		global	getstack

;****************************************************************************
		section	.text

swapstack:	pop	ecx		; get return address
		pop	edx		; get parameter
		mov	eax,esp		; current ESP as return value
		mov	esp,edx		; set new stack
		push	edx		; set up stack frame
		push	ecx
		ret

;*****************************************************************************

getstack:	lea	eax,[esp + 4]
		ret

;*****************************************************************************
