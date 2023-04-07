; nasm -f lf64 arithmetic64bit.asm -o test.o
; gcc -no-pie test.o -o testing
; ./testing

extern printf		; the C function to be called

%macro	pabc 1			; a "simple" print macro
	section	.data
.str	db	%1,0		; %1 is macro call first actual parameter
	section .text
				; push onto stack backwards 
        mov	rdi, fmt	; address of format string
	mov	rsi, .str	; string passed to macro
	movq	xmm0, qword [a]	; first floating point in fmt
	movq	xmm1, qword [b]	; second floating point
	movq	xmm2, qword [c]	; third floating point
	mov	rax, 3		; 3 floating point arguments to printf
        call    printf          ; Call C function
%endmacro
	
	section	.data  		; preset constants, writable
a:	dq	3.0		; 64-bit variable a initialized to 3.0
b:	dq	4.0		; 64-bit variable b initializes to 4.0
i:	dq	8		; a 64 bit integer
five:	dq	5.0		; constant 5.0
fmt:    db "%s, a=%e, b=%e, c=%e",10,0	; format string for printf
	
	section .bss 		; uninitialized space
c:	resq	1		; reserve a 64-bit word

	section .text		; instructions, code segment
	global	main		; for gcc standard linking
main:				; label

	push	rbp		; set up stack
lit5:				; c=5.0;
	fld	qword [five]	; 5.0 constant
	fstp	qword [c]	; store into c
	pabc	"c=5.0"		; invoke the print macro
	
addb:				; c=a+b;
	fld	qword [a] 	; load a (pushed on flt pt stack, st0)
	fadd	qword [b]	; floating add b (to st0)
	fstp	qword [c]	; store into c (pop flt pt stack)
	pabc	"c=a+b"		; invoke the print macro
	
subb:				; c=a-b;
	fld	qword [a] 	; load a (pushed on flt pt stack, st0)
	fsub	qword [b]	; floating subtract b (to st0)
	fstp	qword [c]	; store into c (pop flt pt stack)
	pabc	"c=a-b"		; invoke the print macro
	
mulb:				; c=a*b;
	fld	qword [a]	; load a (pushed on flt pt stack, st0)
	fmul	qword [b]	; floating multiply by b (to st0)
	fstp	qword [c]	; store product into c (pop flt pt stack)
	pabc	"c=a*b"		; invoke the print macro
	
diva:				; c=c/a;
	fld	qword [c] 	; load c (pushed on flt pt stack, st0)
	fdiv	qword [a]	; floating divide by a (to st0)
	fstp	qword [c]	; store quotient into c (pop flt pt stack)
	pabc	"c=c/a"		; invoke the print macro

intflt:				; a=i;
	fild	dword [i]	; load integer as floating point
	fst	qword [a]	; store the floating point (no pop)
	fadd	st0		; b=a+i; 'a' as 'i'  already on flt stack
	fst	qword [b]	; store sum (no pop) 'b' still on stack
	fistp	dword [i]	; i=b; store floating point as integer
	fild	dword [i]	; c=i; load again from ram (redundant)
	fstp	qword [c]
	pabc	"a=i  "		; invoke the print macro

cmpflt:	fld	dword [b]	; into st0, then pushed to st1
	fld	dword [a]	; in st0
	fcomip	st0,st1		; a compare b, pop a
	jg	cmpfl2
	pabc	"a<=b "
	jmp	cmpfl3
cmpfl2:	
	pabc	"a>b  "
cmpfl3:
	fld	dword [c]	; should equal [b]
	fcomip  st0,st1
	jne	cmpfl4
	pabc	"b==c "
	jmp	cmpfl5
cmpfl4:
	pabc	"b!=c "
cmpfl5:

	pop	rbp		; pop stack
        mov     rax,0           ; exit code, 0=normal
	ret			; main returns to operating system