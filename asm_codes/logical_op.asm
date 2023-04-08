extern printf		; the C function to be called

%macro	pabc 1			; a "simple" print macro
	section .data
.str	db	%1,0		; %1 is first actual in macro call
	section .text
        mov	rdi, fmt        ; address of format string
	mov	rsi, .str 	; users string
	mov	rdx, [a]	; long int a
	mov	rcx, [b]	; long int b 
	mov	r8, [c]		; long int c
	mov     rax, 0	        ; no xmm used
        call    printf          ; Call C function
%endmacro
	
	section .data  		; preset constants, writable
a:	dq	3		; 64-bit variable a initialized to 3
b:	dq	5		; 64-bit variable b initializes to 4
fmt:    db "%s, a=%ld, b=%ld, c=%ld",10,0 ; format string for printf
	
	section .bss 		; unitialized space
c:	resq	1		; reserve a 64-bit word

	section .text		; instructions, code segment
	global	 main		; for gcc standard linking
main:				; label
	push	rbp		; set up stack
	
lit5:				; c=5;
	mov	rax,15	 	; 5 is a literal constant
	mov	[c],rax		; store into c
	pabc	"c=5  "		; invoke the print macro
	
andb:				; c=a&b;
	mov	rax,[a]	 	; load a
	and	rax,[b]		; and with b
	mov	[c],rax		; store into c
	pabc	"c=a&b"		; invoke the print macro
	
orw:				; c=a-b;
	mov	rax,[a]	 	; load a
	or	rax,[b]		; logical or with b
	mov	[c],rax		; store into c
	pabc	"c=a|b"		; invoke the print macro
	
xorw:				; c=a^b;
	mov	rax,[a]	 	; load a
	xor	rax,[b] 	; exclusive or with b
	mov	[c],rax		; store result in c
	pabc	"c=a^b"		; invoke the print macro
	
notw:				; c=~a;
	mov	rax,[a]	 	; load c
	not	rax	 	; not, complement
	mov	[c],rax		; store result into c
	pabc	"c=~a "		; invoke the print macro

	pop	rbp		; restore stack
	mov     rax,0           ; exit code, 0=normal
	ret			; main returns to operating system