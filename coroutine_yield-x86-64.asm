;***************************************************************************
;
; Copyright 2017 by Sean Conner.
;
; This library is free software; you can redistribute it and/or modify it
; under the terms of the GNU Lesser General Public License as published by
; the Free Software Foundation; either version 3 of the License, or (at your
; option) any later version.
;
; This library is distributed in the hope that it will be useful, but
; WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
; or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
; License for more details.
;
; You should have received a copy of the GNU Lesser General Public License
; along with this library; if not, see <http://www.gnu.org/licenses/>.
;
; Comments, questions and criticisms can be sent to: sean@conman.org
;
;***************************************************************************

		bits	64
		global	coroutine_init64
		global	coroutine_yield64

;***************************************************************************
		section	.text

;===========================================================================

%assign		L_co	-8
%assign		L_fun	-16

start_it_up:	mov	rdi,rax
		mov	rsi,[rbp + L_co]
		call	[rbp + L_fun]

do_it_again:	mov	rdi,rax
		mov	rsi,[rbp + L_co]
		call	coroutine_yield64
		jmp	do_it_again

;===========================================================================
;
;	RDI -> co
;	RSI -> fun
;	RDX -> stack
;	RAX -> 0
;	RCX -> adjusted stack
;
;	L_co			64
;	L_run			56
;	start_it_up		48
;	rbp			40
;	rbx			32
;	r12			24
;	r13			16
;	r14			8
;	r15			0
;

coroutine_init64:
		xor	rax,rax
		lea	rcx,[rdx - 9*8]
		
		mov	[rcx + 64],rdi	; L_co
		mov	[rcx + 56],rsi	; L_fun
		mov	qword [rcx + 48],start_it_up
		mov	[rcx + 40],rdx	; "saved" rbp
		mov	[rcx + 32],rax	; "saved" rbx
		mov	[rcx + 24],rax	; "saved" r12
		mov	[rcx + 16],rax	; "saved" r13
		mov	[rcx + 8],rax	; "saved" r14
		mov	[rcx],rax	; "saved" r15
		ret

;===========================================================================

coroutine_yield64:
		push	rbp
		push	rbx
		push	r12
		push	r13
		push	r14
		push	r15

		mov	rax,rsi		; value to return
		xchg	rsp,[rdi]	; YIELD!

		pop	r15
		pop	r14
		pop	r13
		pop	r12
		pop	rbx
		pop	rbp
		ret

;***************************************************************************
