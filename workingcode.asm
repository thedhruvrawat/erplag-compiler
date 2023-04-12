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
	sub rsp, 800
	; ASSIGNMENT OPERATION (NON-ARRAY)
	MOV rax, 1
	MOV qword[rbp-192], rax
	; ASSIGNMENT OPERATION (NON-ARRAY)
	MOV rax, 5
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
	MOV rax, qword [temp_integer__0]
	MOV qword[rbp-0], rax
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
	MOV qword[rbp-32], rax
	MOV rax, qword[rbp-32]
	MOV rbx, qword[rbp-128]
	; MULTIPLICATION
	IMUL rax, rbx
	MOV QWORD[rbp-224], rax
	MOV rax, qword[rbp-0]
	MOV rbx, qword[rbp-224]
	; ADDITION
	ADD rax, rbx
	MOV QWORD[rbp-256], rax
	MOV rax, qword[rbp-96]
	MOV rbx, qword[rbp-128]
	; SUBTRACTION
	SUB rax, rbx
	MOV QWORD[rbp-288], rax
	MOV rax, qword[rbp-288]
	MOV rbx, qword[rbp-32]
	; MULTIPLICATION
	IMUL rax, rbx
	MOV QWORD[rbp-320], rax
	MOV rax, qword[rbp-256]
	MOV rbx, qword[rbp-320]
	; ADDITION
	ADD rax, rbx
	MOV QWORD[rbp-352], rax
	MOV rax, qword[rbp-96]
	MOV rbx, 2
	; MULTIPLICATION
	IMUL rax, rbx
	MOV QWORD[rbp-384], rax
	MOV rax, qword[rbp-352]
	MOV rbx, qword[rbp-384]
	; ADDITION
	ADD rax, rbx
	MOV QWORD[rbp-416], rax
	MOV rax, qword[rbp-128]
	MOV rbx, qword[rbp-0]
	; MULTIPLICATION
	IMUL rax, rbx
	MOV QWORD[rbp-448], rax
	MOV rax, qword[rbp-416]
	MOV rbx, qword[rbp-448]
	; SUBTRACTION
	SUB rax, rbx
	MOV QWORD[rbp-480], rax
	; ASSIGNMENT OPERATION (NON-ARRAY)
	MOV rax, qword[rbp-480]
	MOV QWORD[rbp-64], rax
	MOV rcx, 0
	MOV rdx, 1
	MOV rax, qword[rbp-64]
	MOV rbx, 10
	; GREATER THAN
	CMP rax, rbx
	CMOVG rcx, rdx
	MOV qword[rbp-512], rcx
	MOV rcx, 0
	MOV rdx, 1
	MOV rax, qword[rbp-96]
	MOV rbx, qword[rbp-128]
	; LESS THAN OR EQUAL TO
	CMP rax, rbx
	CMOVLE rcx, rdx
	MOV qword[rbp-528], rcx
	MOV rcx, 0
	MOV rdx, 1
	MOV rax, qword[rbp-512]
	MOV rbx, qword[rbp-528]
	; LOGICAL OR
	OR rax, rbx
	MOV qword[rbp-544], rax
	MOV rcx, 0
	MOV rdx, 1
	MOV rax, qword[rbp-0]
	MOV rbx, qword[rbp-32]
	; LESS THAN
	CMP rax, rbx
	CMOVL rcx, rdx
	MOV qword[rbp-560], rcx
	MOV rcx, 0
	MOV rdx, 1
	MOV rax, qword[rbp-544]
	MOV rbx, qword[rbp-560]
	; LOGICAL AND
	AND rax, rbx
	MOV qword[rbp-576], rax
	MOV rcx, 0
	MOV rdx, 1
	MOV rax, qword[rbp-576]
	MOV rbx, qword[rbp-192]
	; LOGICAL AND
	AND rax, rbx
	MOV qword[rbp-592], rax
	; ASSIGNMENT OPERATION (NON-ARRAY)
	MOV rax, qword[rbp-592]
	MOV qword[rbp-208], rax
	; Printing an integer ID
	MOV rax, qword[rbp-64]
	MOV rsi, rax
	MOV rdi, outputInt
	mov rax, 0
	call printf
	; Printing a boolean ID
	MOV rax, qword[rbp-208]
	CMP rax, 0
	JE boolVarIsFalse__0
	MOV rdi, outputTrue
	jmp continue_post_bool__0
boolVarIsFalse__0:
	MOV rdi, outputFalse
continue_post_bool__0:
	call printf
exit:
	mov rsp, rbp
	pop rbp
	mov rax, 0
	ret
