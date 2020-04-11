extern printf, scanf
section .data
		ffour_reg_fmt: db `RAX = %ld, RBX = %ld, RCX = %ld, RDX = %ld`, 10, 0
		lfour_reg_fmt: db `RSP = %ld, RBP = %ld, RSI = %ld, RDI = %ld\n`, 10, 0
		int_fmt: db "%d", 0
		real_fmt: db "%lf", 0
		bool_fmt: db "%s", 0
		str_fmt: db "%s", 0
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
                    mov RDI, ffour_reg_fmt		; first arg, format 
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
                    mov RDI, lfour_reg_fmt		; first arg, format 
                    pop RSI 
                    add RSI, 88 ; 88 because RSP has pushed additional 11 registers, so original value is 88 more than current
                    pop RDX 
                    pop RCX 
                    pop R8 
                    mov RAX, 0 
                    call printf 
                    pop_all
                %endmacro 

				global main
main:
				SUB RSP, 8     ; align RSP to 16 boundary to enable calls to scanf, etc
				ENTER 24, 0
				print_first_four
				print_last_four
				push_all
				mov RDI, bool_fmt		;first arg, format "%s" 
				mov RDX, RBP
                sub RDX, 0     ; make RDX to point at location of variable on the stack
                mov RSI, RDX 
                mov RAX, 0 
                call scanf 
                pop_all
label_0:
				push_all
				mov RDI, bool_fmt		;first arg, format "%s" 
				mov RDX, RBP
                sub RDX, 0     ; make RDX to point at location of variable on the stack
                mov RSI, RDX 
                mov RAX, 0 
                call printf 
                pop_all
label_1:
				push_all
				mov RDI, bool_fmt		;first arg, format "%s" 
				mov RDX, RBP
                sub RDX, 8     ; make RDX to point at location of variable on the stack
                mov RSI, RDX 
                mov RAX, 0 
                call scanf 
                pop_all
label_2:
				push_all
				mov RDI, bool_fmt		;first arg, format "%s" 
				mov RDX, RBP
                sub RDX, 8     ; make RDX to point at location of variable on the stack
                mov RSI, RDX 
                mov RAX, 0 
                call printf 
                pop_all
label_3:
				push_all
				mov RDI, bool_fmt		;first arg, format "%s" 
				mov RDX, RBP
                sub RDX, 16     ; make RDX to point at location of variable on the stack
                mov RSI, RDX 
                mov RAX, 0 
                call scanf 
                pop_all
label_4:
				push_all
				mov RDI, bool_fmt		;first arg, format "%s" 
				mov RDX, RBP
                sub RDX, 16     ; make RDX to point at location of variable on the stack
                mov RSI, RDX 
                mov RAX, 0 
                call printf 
                pop_all
main_end:
				LEAVE
				ADD RSP, 8     ; align RSP back to intial position
				ret

