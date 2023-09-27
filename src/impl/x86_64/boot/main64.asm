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
	mov dword [0xb8000], 0x4f524f45
	mov dword [0xb8004], 0x4f3a4f52
	mov dword [0xb8008], 0x4f204f20
	mov byte  [0xb800a], al
	pushaq
	cld
	call keyboad_handler_isr
	popaq
	iretd

load_interrupt_data_table:
	mov edx, [esp + 4]
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