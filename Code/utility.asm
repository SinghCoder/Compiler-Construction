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