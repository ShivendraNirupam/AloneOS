section .text

extern int21h_handler
extern nointerrupt_handler

global idt_load
global int21h
global no_interrupt


; ============================================================
; void idt_load(void* ptr)
; ============================================================

idt_load:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]

    lidt [eax]

    pop ebp
    ret


; ============================================================
; Keyboard IRQ1 -> INT 0x21
; ============================================================

int21h:
    pushad

    call int21h_handler

    popad

    iretd


; ============================================================
; Default interrupt handler
; ============================================================

no_interrupt:
    pushad

    call nointerrupt_handler

    popad

    iretd