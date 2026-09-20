section .text

global insb
global insw
global outb
global outw


; ============================================================
; uint8_t insb(uint16_t port)
; Read 1 byte from an I/O port
; ============================================================

insb:
    push ebp
    mov ebp, esp

    xor eax, eax

    mov edx, [ebp + 8]
    in  al, dx

    pop ebp
    ret


; ============================================================
; uint16_t insw(uint16_t port)
; Read 2 bytes from an I/O port
; ============================================================

insw:
    push ebp
    mov ebp, esp

    xor eax, eax

    mov edx, [ebp + 8]
    in  ax, dx

    pop ebp
    ret


; ============================================================
; void outb(uint16_t port, uint8_t value)
; Write 1 byte to an I/O port
; ============================================================

outb:
    push ebp
    mov ebp, esp

    mov edx, [ebp + 8]
    mov eax, [ebp + 12]

    out dx, al

    pop ebp
    ret


; ============================================================
; void outw(uint16_t port, uint16_t value)
; Write 2 bytes to an I/O port
; ============================================================

outw:
    push ebp
    mov ebp, esp

    mov edx, [ebp + 8]
    mov eax, [ebp + 12]

    out dx, ax

    pop ebp
    ret