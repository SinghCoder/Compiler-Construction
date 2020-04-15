;/***************************************
                ;|GROUP-09|    
    ;Aditya Upadhyay      -   2017A7PS0083P
    ;Harpider Jot Singh   -   2017A7PS0057P
    ;Jaladi Lakshmi Teja  -   2017A7PS0068P
    ;Vishal Mittal        -   2017A7PS0080P
    ;Yash Vijay           -   2017A7PS0072P
;*****************************************
extern printf, scanf, exit
section .data
		ffour_reg_fmt: db `RAX = %ld, RBX = %ld, RCX = %ld, RDX = %ld`, 10, 0
		lfour_reg_fmt: db `RSP = %ld, RBP = %ld, RSI = %ld, RDI = %ld\n`, 10, 0
		int_fmt: db "%d", 0
		real_fmt: db "%lf", 0
		bool_fmt: db "%d", 0
		str_fmt: db "%s",10, 0
		str_no_new_line_fmt: db "%s",0
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
                    align_16_rsp ;align RSP to 16 byte boundary
                    call printf 
                    remove_align_16_rsp ;realign RSP
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
                    align_16_rsp ;align RSP to 16 byte boundary
                    call printf 
                    remove_align_16_rsp ;realign RSP
                    pop_all
                %endmacro 

				%macro print_str 1 
                    section .data
                        %%str db %1,0 ;% is to make label local to this macro, to enable mutiple calls within same label
                    section .text
                        push_all
                        mov RDI, str_fmt    ;"%s\n"
                        mov RSI, %%str
                        mov RAX, 0 
                        align_16_rsp ;align RSP to 16 byte boundary
                        call printf 
                        remove_align_16_rsp ;realign RSP
                        pop_all
                %endmacro 

				%macro print_str_no_new_line 1 
                    section .data
                        %%str db %1,0 ;% is to make label local to this macro, to enable mutiple calls within same label
                    section .text
                        push_all
                        mov RDI, str_no_new_line_fmt    ;"%s"
                        mov RSI, %%str
                        mov RAX, 0 
                        align_16_rsp ;align RSP to 16 byte boundary
                        call printf 
                        remove_align_16_rsp ;realign RSP
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
				ENTER 104, 0
				;reserve space for the input/output params of fn, later flush this space
				sub RSP, 0; allocated space for i/o variables on stack
				push_all
				;Assignment to a INTEGER
				mov RAX, 19
				mov [RBP - 8], RAX
				pop_all
label_1:
				push_all
				;Assignment to a INTEGER
				mov RAX, 56
				mov [RBP - 16], RAX
				pop_all
label_3:
				push_all
				;Assignment to a INTEGER
				mov RAX, 3
				mov [RBP - 32], RAX
				pop_all
label_4:
				push_all
				mov RCX, 0; initialize RCX with false
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies
				;Comparison (LE) of integers
				mov RAX, [RBP - 32]
                mov RBX, 6
                cmp RAX, RBX 
                cmovle RCX, RDX 
                mov [RBP - 40], RCX 
				pop_all
				push_all
				;jmp if true code
				mov RAX, [RBP - 40]
				sub RAX, 1
				jz label_5
				pop_all
				jmp main_end
label_5:
				; Code to take input value(s) of x
				push_all
				print_str ""
				print_str "Input: Enter an integer value"
				mov RDI, int_fmt		;first arg, format "%d" 
				mov RDX, RBP
                sub RDX, 0     ; make RDX to point at location of variable on the stack
                mov RAX, 0x0000_0000_ffff_ffff ;machine has sizeof(int) to be 4, for uniformity, We are taking
                mov [RDX] , RAX ;our sizeof(int) to be 8, now scanf will just enter values in lower 32 bits
                ;So, we are firstly clearing upper 32 bits of memory so as to access data properly later
                mov RSI, RDX 
                mov RAX, 0 
                align_16_rsp ;align RSP to 16 byte boundary for scanf call
                call scanf 
                remove_align_16_rsp ;realign it to original position
				pop_all
label_8:
				jmp label_10
label_11:
				push_all
				;Multiplication of integers
				mov RAX, [RBP - 0]
                mov RBX, 2
                mul RBX 
                mov [RBP - 48], RAX 
				pop_all
				push_all
				;Subtraction of integers
				mov RAX, [RBP - 48]
                mov RBX, [RBP - 8]
                sub RAX, RBX 
                mov [RBP - 56], RAX 
				pop_all
				push_all
				;Addition of integers
				mov RAX, [RBP - 56]
                mov RBX, [RBP - 32]
                add RAX, RBX 
                mov [RBP - 64], RAX 
				pop_all
				push_all
				;Assignment to a INTEGER
				mov RAX, [RBP - 64]
				mov [RBP - 8], RAX
				pop_all
label_13:
				; Code to output value(s) of y
				print_str ""
				print_str_no_new_line "Output: "
				push_all
				mov RDI, int_fmt		;first arg, format "%d" 
				mov RDX, RBP
                sub RDX, 8     ; make RDX to point at location of variable on the stack
                mov RSI, [RDX]  ; for integer, value stored at offset should be passed to printf
                mov RAX, 0 
                push_all
                align_16_rsp ;align RSP to 16 byte boundary for printf call
                call printf 
                remove_align_16_rsp ;realign RSP
                pop_all
				pop_all
				print_str "" ;print a newline after output
				jmp label_9
label_19:
				push_all
				;Multiplication of integers
				mov RAX, [RBP - 0]
                mov RBX, 3
                mul RBX 
                mov [RBP - 72], RAX 
				pop_all
				push_all
				;Subtraction of integers
				mov RAX, [RBP - 72]
                mov RBX, [RBP - 16]
                sub RAX, RBX 
                mov [RBP - 80], RAX 
				pop_all
				push_all
				;Addition of integers
				mov RAX, [RBP - 80]
                mov RBX, [RBP - 32]
                add RAX, RBX 
                mov [RBP - 88], RAX 
				pop_all
				push_all
				;Assignment to a INTEGER
				mov RAX, [RBP - 88]
				mov [RBP - 16], RAX
				pop_all
label_21:
				; Code to output value(s) of z
				print_str ""
				print_str_no_new_line "Output: "
				push_all
				mov RDI, int_fmt		;first arg, format "%d" 
				mov RDX, RBP
                sub RDX, 16     ; make RDX to point at location of variable on the stack
                mov RSI, [RDX]  ; for integer, value stored at offset should be passed to printf
                mov RAX, 0 
                push_all
                align_16_rsp ;align RSP to 16 byte boundary for printf call
                call printf 
                remove_align_16_rsp ;realign RSP
                pop_all
				pop_all
				print_str "" ;print a newline after output
				jmp label_9
label_27:
				; Code to output value(s) of x
				print_str ""
				print_str_no_new_line "Output: "
				push_all
				mov RDI, int_fmt		;first arg, format "%d" 
				mov RDX, RBP
                sub RDX, 0     ; make RDX to point at location of variable on the stack
                mov RSI, [RDX]  ; for integer, value stored at offset should be passed to printf
                mov RAX, 0 
                push_all
                align_16_rsp ;align RSP to 16 byte boundary for printf call
                call printf 
                remove_align_16_rsp ;realign RSP
                pop_all
				pop_all
				print_str "" ;print a newline after output
				jmp label_9
label_10:
				push_all
				mov RCX, 0; initialize RCX with false
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies
				;Comparison (EQ) of integers
				mov RAX, 1
                mov RBX, [RBP - 0]
                cmp RAX, RBX 
                cmove RCX, RDX 
                mov [RBP - 96], RCX 
				pop_all
				push_all
				;jmp if true code
				mov RAX, [RBP - 96]
				sub RAX, 1
				jz label_11
				pop_all
				push_all
				mov RCX, 0; initialize RCX with false
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies
				;Comparison (EQ) of integers
				mov RAX, 2
                mov RBX, [RBP - 0]
                cmp RAX, RBX 
                cmove RCX, RDX 
                mov [RBP - 96], RCX 
				pop_all
				push_all
				;jmp if true code
				mov RAX, [RBP - 96]
				sub RAX, 1
				jz label_19
				pop_all
				jmp label_27
label_9:
				; Code to output value(s) of y
				print_str ""
				print_str_no_new_line "Output: "
				push_all
				mov RDI, int_fmt		;first arg, format "%d" 
				mov RDX, RBP
                sub RDX, 8     ; make RDX to point at location of variable on the stack
                mov RSI, [RDX]  ; for integer, value stored at offset should be passed to printf
                mov RAX, 0 
                push_all
                align_16_rsp ;align RSP to 16 byte boundary for printf call
                call printf 
                remove_align_16_rsp ;realign RSP
                pop_all
				pop_all
				print_str "" ;print a newline after output
label_31:
				; Code to output value(s) of z
				print_str ""
				print_str_no_new_line "Output: "
				push_all
				mov RDI, int_fmt		;first arg, format "%d" 
				mov RDX, RBP
                sub RDX, 16     ; make RDX to point at location of variable on the stack
                mov RSI, [RDX]  ; for integer, value stored at offset should be passed to printf
                mov RAX, 0 
                push_all
                align_16_rsp ;align RSP to 16 byte boundary for printf call
                call printf 
                remove_align_16_rsp ;realign RSP
                pop_all
				pop_all
				print_str "" ;print a newline after output
				push_all
				;Addition of integers
				mov RAX, [RBP - 32]
                mov RBX, 1
                add RAX, RBX 
                mov [RBP - 32], RAX 
				pop_all
				jmp label_4
main_end:
				;Deallocate space given to I/O variables on stack
				add RSP, 0
				LEAVE
				ret

