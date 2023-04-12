extern printf
extern scanf
%macro EMPTY_STACK 0
	pop rsi
	pop rdi
	pop rbp
	pop rsp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro align_16 0
	mov rbx, rsp
	and rsp, -16
%endmacro

%macro realign_16 0
	mov rsp, rbx
%endmacro

%macro FILL_STACK 0
	push rax
	push rbx
	push rcx
	push rdx
	push rsp
	push rbp
	push rdi
	push rsi
%endmacro

section .data
	input: db "%d", 0
	outputInt: db "Output: %d", 0
	outputReal: db "Output: %lf", 0
	outputTrue: db "Output: true", 0
	outputFalse: db "Output: false", 0
	inputInt: db "Input: Enter an integer value ", 0
	inputReal: db "Input: Enter a real value ", 0
	inputBool: db "Input: Enter a boolean value ", 0
	newline: db "", 10, 0
	OutOfBoundError: db "RUNTIME ERROR: Array index out of bounds", 10, 0
	TypeMismatchError: db "RUNTIME ERROR: Type Mismatch Error", 10, 0
	DivBy0Exception: db "EXCEPTION: Division by 0", 10, 0

	typeInteger: db "integer", 0
	typeReal: db "real", 0
	typeBoolean: db "boolean", 0
section .text
	global main
main:
	push rbp
	mov rbp, rsp
	; ASSIGNMENT OPERATION (NON-ARRAY)
	MOV rax, 5
	MOV QWORD[rbp-96], rax
	; ASSIGNMENT OPERATION (NON-ARRAY)
	MOV rax, 9
	MOV QWORD[rbp-128], rax
	MOV rdx, rbp
	; Getting an integer
	section .bss
		temp_integer__0: resq 1
	section .text
	; FILL_STACK
	MOV rdi, inputInt
	CALL printf
	MOV rdi, input
	MOV rsi, temp_integer__0
	xor rax, rax
	CALL scanf

	MOV rdi, outputInt
	mov rsi, qword [temp_integer__0]
	call printf

	MOV rdi, newline
	call printf


	MOV rax, [temp_integer__0]
	MOV QWORD[rbp-160], rax
	MOV rdi, outputInt
	mov rsi, qword [rbp-128]
	call printf

	MOV rdi, newline
	call printf

	MOV rdi, outputInt
	mov rsi, qword [rbp-160]
	call printf

	MOV rdi, newline
	call printf
	; EMPTY_STACK
	; FILL_STACK
	MOV rax, qword[rbp-128]
	MOV rbx, qword[rbp-96]
	; SUBTRACTION
	SUB rax, rbx
	MOV QWORD[rbp-208], rax
	; EMPTY_STACK
	; FILL_STACK
	MOV rax, qword[rbp-208]
	MOV rbx, qword[rbp-96]
	; MULTIPLICATION
	IMUL rax, rbx
	MOV QWORD[rbp-240], rax
	; EMPTY_STACK
	; ASSIGNMENT OPERATION (NON-ARRAY)
	MOV rax, qword[rbp-240]
	MOV qword[rbp-64], rax
	; Printing an integer ID
	MOV rdi, outputInt
	mov rsi, qword[rbp-64]
	call printf
	MOV rdi, newline
	call printf

	; Printing a real number
	section .data
	__label_0: dq 123200000.000000
	section .text
	MOV rdi, outputReal
	MOVSD xmm0, [__label_0]
	MOVQ rsi, xmm0
	call printf
	MOV rdi, newline
	call printf
	; Printing a boolean
	MOV rax, 1
	CMP rax, 0
	JE boolIsFalse__0
	MOV rdi, outputTrue
	jmp continue_post_bool__0
boolIsFalse__0:
	MOV rdi, outputFalse
continue_post_bool__0:
	call printf
	MOV rdi, newline
	call printf
	; ASSIGNMENT OPERATION (NON-ARRAY)
	MOV rax, 0
	MOV qword[rbp-192], rax
	MOV rcx, 0
	MOV rdx, 1
	MOV rax, qword[rbp-192]
	MOV rbx, 0
	; LOGICAL AND
	ADD rax, rbx
	CMP rax, 2
	CMOVE rcx, rdx
	MOV qword[rbp-272], rcx
	MOV rcx, 0
	MOV rdx, 1
	MOV rax, qword[rbp-192]
	MOV rbx, 1
	; LOGICAL OR
	SUB rax, rbx
	CMP rax, 0
	CMOVNE rcx, rdx
	MOV qword[rbp-288], rcx
	MOV rcx, 0
	MOV rdx, 1
	MOV rax, qword[rbp-272]
	MOV rbx, qword[rbp-288]
	; LOGICAL AND
	ADD rax, rbx
	CMP rax, 2
	CMOVE rcx, rdx
	MOV qword[rbp-304], rcx
	; ASSIGNMENT OPERATION (NON-ARRAY)
	MOV rax, qword[rbp-304]
	MOV qword[rbp-192], rax
	; Printing a boolean ID
	MOV rax, qword[rbp-192]
	CMP rax, 0
	JE boolVarIsFalse__1
	MOV rdi, outputTrue
	jmp continue_post_bool__1
boolVarIsFalse__1:
	MOV rdi, outputFalse
continue_post_bool__1:
	call printf
	MOV rdi, newline
	call printf
	; Printing an integer ID
	MOV rdi, outputInt
	mov rsi, qword[rbp-160]
	call printf
	MOV rdi, newline
	call printf
	pop rbp
	mov rax, 0
	ret
