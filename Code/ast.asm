extern printf
section .data
		fmt1: db "RAX = %ld, RBX = %ld, RCX = %ld, RDX = %ld", 10, 0
		fmt2: db "RSP = %ld, RBP = %ld, RSI = %ld, RDI = %ld", 10, 0
section .text
				%macro push_all 0 
                    push RAX 
                    push RBX 
                    push RCX 
                    push RDX 
                    push RSP 
                    push RBP 
                    push RSI 
                    push RDI 
                %endmacro 

				%macro pop_all 0 
                    pop RDI 
                    pop RSI 
                    pop RBP 
                    pop RSP 
                    pop RDX 
                    pop RCX 
                    pop RBX 
                    pop RAX 
                %endmacro 

				%macro print_first_four 0 
                    push_all
                    push RDX 
                    push RCX 
                    push RBX 
                    push RAX 
                    mov RDI, fmt1		; first arg, format 
                    pop RSI 
                    pop RDX 
                    pop RCX 
                    pop R8 
                    mov RAX, 0 
                    call printf 
                    pop_all
                %endmacro 

				%macro print_last_four 0 
                    push_all
                    push RDI 
                    push RSI 
                    push RBP 
                    push RSP 
                    mov RDI, fmt2		; first arg, format 
                    pop RSI 
                    pop RDX 
                    pop RCX 
                    pop R8 
                    mov RAX, 0 
                    call printf 
                    pop_all
                %endmacro 

				global main
main:
				PUSH RBP
				MOV RBP, RSP
				SUB RSP, 30
				print_first_four
				print_last_four
	label_0:
	label_1:
	label_2:
	main_end:
				ADD RSP, 30
				ret

