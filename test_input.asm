section .data
    prompt db "Enter an integer: ", 0
    
section .bss
    input resq 1
    
section .text
    global main
    
main:
    ; Print prompt
    mov rax, 1              ; syscall for write
    mov rdi, 1              ; file descriptor for stdout
    mov rsi, prompt         ; address of the prompt string
    mov rdx, 16             ; length of the prompt string
    syscall                 ; call the kernel
    
    ; Read integer input
    mov rax, 0              ; syscall for read
    mov rdi, 0              ; file descriptor for stdin
    mov rsi, input          ; address of the input buffer
    mov rdx, 8              ; maximum number of bytes to read
    syscall                 ; call the kernel
    
    ; Convert input string to integer
    mov rdi, input          ; address of the input buffer
    call atoi               ; call the atoi function to convert string to integer
    
    ; Store the integer in memory
    mov qword [rbp-160], rax
    
    ; Exit the program
    mov rax, 60             ; syscall for exit
    xor rdi, rdi            ; return code 0
    syscall                 ; call the kernel
    
; atoi function definition
; Converts a null-terminated string to an integer
; Input: RDI - address of the string
; Output: RAX - the integer value
atoi:
    push rbx
    xor rax, rax
    xor rbx, rbx
    
    .loop:
        mov bl, byte [rdi]
        cmp bl, 0
        je .done
        
        ; Check if the character is a digit
        cmp bl, '0'
        jl .error
        cmp bl, '9'
        jg .error
        
        ; Convert the digit to integer and add it to the result
        sub bl, '0'
        movzx rbx, bl
        imul rax, 10
        add rax, rbx
        
        inc rdi
        jmp .loop
    
    .error:
        ; If the string is not a valid integer, return 0
        xor rax, rax
        
    .done:
        pop rbx
        ret