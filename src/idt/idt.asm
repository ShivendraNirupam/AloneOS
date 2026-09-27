section .text

extern int21h_handler
extern nointerrupt_handler

global idt_load
global int21h
global no_interrupt
global enable_interrupts
global disable_interrupts
global isr0
extern idt_zero_handler

isr0:
    cli
    pushad
    cld
    call idt_zero_handler   ; this one should halt, never return
.hang:
    hlt
    jmp .hang

; ============================================================
;   void enable_interrupts()
; ============================================================


enable_interrupts:
    sti
    ret


; ============================================================
;   void disable_interrupts()
; ============================================================


disable_interrupts:
    cli
    ret


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

    cld

    call int21h_handler

    popad

    iretd


; ============================================================
; Default interrupt handler
; ============================================================

no_interrupt:
    pushad

    cld

    call nointerrupt_handler

    popad

    iretd