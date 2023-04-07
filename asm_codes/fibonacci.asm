; Now an implementation closer to C, storing variables

; fib_64m.asm  using 64 bit memory more like C code
; // fib.c  same as computation as fib_64m.asm
; #include 
; int main(int argc, char *argv[])
; {
;   long int c = 95;  // loop counter
;   long int a = 1;   // current number, becomes next
;   long int b = 2;   // next number, becomes sum a+b
;   long int d;       // temp
;   printf("fibinachi numbers\n");
;   for(c=c; c!=0; c--)
;   {
;     printf("%21ld\n",a);
;     d = a;
;     a = b;
;     b = d+b;
;   }
; }
	global main
	extern printf

	section .bss
d:	resq	1		; temp  unused, kept in register rdx
	
	section .data
c:	dq	95		; loop counter
a:	dq	1		; current number, becomes next
b:	dq	2		; next number, becomes sum a+b


format:	db '%15ld', 10, 0
title:	db 'fibinachi numbers', 10, 0
	
	section .text
main:
	push rbp 		; set up stack
	mov rdi, title 		; arg 1 is a pointer
	mov rax, 0 		; no vector registers in use
	call printf

print:
	;  We need to call printf, but we are using rax, rbx, and rcx.
	mov rdi, format 	; arg 1 is a pointer
	mov rsi,[a] 		; arg 2 is the current number
	mov rax, 0 		; no vector registers in use
	call printf

	mov rdx,[a] 		; save the current number, in register
	mov rbx,[b] 		;
	mov [a],rbx		; next number is now current, in ram
	add rbx, rdx 		; get the new next number
	mov [b],rbx		; store in ram
	mov rcx,[c]		; get loop count
	dec rcx 		; count down
	mov [c],rcx		; save in ram
	jnz print 		; if not done counting, do some more

	pop rbp 		; restore stack
	mov rax, 0		; normal exit
	ret			; return to operating system
