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
	outputInt: db "Output: %d", 10, 0
	outputReal: db "Output: %lf", 10, 0
	outputTrue: db "Output: true", 10, 0
	outputFalse: db "Output: false", 10, 0
	inputInt: db "Input: Enter an integer value ", 10, 0
	inputReal: db "Input: Enter a real value ", 10, 0
	inputBool: db "Input: Enter a boolean value ", 10, 0
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
	sub rsp, 928
	; Getting an integer
	section .bss
		temp_integer__0 resq 1
	section .text
	MOV rdi, inputInt
	xor rax, rax
	CALL printf
	LEA rsi, [temp_integer__0]
	MOV rdi, input
	MOV rax, 0
	CALL scanf
	MOV rax, qword [temp_integer__0]
	MOV qword[rbp-192], rax
	; Getting an integer
	section .bss
		temp_integer__1 resq 1
	section .text
	MOV rdi, inputInt
	xor rax, rax
	CALL printf
	LEA rsi, [temp_integer__1]
	MOV rdi, input
	MOV rax, 0
	CALL scanf
	MOV rax, qword [temp_integer__1]
	MOV qword[rbp-160], rax
	MOV rax, qword[rbp-0]
	MOV rbx, qword[rbp-32]
	; MULTIPLICATION
	IMUL rax, rbx
	MOV QWORD[rbp-240], rax
	MOV rax, qword[rbp-192]
	MOV rbx, 2
	; MULTIPLICATION
	IMUL rax, rbx
	MOV QWORD[rbp-272], rax
	MOV rax, qword[rbp-240]
	MOV rbx, qword[rbp-272]
	; ADDITION
	ADD rax, rbx
	MOV QWORD[rbp-304], rax
	MOV rax, qword[rbp-304]
	MOV rbx, 3
	; SUBTRACTION
	SUB rax, rbx
	MOV QWORD[rbp-336], rax
	; ASSIGNMENT OPERATION (NON-ARRAY)
	MOV rax, qword[rbp-336]
	MOV QWORD[rbp-128], rax
	MOV rcx, 0
	MOV rdx, 1
	MOV rax, qword[rbp-0]
	MOV rbx, qword[rbp-32]
	; LESS THAN OR EQUAL TO
	CMP rax, rbx
	CMOVLE rcx, rdx
	MOV qword[rbp-368], rcx
	MOV rcx, 0
	MOV rdx, 1
	MOV rax, qword[rbp-64]
	MOV rbx, qword[rbp-368]
	; LOGICAL AND
	AND rax, rbx
	MOV qword[rbp-384], rax
	; ASSIGNMENT OPERATION (NON-ARRAY)
	MOV rax, qword[rbp-384]
	MOV qword[rbp-224], rax
	MOV rax, qword[rbp-128]
	MOV rbx, qword[rbp-160]
	; ADDITION
	ADD rax, rbx
	MOV QWORD[rbp-400], rax
	; ASSIGNMENT OPERATION (NON-ARRAY)
	MOV rax, qword[rbp-400]
	MOV QWORD[rbp-80], rax
	MOV rcx, 0
	MOV rdx, 1
	MOV rax, qword[rbp-128]
	MOV rbx, qword[rbp-160]
	; LESS THAN OR EQUAL TO
	CMP rax, rbx
	CMOVLE rcx, rdx
	MOV qword[rbp-432], rcx
	MOV rcx, 0
	MOV rdx, 1
	MOV rax, qword[rbp-224]
	MOV rbx, qword[rbp-432]
	; LOGICAL OR
	OR rax, rbx
	MOV qword[rbp-448], rax
	; ASSIGNMENT OPERATION (NON-ARRAY)
	MOV rax, qword[rbp-448]
	MOV qword[rbp-112], rax
	; Printing an integer ID
	MOV rax, qword[rbp-0]
	MOV rsi, rax
	MOV rdi, outputInt
	mov rax, 0
	call printf
	; Printing an integer ID
	MOV rax, qword[rbp-32]
	MOV rsi, rax
	MOV rdi, outputInt
	mov rax, 0
	call printf
	; Printing a boolean ID
	MOV rax, qword[rbp-64]
	CMP rax, 0
	JE boolVarIsFalse__0
	MOV rdi, outputTrue
	jmp continue_post_bool__0
boolVarIsFalse__0:
	MOV rdi, outputFalse
continue_post_bool__0:
	call printf
	; Printing an integer ID
	MOV rax, qword[rbp-192]
	MOV rsi, rax
	MOV rdi, outputInt
	mov rax, 0
	call printf
	; Printing an integer ID
	MOV rax, qword[rbp-128]
	MOV rsi, rax
	MOV rdi, outputInt
	mov rax, 0
	call printf
	; Printing an integer ID
	MOV rax, qword[rbp-160]
	MOV rsi, rax
	MOV rdi, outputInt
	mov rax, 0
	call printf
	; Printing an integer ID
	MOV rax, qword[rbp-80]
	MOV rsi, rax
	MOV rdi, outputInt
	mov rax, 0
	call printf
	; Printing a boolean ID
	MOV rax, qword[rbp-112]
	CMP rax, 0
	JE boolVarIsFalse__1
	MOV rdi, outputTrue
	jmp continue_post_bool__1
boolVarIsFalse__1:
	MOV rdi, outputFalse
continue_post_bool__1:
	call printf
	; Getting an integer
	section .bss
		temp_integer__2 resq 1
	section .text
	MOV rdi, inputInt
	xor rax, rax
	CALL printf
	LEA rsi, [temp_integer__2]
	MOV rdi, input
	MOV rax, 0
	CALL scanf
	MOV rax, qword [temp_integer__2]
	MOV qword[rbp-0], rax
	; Getting an integer
	section .bss
		temp_integer__3 resq 1
	section .text
	MOV rdi, inputInt
	xor rax, rax
	CALL printf
	LEA rsi, [temp_integer__3]
	MOV rdi, input
	MOV rax, 0
	CALL scanf
	MOV rax, qword [temp_integer__3]
	MOV qword[rbp-32], rax
	; ASSIGNMENT OPERATION (NON-ARRAY)
	MOV rax, 0
	MOV qword[rbp-112], rax
	; Printing an integer ID
	MOV rax, qword[rbp-0]
	MOV rsi, rax
	MOV rdi, outputInt
	mov rax, 0
	call printf
	; Printing an integer ID
	MOV rax, qword[rbp-32]
	MOV rsi, rax
	MOV rdi, outputInt
	mov rax, 0
	call printf
	; Printing an integer ID
	MOV rax, qword[rbp-64]
	MOV rsi, rax
	MOV rdi, outputInt
	mov rax, 0
	call printf
	; Printing a boolean ID
	MOV rax, qword[rbp-96]
	CMP rax, 0
	JE boolVarIsFalse__2
	MOV rdi, outputTrue
	jmp continue_post_bool__2
boolVarIsFalse__2:
	MOV rdi, outputFalse
continue_post_bool__2:
	call printf
exit:
	mov rsp, rbp
	pop rbp
	mov rax, 0
	ret
