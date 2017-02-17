
		bits	32
		global	coroutine_init
		global	coroutine_resume
		global	coroutine_yield
		extern	abort
		extern	syslog

;***************************************************************************

		struc	co
			.base:		resd	1
			.size:		resd	1
			.csp:		resd	1
			.cbp:		resd	1
			.ysp:		resd	1
			.ybp:		resd	1
		endstruc

;***************************************************************************

%macro	SYSLOG	1
	section	.rodata
%%text		db	%1,0
	section	.text
		pusha
		push	%%text
		push	7
		call	syslog
		add	esp,8
		popa
%endmacro

%macro	SYSLOG	2
	section	.rodata
%%text		db	%1,0
	section	.text
		pusha
		push	%2
		push	%%text
		push	7
		call	syslog
		add	esp,12
		popa
%endmacro

%macro	SYSLOG	3
	section	.rodata
%%text		db	%1,0
	section	.text
		pusha
		push	%3
		push	%2
		push	%%text
		push	7
		call	syslog
		add	esp,16
		popa
%endmacro

%macro	SYSLOG	4
	section	.rodata
%%text		db	%1,0
	section	.text
		pusha
		push	%4
		push	%3
		push	%2
		push	%%text
		push	7
		call	syslog
		add	esp,20
		popa
%endmacro
			
;***************************************************************************
		section	.text

;===========================================================================

%assign	P_fun		16
%assign	P_param		12
%assign P_co		8

coroutine_init:
		enter	0,0

		SYSLOG	"init: RET=%08X",dword [ebp + 4]
		mov	edx,[ebp + P_co]
		mov	eax,[edx + co.base]
		add	eax,[edx + co.size]
		mov	[edx + co.ysp],esp
		mov	[edx + co.ybp],ebp
		mov	[edx + co.csp],eax
	;	mov	[edx + co.cbp],eax

		mov	esp,eax
	;	mov	ebp,eax

		SYSLOG	"init: FROM=%08X TO=%08X",ebp,dword [edx + co.cbp]
		
		push	edx		; save pointer to _co
		push	dword [ebp + P_param]
		push	edx
		call	[ebp + P_fun]
		add	esp,8
		pop	edx

		SYSLOG	"init: post-co=%08X",esp

		mov	esp,[edx + co.ysp]
		mov	ebp,[edx + co.ybp]

		leave
		ret

;===========================================================================

%assign	P_param		12
%assign P_co		8

coroutine_resume:
		enter	0,0
		mov	edx,[ebp + P_co]

		SYSLOG	"resume: FROM=%08X TO=%08X",ebp,dword [edx + co.cbp]

		mov	eax,[ebp + P_param]
		mov	edx,[ebp + P_co]
		mov	[edx + co.ysp],esp
		mov	[edx + co.ybp],ebp
		mov	esp,[edx + co.csp]
		mov	ebp,[edx + co.cbp]

		leave
		ret

;===========================================================================

%assign	P_param		12
%assign	P_co		8

coroutine_yield:
		enter	0,0

		mov	edx,[ebp + P_co]	; return parameter

		SYSLOG	"yield: FROM=%08X TO=%08X",ebp,dword [edx + co.ybp]

		mov	eax,[ebp + P_param]
		mov	edx,[ebp + P_co]
		mov	[edx + co.csp],esp
		mov	[edx + co.cbp],ebp
		mov	esp,[edx + co.ysp]
		mov	ebp,[edx + co.ybp]

		SYSLOG	"yield: RET=%08X",dword [ebp + 4]
		leave
		ret

;***************************************************************************

