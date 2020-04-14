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
binarySearch:
				ENTER 288, 0
				;reserve space for the input/output params of fn, later flush this space
				sub RSP, 32; allocated space for i/o variables on stack
				; Allocating space to m inp param on stack
				mov RAX, [RBP + 16 + 8 + 8*0]; copy value of this param from caller
				mov RAX, [RAX] ;copy to my local space for the param
				mov [RBP - 240], RAX ;copy to my local space for the param
				; Allocating space to A inp param on stack
				mov RAX, [RBP + 16 + 8 + 8*1]; copy value of this param from caller
				mov RAX, [RAX] ;copy to my local space for the param
				mov [RBP - 248], RAX ;copy to my local space for the param
				; Allocating space to x inp param on stack
				mov RAX, [RBP + 16 + 8 + 8*2]; copy value of this param from caller
				mov RAX, [RAX] ;copy to my local space for the param
				mov [RBP - 256], RAX ;copy to my local space for the param
				print_first_four
				print_last_four
				; Code to output value(s) of A
				print_str "Output: "
				push_all
				mov RDI, int_fmt		;first arg, format "%d" 
				mov RAX, 1 ; Load low range value into RAX
				mov RBX, 5 ; Load high range value into RBX
				;Store count of array elements in RCX
				mov RCX, RBX
				sub RCX, RAX
				add RCX, 1  ; count = high_range - low_range + 1
				;just check RCX value to verify
				print_first_four
				print_last_four
				print_str ""
				;printing array elements
				mov RDX, [RBP - 248] ;address of arr[0] in RDX from contents of [A]
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
				mov [RBP - 32], RAX
				pop_all
label_4:
				push_all
				;Unary minus for integers
				mov RAX, [RBP - 32]
                neg RAX
                mov [RBP - 40], RAX 
				pop_all
				push_all
				;Assignment to a INTEGER
				mov RAX, [RBP - 40]
				mov [RBP - 264], RAX
				pop_all
label_6:
				push_all
				;Assignment to a INTEGER
				mov RAX, 1
				mov [RBP - 0], RAX
				pop_all
label_9:
				push_all
				;Assignment to a INTEGER
				mov RAX, [RBP - 240]
				mov [RBP - 8], RAX
				pop_all
label_11:
label_13:
				push_all
				mov RCX, 0; initialize RCX with false
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies
				;Comparison (LE) of integers
				mov RAX, [RBP - 0]
                mov RBX, [RBP - 8]
                cmp RAX, RBX 
                cmovle RCX, RDX 
                mov [RBP - 48], RCX 
				pop_all
				push_all
				;jmp if true code
				mov RAX, [RBP - 48]
				sub RAX, 1
				jz label_14
				pop_all
				jmp binarySearch_end
label_14:
				push_all
				;Addition of integers
				mov RAX, [RBP - 0]
                mov RBX, [RBP - 8]
                add RAX, RBX 
                mov [RBP - 56], RAX 
				pop_all
				push_all
				;Division of integers
				mov RDX, 0
                mov RAX, [RBP - 56]
                mov RBX, 2
                div RBX 
                mov [RBP - 64], RAX 
				pop_all
				push_all
				;Assignment to a INTEGER
				mov RAX, [RBP - 64]
				mov [RBP - 16], RAX
				pop_all
label_19:
				push_all
				mov RCX, 0; initialize RCX with false
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies
				;Comparison (LT) of integers
				mov RAX, [RBP - 16]
                mov RBX, 1
                cmp RAX, RBX 
                cmovl RCX, RDX 
                mov [RBP - 80], RCX 
				pop_all
				push_all
				;exit if condition is true
				mov RAX, [RBP - 80]
				sub RAX, 1
				jnz label_26
				mov RDI, 0
				print_str "Invalid array access, exiting program"
				call exit
				pop_all
label_26:
				push_all
				mov RCX, 0; initialize RCX with false
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies
				;Comparison (GT) of integers
				mov RAX, [RBP - 16]
                mov RBX, 5
                cmp RAX, RBX 
                cmovg RCX, RDX 
                mov [RBP - 80], RCX 
				pop_all
				push_all
				;exit if condition is true
				mov RAX, [RBP - 80]
				sub RAX, 1
				jnz label_25
				mov RDI, 0
				print_str "Invalid array access, exiting program"
				call exit
				pop_all
label_25:
				push_all
				;Subtraction of integers
				mov RAX, [RBP - 16]
                mov RBX, 1
                sub RAX, RBX 
                mov [RBP - 96], RAX 
				pop_all
				push_all
				;Multiplication of integers
				mov RAX, [RBP - 96]
                mov RBX, 8
                mul RBX 
                mov [RBP - 88], RAX 
				pop_all
				push_all
				; array access
				mov RCX, [RBP - 88]
				mov RBX, [RBP - 248]
				sub RBX, RCX
				mov RAX, [RBX]
				mov [RBP - 72], RAX
				pop_all
				push_all
				mov RCX, 0; initialize RCX with false
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies
				;Comparison (EQ) of integers
				mov RAX, [RBP - 256]
                mov RBX, [RBP - 72]
                cmp RAX, RBX 
                cmove RCX, RDX 
                mov [RBP - 104], RCX 
				pop_all
				push_all
				;Assignment to a BOOLEAN
				mov RAX, [RBP - 104]
				mov [RBP - 24], RAX
				pop_all
label_23:
				; Code to output value(s) of flag
				print_str "Output: "
				push_all
				mov RDI, bool_fmt		;first arg, format "%s" 
				mov RDX, RBP
                sub RDX, 24     ; make RDX to point at location of variable on the stack
                mov RSI, [RDX]    
                mov RAX, 0 
                push_all
                call printf 
                pop_all
				pop_all
				print_str "" ;print a newline after output
label_28:
				jmp label_31
label_32:
				print_str "101"
label_34:
				push_all
				;Assignment to a INTEGER
				mov RAX, [RBP - 16]
				mov [RBP - 264], RAX
				pop_all
label_36:
				push_all
				;Addition of integers
				mov RAX, [RBP - 8]
                mov RBX, 1
                add RAX, RBX 
                mov [RBP - 112], RAX 
				pop_all
				push_all
				;Assignment to a INTEGER
				mov RAX, [RBP - 112]
				mov [RBP - 0], RAX
				pop_all
				jmp label_30
label_40:
				print_str "102"
				jmp label_30
label_31:
				push_all
				mov RCX, 0; initialize RCX with false
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies
				;Comparison (EQ) of integers
				mov RAX, 1
                mov RBX, [RBP - 24]
                cmp RAX, RBX 
                cmove RCX, RDX 
                mov [RBP - 120], RCX 
				pop_all
				push_all
				;jmp if true code
				mov RAX, [RBP - 120]
				sub RAX, 1
				jz label_32
				pop_all
				push_all
				mov RCX, 0; initialize RCX with false
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies
				;Comparison (EQ) of integers
				mov RAX, 0
                mov RBX, [RBP - 24]
                cmp RAX, RBX 
                cmove RCX, RDX 
                mov [RBP - 120], RCX 
				pop_all
				push_all
				;jmp if true code
				mov RAX, [RBP - 120]
				sub RAX, 1
				jz label_40
				pop_all
label_30:
				push_all
				mov RCX, 0; initialize RCX with false
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies
				;Comparison (LT) of integers
				mov RAX, [RBP - 16]
                mov RBX, 1
                cmp RAX, RBX 
                cmovl RCX, RDX 
                mov [RBP - 136], RCX 
				pop_all
				push_all
				;exit if condition is true
				mov RAX, [RBP - 136]
				sub RAX, 1
				jnz label_46
				mov RDI, 0
				print_str "Invalid array access, exiting program"
				call exit
				pop_all
label_46:
				push_all
				mov RCX, 0; initialize RCX with false
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies
				;Comparison (GT) of integers
				mov RAX, [RBP - 16]
                mov RBX, 5
                cmp RAX, RBX 
                cmovg RCX, RDX 
                mov [RBP - 136], RCX 
				pop_all
				push_all
				;exit if condition is true
				mov RAX, [RBP - 136]
				sub RAX, 1
				jnz label_45
				mov RDI, 0
				print_str "Invalid array access, exiting program"
				call exit
				pop_all
label_45:
				push_all
				;Subtraction of integers
				mov RAX, [RBP - 16]
                mov RBX, 1
                sub RAX, RBX 
                mov [RBP - 152], RAX 
				pop_all
				push_all
				;Multiplication of integers
				mov RAX, [RBP - 152]
                mov RBX, 8
                mul RBX 
                mov [RBP - 144], RAX 
				pop_all
				push_all
				; array access
				mov RCX, [RBP - 144]
				mov RBX, [RBP - 248]
				sub RBX, RCX
				mov RAX, [RBX]
				mov [RBP - 128], RAX
				pop_all
				push_all
				mov RCX, 0; initialize RCX with false
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies
				;Comparison (LT) of integers
				mov RAX, [RBP - 256]
                mov RBX, [RBP - 128]
                cmp RAX, RBX 
                cmovl RCX, RDX 
                mov [RBP - 160], RCX 
				pop_all
				push_all
				;Assignment to a BOOLEAN
				mov RAX, [RBP - 160]
				mov [RBP - 24], RAX
				pop_all
label_43:
				jmp label_48
label_49:
				print_str "201"
label_51:
				push_all
				;Subtraction of integers
				mov RAX, [RBP - 16]
                mov RBX, 1
                sub RAX, RBX 
                mov [RBP - 168], RAX 
				pop_all
				push_all
				;Assignment to a INTEGER
				mov RAX, [RBP - 168]
				mov [RBP - 8], RAX
				pop_all
				jmp label_47
label_54:
				print_str "202"
				jmp label_47
label_48:
				push_all
				mov RCX, 0; initialize RCX with false
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies
				;Comparison (EQ) of integers
				mov RAX, 1
                mov RBX, [RBP - 24]
                cmp RAX, RBX 
                cmove RCX, RDX 
                mov [RBP - 176], RCX 
				pop_all
				push_all
				;jmp if true code
				mov RAX, [RBP - 176]
				sub RAX, 1
				jz label_49
				pop_all
				push_all
				mov RCX, 0; initialize RCX with false
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies
				;Comparison (EQ) of integers
				mov RAX, 0
                mov RBX, [RBP - 24]
                cmp RAX, RBX 
                cmove RCX, RDX 
                mov [RBP - 176], RCX 
				pop_all
				push_all
				;jmp if true code
				mov RAX, [RBP - 176]
				sub RAX, 1
				jz label_54
				pop_all
label_47:
				push_all
				mov RCX, 0; initialize RCX with false
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies
				;Comparison (LT) of integers
				mov RAX, [RBP - 16]
                mov RBX, 1
                cmp RAX, RBX 
                cmovl RCX, RDX 
                mov [RBP - 192], RCX 
				pop_all
				push_all
				;exit if condition is true
				mov RAX, [RBP - 192]
				sub RAX, 1
				jnz label_60
				mov RDI, 0
				print_str "Invalid array access, exiting program"
				call exit
				pop_all
label_60:
				push_all
				mov RCX, 0; initialize RCX with false
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies
				;Comparison (GT) of integers
				mov RAX, [RBP - 16]
                mov RBX, 5
                cmp RAX, RBX 
                cmovg RCX, RDX 
                mov [RBP - 192], RCX 
				pop_all
				push_all
				;exit if condition is true
				mov RAX, [RBP - 192]
				sub RAX, 1
				jnz label_59
				mov RDI, 0
				print_str "Invalid array access, exiting program"
				call exit
				pop_all
label_59:
				push_all
				;Subtraction of integers
				mov RAX, [RBP - 16]
                mov RBX, 1
                sub RAX, RBX 
                mov [RBP - 208], RAX 
				pop_all
				push_all
				;Multiplication of integers
				mov RAX, [RBP - 208]
                mov RBX, 8
                mul RBX 
                mov [RBP - 200], RAX 
				pop_all
				push_all
				; array access
				mov RCX, [RBP - 200]
				mov RBX, [RBP - 248]
				sub RBX, RCX
				mov RAX, [RBX]
				mov [RBP - 184], RAX
				pop_all
				push_all
				mov RCX, 0; initialize RCX with false
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies
				;Comparison (GT) of integers
				mov RAX, [RBP - 256]
                mov RBX, [RBP - 184]
                cmp RAX, RBX 
                cmovg RCX, RDX 
                mov [RBP - 216], RCX 
				pop_all
				push_all
				;Assignment to a BOOLEAN
				mov RAX, [RBP - 216]
				mov [RBP - 24], RAX
				pop_all
label_57:
				jmp label_61
label_62:
				print_str "301"
label_64:
				push_all
				;Addition of integers
				mov RAX, [RBP - 16]
                mov RBX, 1
                add RAX, RBX 
                mov [RBP - 224], RAX 
				pop_all
				push_all
				;Assignment to a INTEGER
				mov RAX, [RBP - 224]
				mov [RBP - 0], RAX
				pop_all
				jmp label_17
label_67:
				print_str "302"
				jmp label_17
label_61:
				push_all
				mov RCX, 0; initialize RCX with false
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies
				;Comparison (EQ) of integers
				mov RAX, 1
                mov RBX, [RBP - 24]
                cmp RAX, RBX 
                cmove RCX, RDX 
                mov [RBP - 232], RCX 
				pop_all
				push_all
				;jmp if true code
				mov RAX, [RBP - 232]
				sub RAX, 1
				jz label_62
				pop_all
				push_all
				mov RCX, 0; initialize RCX with false
                mov RDX, 1 ; assign RDX, a value true to assign to result if condition satisfies
				;Comparison (EQ) of integers
				mov RAX, 0
                mov RBX, [RBP - 24]
                cmp RAX, RBX 
                cmove RCX, RDX 
                mov [RBP - 232], RCX 
				pop_all
				push_all
				;jmp if true code
				mov RAX, [RBP - 232]
				sub RAX, 1
				jz label_67
				pop_all
label_17:
				jmp label_13
binarySearch_end:
				print_first_four
				print_last_four
				; copy values to output parameters that u have computed so far
				mov RAX, [RBP - 264]
				mov RBX, [RBP + 16 + 8*3 + 8*1]
				mov [RBX], RAX
				;Deallocate space given to I/O variables on stack
				add RSP, 32
				LEAVE
				ret

main:
				ENTER 72, 0
				;reserve space for the input/output params of fn, later flush this space
				sub RSP, 0; allocated space for i/o variables on stack
				print_first_four
				print_last_four
				push_all
				;Assignment to a INTEGER
				mov RAX, 5
				mov [RBP - 0], RAX
				pop_all
label_70:
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
				mov RBX, 5 ; Load high range value into RBX
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
label_71: ;scanf loop label
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
				jnz label_71
				pop_all
label_72:
				; Code to take input value(s) of search
				push_all
				print_str "Input: Enter an integer value"
				mov RDI, int_fmt		;first arg, format "%d" 
				mov RDX, RBP
                sub RDX, 56     ; make RDX to point at location of variable on the stack
                mov RAX, 0x0000_0000_ffff_ffff ;machine has sizeof(int) to be 4, for uniformity, We are taking
                mov [RDX] , RAX ;our sizeof(int) to be 8, now scanf will just enter values in lower 32 bits
                ;So, we are firstly clearing upper 32 bits of memory so as to access data properly later
                mov RSI, RDX 
                mov RAX, 0 
                align_16_rsp ;align RSP to 16 byte boundary for scanf call
                call scanf 
                remove_align_16_rsp ;realign it to original position
				pop_all
label_74:
				;pushing a parameter(idx) on stack
				mov RAX, RBP
				sub RAX, 64
				push RAX
				;pushing a parameter(search) on stack
				mov RAX, RBP
				sub RAX, 56
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
				call binarySearch
				pop RBP
				;popping back input parameters
				pop RAX
				pop RAX
				pop RAX
				;popping back output parameters
				pop RAX
label_75:
				; Code to output value(s) of idx
				print_str "Output: "
				push_all
				mov RDI, int_fmt		;first arg, format "%d" 
				mov RDX, RBP
                sub RDX, 64     ; make RDX to point at location of variable on the stack
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

