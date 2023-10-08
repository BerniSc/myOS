global long_mode_start
extern kernel_main

; interrupts
extern keyboad_handler_isr
global load_interrupt_data_table
global keyboard_handler_interrupt

;   =====================================================================================
;   MACRO Defintions
;   =====================================================================================
%macro pushaq 0
    push rax
    push rcx
    push rdx
    push rbx
    push rsp
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endm ; pushaq

%macro popaq 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rsp
    pop rbx
    pop rdx
    pop rcx
    pop rax
%endm ; popaq

;   =====================================================================================
;   Code Section
;   =====================================================================================

global heap_start
global heap_end

section .heap
bits 64
heap_start:         ; Startlable for Heap
    resb 4096 * 4   ; Reserve 16KB for the heap
heap_end:           ; Endlable for Heap

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
;   NORMAL INTERRUPT SERVICE ROUTINES Defintions
;   =====================================================================================
    
global division_zero_handler_interrupt
extern div_zero_exception_handler_isr

division_zero_handler_interrupt:
    pushaq
    cld
    call div_zero_exception_handler_isr
    popaq
	iretq