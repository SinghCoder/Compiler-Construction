extern printf, scanf, exit
%include "utility.asm"
section .data
		ffour_reg_fmt: db `RAX = %ld, RBX = %ld, RCX = %ld, RDX = %ld`, 10, 0
		lfour_reg_fmt: db `RSP = %ld, RBP = %ld, RSI = %ld, RDI = %ld\n`, 10, 0
		int_fmt: db "%d", 0
		real_fmt: db "%lf", 0
		bool_fmt: db "%d", 0
		str_fmt: db "%s", 0
section .text
				global main
linearSearch:
				ENTER 24, 0
				;reserve space for the input/output params of fn, later flush this space
				; Allocating space to m inp param on stack
				mov RAX, [RBP + 16 + 8 + 8*0]; copy value of this param from caller
				mov RAX, [RAX] ;copy to my local space for the param
				mov [RBP - 48], RAX ;copy to my local space for the param
				; Allocating space to A inp param on stack
				mov RAX, [RBP + 16 + 8 + 8*1]; copy value of this param from caller
				mov RAX, [RAX] ;copy to my local space for the param
				mov [RBP - 56], RAX ;copy to my local space for the param
				; Allocating space to x inp param on stack
				mov RAX, [RBP + 16 + 8 + 8*2]; copy value of this param from caller
				mov RAX, [RAX] ;copy to my local space for the param
				mov [RBP - 64], RAX ;copy to my local space for the param


				; print_str "hi?"
				push RBP
				pop RBP

				sub RSP, 32; allocated space for i/o variables on stack
				print_first_four
				print_last_four
				; Code to output value(s) of A
				print_str "Output: "
				push_all
				mov RDI, int_fmt		;first arg, format "%d" 
				mov RAX, 1 ; Load low range value into RAX
				mov RBX, 7 ; Load high range value into RBX
				;Store count of array elements in RCX
				mov RCX, RBX
				sub RCX, RAX
				add RCX, 1  ; count = high_range - low_range + 1
				;just check RCX value to verify
				print_first_four
				print_last_four
				print_str ""
				;printing array elements
				mov RDX, [RBP - 56] ;address of arr[0] in RDX from contents of [A]
label_0: ;printf loop label
				mov RSI, [RDX] ;contents of variable to be printed in RSI
				mov RAX, 0 ;num of float regs
				push_all
				call printf
				pop_all
				print_str ""
				sub RDX, 8 ;move pointer to next elem of array
				dec RCX ;decrement num of elem count to be scanned
				jnz label_0
				pop_all
				print_str "" ;print a newline after output
label_1:
				push_all
				;Assignment to a INTEGER
				mov RAX, 1
				mov [RBP - 0], RAX
				pop_all
label_4:
				push_all
				;Unary minus for integers
				mov RAX, [RBP - 0]
                neg RAX
                mov [RBP - 16], RAX 
				pop_all
				push_all
				;Assignment to a INTEGER
				mov RAX, [RBP - 16]
				mov [RBP - 72], RAX
				pop_all
label_6:
				; Code to output value(s) of b
				print_str "Output: "
				push_all
				mov RDI, int_fmt		;first arg, format "%d" 
				mov RDX, RBP
                sub RDX, 0     ; make RDX to point at location of variable on the stack
                mov RSI, [RDX]  ; for integer, value stored at offset should be passed to printf
                mov RAX, 0 
                push_all
                call printf 
                pop_all
				pop_all
				print_str "" ;print a newline after output
label_9:
				push_all
				;Assignment to a INTEGER
				mov RAX, 1
				mov [RBP - 0], RAX
				pop_all
label_11:
				push_all
				mov RCX, 0; initialize RCX with false
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies
				;Comparison (LE) of integers
				mov RAX, [RBP - 0]
                mov RBX, 7
                cmp RAX, RBX 
                cmovle RCX, RDX 
                mov [RBP - 24], RCX 
				pop_all
				push_all
				;jmp if true code
				mov RAX, [RBP - 24]
				sub RAX, 1
				jz label_12
				pop_all
				jmp linearSearch_end
label_12:
label_13:
				push_all
				;Subtraction of integers
				mov RAX, 1
                mov RBX, 1
                sub RAX, RBX 
                mov [RBP - 40], RAX 
				pop_all
				push_all
				;Multiplication of integers
				mov RAX, [RBP - 40]
                mov RBX, 8
                mul RBX 
                mov [RBP - 32], RAX 
				pop_all
				push_all
				; index copy
				mov RAX, [RBP - 56]
				sub RAX, [RBP - 32]
				mov RBX, [RBP - 0]
				mov [RAX], RBX
				pop_all
label_14:
				; Code to output value(s) of A
				print_str "Output: "
				push_all
				mov RDI, int_fmt		;first arg, format "%d" 
				mov RAX, 1 ; Load low range value into RAX
				mov RBX, 7 ; Load high range value into RBX
				;Store count of array elements in RCX
				mov RCX, RBX
				sub RCX, RAX
				add RCX, 1  ; count = high_range - low_range + 1
				;just check RCX value to verify
				print_first_four
				print_last_four
				print_str ""
				;printing array elements
				mov RDX, [RBP - 56] ;address of arr[0] in RDX from contents of [A]
label_16: ;printf loop label
				mov RSI, [RDX] ;contents of variable to be printed in RSI
				mov RAX, 0 ;num of float regs
				push_all
				call printf
				pop_all
				print_str ""
				sub RDX, 8 ;move pointer to next elem of array
				dec RCX ;decrement num of elem count to be scanned
				jnz label_16
				pop_all
				print_str "" ;print a newline after output
				push_all
				;Addition of integers
				mov RAX, [RBP - 0]
                mov RBX, 1
                add RAX, RBX 
                mov [RBP - 0], RAX 
				pop_all
				jmp label_11
linearSearch_end:
				print_first_four
				print_last_four
				; copy values to output parameters that u have computed so far
				mov RAX, [RBP - 72]
				mov RBX, [RBP + 16 + 8*3 + 8*1]
				mov [RBX], RAX
				;Deallocate space given to I/O variables on stack
				add RSP, 32
				LEAVE
				ret

main:
				ENTER 88, 0
				;reserve space for the input/output params of fn, later flush this space
				sub RSP, 0; allocated space for i/o variables on stack
				print_first_four
				print_last_four
				push_all
				;Assignment to a INTEGER
				mov RAX, 7
				mov [RBP - 0], RAX
				pop_all
label_19:
				push_all
				;Declaring static array A, store A[0]'s address at location of A
				MOV RAX, RBP
				SUB RAX, 8
				MOV RBX, RAX
				SUB RAX, 8
				MOV [RBX], RAX
				;Declaration ends
				pop_all
				; Code to take input value(s) of A
				push_all
				mov RDI, int_fmt		;first arg, format "%d" 
				mov RAX, 1 ; Load low range value into RAX
				mov RBX, 7 ; Load high range value into RBX
				;Store count of array elements in RCX
				mov RCX, RBX
				sub RCX, RAX
				add RCX, 1  ; count = high_range - low_range + 1
				;just check RCX value to verify
				print_first_four
				print_last_four
				print_str "Input: Enter {RCX} number of aray elements"
				;taking input for array elements
				mov RDX, [RBP - 8] ;address of arr[0] in RDX from contents of [A]
label_20: ;scanf loop label
				mov RAX, 0x0000_0000_ffff_ffff ;machine has sizeof(int) to be 4, and we took 8, so clear upper 32 bits
				mov [RDX], RAX ;clear upper 32 bits of array's element
				mov RSI, RDX ;address of variable to be scanned in RSI
				mov RAX, 0 ;num pf float regs
				push_all
				align_16_rsp ;align RSP to 16 byte boundary for scanf call
				call scanf
				remove_align_16_rsp ;realign it to original position
				pop_all
				sub RDX, 8 ;move pointer to next elem of array
				dec RCX ;decrement num of elem count to be scanned
				jnz label_20
				pop_all
label_21:
				; Code to take input value(s) of search
				push_all
				print_str "Input: Enter an integer value"
				mov RDI, int_fmt		;first arg, format "%d" 
				mov RDX, RBP
                sub RDX, 72     ; make RDX to point at location of variable on the stack
                mov RAX, 0x0000_0000_ffff_ffff ;machine has sizeof(int) to be 4, for uniformity, We are taking
                mov [RDX] , RAX ;our sizeof(int) to be 8, now scanf will just enter values in lower 32 bits
                ;So, we are firstly clearing upper 32 bits of memory so as to access data properly later
                mov RSI, RDX 
                mov RAX, 0 
                align_16_rsp ;align RSP to 16 byte boundary for scanf call
                call scanf 
                remove_align_16_rsp ;realign it to original position
				pop_all
label_23:
				;pushing a parameter(idx) on stack
				mov RAX, RBP
				sub RAX, 80
				push RAX
				;pushing a parameter(search) on stack
				mov RAX, RBP
				sub RAX, 72
				push RAX
				;pushing a parameter(A) on stack
				mov RAX, RBP
				sub RAX, 8
				push RAX
				;pushing a parameter(m) on stack
				mov RAX, RBP
				sub RAX, 0
				push RAX
				push RBP
				call linearSearch
				pop RBP
				;popping back input parameters
				pop RAX
				pop RAX
				pop RAX
				;popping back output parameters
				pop RAX
label_24:
				; Code to output value(s) of idx
				print_str "Output: "
				push_all
				mov RDI, int_fmt		;first arg, format "%d" 
				mov RDX, RBP
                sub RDX, 80     ; make RDX to point at location of variable on the stack
                mov RSI, [RDX]  ; for integer, value stored at offset should be passed to printf
                mov RAX, 0 
                push_all
                call printf 
                pop_all
				pop_all
				print_str "" ;print a newline after output
main_end:
				print_first_four
				print_last_four
				;Deallocate space given to I/O variables on stack
				add RSP, 0
				LEAVE
				ret

