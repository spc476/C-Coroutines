
		bits	32
		global	coroutine_init32
		global	coroutine_yield32

;***************************************************************************
		section	.text

;===========================================================================
; The stack frame for this routine has been set up *AS IF* the following
; code had been executed upon entry:
;
;		enter	-8,0
;		mov	eax,[ebp + P_co]
;		mov	[ebp + L_co],eax
;		mov	eax,[ebp + P_fun]
;		mov	[ebp + L_fun],eax
;		call	some_function_somewhere
;
; The code assumes we've just called some_function_somewhere() to retrieve
; some sort of return code, and then is calling L_fun right afterwards.  The
; only difference is that in reality, the parameters, return address and
; previous EBP don't actually exist---just keep that in mind.
;
; After L_fun returns, we return it's final return code using
; coroutine_yield().  How we handle a coroutine_resume() after that is still
; up in the air.  For right now, simply return the passed in value back to
; coroutine_yield().  I could call abort() ...  I don't know ...
;---------------------------------------------------------------------------

%assign L_co		-4
%assign L_fun		-8
%assign C_param		-12

start_it_up:	push	eax
		push	dword [ebp + L_co]
		call	[ebp + L_fun]

do_it_again:	mov	[ebp + C_param],eax
		call	coroutine_yield32
		jmp	do_it_again

;===========================================================================

%assign P_stack		12
%assign	P_fun		8
%assign P_co		4

coroutine_init32:
		mov	edx,[esp + P_co]
		mov	eax,[esp + P_stack]	; stack to switch to

	;------------------------------------------------------------
	; Create the stack for resuming to start_it_up().  The stack
	; is set up as:
	;
	;       EAX ->
	;		+--------------------------+
	;		| L_co                     |
	;		+--------------------------+
	;		| L_fun                    |
	;		+--------------------------+
	;		| start_it_up              |
	;		+--------------------------+
	;     co EBP -> | EBP of start_it_up (EAX) |
	;		+--------------------------+
	;		| "saved" EBX (0)          |
	;		+--------------------------+
	;		| "saved" ESI (0)          |
	;		+--------------------------+
	;     co ESP -> | "saved" EDI (0)          |
	;		+--------------------------+
	;
	; The code in coroutine.resume() will pop the three registers off
	; the stack, then restore EBP/ESP and "return" to start_it_up.
	;------------------------------------------------------------

		lea	ecx,[eax - 28]
		mov	[ecx + 12],eax		; EBP of coroutine
		mov	[ecx + 24],edx		; L_co
		mov	eax,[esp + P_fun]	; L_fun
		mov	[ecx + 20],eax
		mov	dword [ecx + 16],start_it_up
		xor	eax,eax
		mov	[ecx + 8],eax		; "saved" EBX
		mov	[ecx + 4],eax		; "saved" ESI
		mov	[ecx + 0],eax		; "saved" EDI
		mov	[edx],ecx		
		ret

;===========================================================================

%assign	P_param		8 + 16
%assign	P_co		4 + 16

coroutine_yield32:
		push	ebp			; save callee saved registers
		push	ebx
		push	esi
		push	edi

		mov	eax,[esp + P_param]	; return parameter
		mov	edx,[esp + P_co]	; get stack to yield to
		xchg	esp,[edx]		; YIELD!

		pop	edi			; retore registers
		pop	esi
		pop	ebx
		pop	ebp
		ret

;***************************************************************************
