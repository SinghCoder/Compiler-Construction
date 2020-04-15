extern printf, scanf
section .data
		ffour_reg_fmt: db `RAX = %ld, RBX = %ld, RCX = %ld, RDX = %ld`, 10, 0
		lfour_reg_fmt: db `RSP = %ld, RBP = %ld, RSI = %ld, RDI = %ld\n`, 10, 0
		int_fmt: db "%d", 0
		real_fmt: db "%lf", 0
		bool_fmt: db "%d", 0
		str_fmt: db "%s", 0
section .text
				; Push all registers on stack to prevent value loss
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

				; Restore values of registers from stack
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

				; Print RAX, RBX, RCX, RDX values
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

				; Print RSP, RBP, RSI, RDI values
				%macro print_last_four 0 
                    push_all
                    push RDI 
                    push RSI 
                    push RBP 
                    push RSP 
                    mov RDI, lfour_reg_fmt		; first arg, format 
                    pop RSI 
                    add RSI, 88 ; 88 because RSP has pushed additional 11 registers(8 in push_all, and 4 after that), so original value is 88 more than current
                    pop RDX 
                    pop RCX 
                    pop R8 
                    mov RAX, 0 
                    call printf 
                    pop_all
                %endmacro 

				%macro print_str 1 
                    section .data
                        %%str db %1,10,0 ;% is to make label local to this macro, to enable mutiple calls within same label
                    section .text
                        push_all
                        mov RDI, str_fmt    ;"%s"
                        mov RSI, %%str
                        mov RAX, 0 
                        call printf 
                        pop_all
                %endmacro 

				%macro align_16_rsp 0 
                    push RBX
					push RDX
					push RAX
					mov RDX, 0
					mov RAX, RSP
					mov RBX, 16
					idiv RBX
					mov RBX, RDX
					pop RAX
					pop RDX
					sub RSP, RBX
                %endmacro 

				%macro remove_align_16_rsp 0 
                    add RSP, RBX
					pop RBX
                %endmacro 

				global main
main:
				SUB RSP, 8     ; align RSP to 16 boundary to enable calls to scanf, etc
				ENTER 24, 0
				print_first_four
				print_last_four
				push_all
				;Multiplication of integers
				mov RAX, 1
                mov RBX, 2
                mul RBX 
                mov [RBP - 8], RAX 
				pop_all
				push_all
				;Unary minus for integers
				mov RAX, [RBP - 8]
                neg RAX
                mov [RBP - 16], RAX 
				pop_all
				push_all
				;Assignment to a INTEGER
				mov RAX, [RBP - 16]
				mov [RBP - 0], RAX
				pop_all
label_0:
				; Code to output value(s) of b
				print_str "Output: "
				push_all
				push_all
				mov RDI, int_fmt		;first arg, format "%d" 
				mov RDX, RBP
                sub RDX, 0     ; make RDX to point at location of variable on the stack
                mov RSI, [RDX]  ; for integer, value stored at offset should be passed to printf
                mov RAX, 0 
                call printf 
                pop_all
				print_str "" ;print a newline after output
				pop_all
main_end:
				print_first_four
				print_last_four
				LEAVE
				ADD RSP, 8     ; align RSP back to intial position
				ret

