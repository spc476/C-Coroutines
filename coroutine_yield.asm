
		bits	32
		global	coroutine_init
		global	coroutine_yield
		global	coroutine_self

co_base		equ	0
co_sp		equ	4
co_size		equ	8

;***************************************************************************
		section	.text

;===========================================================================

_param1		equ	16
_fun1		equ	12
_co1		equ	8

coroutine_init:
		enter	0,0
		mov	edx,[ebp + _co1]
		mov	eax,[edx + co_size]
		add	eax,[edx + co_base]
		mov	esp,eax
		mov	[edx + co_sp],esp
		push	dword [ebp + _param1]
		push	edx
		push	dword [ebp + _fun1]
		mov	eax,esp
		leave
		ret

;===========================================================================

_param2		equ	16
_cofrom2	equ	12
_coto2		equ	8

coroutine_yield:
		enter	0,0
		push	ebx
		push	esi
		push	edi

		mov	eax,[ebp + _param2]	; return parameter
		mov	edx,[ebp + _cofrom2]	; save ESP for current coroutine
		mov	[edx + co_sp],esp
		mov	edx,[ebp + _coto2]	; switch to new coroutine
		mov	esp,[edx + co_sp]

		pop	edi			; resume new coroutine
		pop	esi
		pop	ebx
		leave
		ret

;===========================================================================

_co3		equ	8

coroutine_self:
		enter	0,0
		mov	edx,[ebp + _co3]
		xor	eax,eax
		mov	[edx + co_base],eax
		mov	[edx + co_sp],eax
		mov	[edx + co_size],eax
		leave
		ret

;***************************************************************************

