extern printf
extern scanf

section .data
	input: db "%d", 0
	outputInt: db "Output: %d", 10, 0
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
	sub rsp, 16	
	; ASSIGNMENT OPERATION (NON-ARRAY)
	MOV rax, 15
	MOV QWORD[rbp-96], rax	
	; ASSIGNMENT OPERATION (NON-ARRAY)
	MOV rax, 9
	MOV QWORD[rbp-128], rax	
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
	MOV rax, [temp_integer__0]
	MOV qword[rbp-160], rax	; mov rsi, rax	; mov rdi, outputInt	; mov rax, 0	; call printf
	
    
    MOV rax, qword[rbp-128]
	MOV rbx, qword[rbp-96]
	; SUBTRACTION
	SUB rax, rbx
	MOV QWORD[rbp-192], rax	
	MOV rax, qword[rbp-192]
	MOV rbx, qword[rbp-96]
	; MULTIPLICATION
	IMUL rax, rbx
	MOV QWORD[rbp-224], rax	
	; ASSIGNMENT OPERATION (NON-ARRAY)
	MOV rax, qword[rbp-224]
	MOV qword[rbp-64], rax
	
	MOV rax, qword[rbp-160]
	mov rsi, rax
	MOV rdi, outputInt
	mov rax, 0
	call printf
    
	mov rax, qword[rbp-64]
	mov rsi, rax
	MOV rdi, outputInt
	xor rax, rax
	call printf
	mov rsp, rbp
	pop rbp
	mov rax, 0
	ret
