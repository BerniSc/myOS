global long_mode_start
extern kernel_main

; interrupts
extern keyboad_handler_isr
global load_interrupt_data_table
global keyboard_handler_interrupt

section .text
bits 64
long_mode_start:
    ; load null into all data segment registers
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

	call kernel_main
    hlt

keyboard_handler_interrupt:
	pushaq
	cld
	call keyboad_handler_isr
	popaq
	iretq

load_interrupt_data_table:
	mov edx, [rsp + 8]
	lidt [edx]
	sti
	ret


;   =====================================================================================
;
;   MACRO Defintions
;
;   =====================================================================================

%macro pushaq 0
    push %rax
    push %rcx
    push %rdx
    push %rbx
    push %rbp
    push %rsi
    push %rdi
%endm ; pushaq

%macro popaq 0
    pop %rdi
    pop %rsi
    pop %rbp
    pop %rbx
    pop %rdx
    pop %rcx
    pop %rax
%endm ; popaq