#include "idt.h"
#include "config.h"
#include "memory/memory.h"
#include "kernel.h"
#include "io/io.h"

struct idt_desc idt_descriptors[ALONEOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern "C" void idt_load(void* ptr);
extern "C" void int21h();
extern "C" void no_interrupt();
extern "C" void isr0();                     // new asm stub for divide error

extern "C" void int21h_handler() {
    uint8_t scancode = insb(0x60);          // must read, or no more IRQs
    (void)scancode;
    print("Keyboard pressed\n");
    outb(0x20, 0x20);
}

extern "C" void nointerrupt_handler() {
    outb(0x20, 0x20);
}

extern "C" void idt_zero_handler() {        // called from isr0, never returns
    print("Divide by zero error\n");
    while (1) { asm volatile("cli; hlt"); }
}

void idt_set(int interrupt_no, void* address) {
    struct idt_desc* desc = &idt_descriptors[interrupt_no];
    uintptr_t addr = reinterpret_cast<uintptr_t>(address);
    desc->offset_1 = addr & 0xFFFF;
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0x00;
    desc->type_attr = 0x8E;                 // present, ring 0, 32-bit interrupt gate
    desc->offset_2 = addr >> 16;
}

void idt_init() {
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) - 1;
    idtr_descriptor.base = reinterpret_cast<uintptr_t>(idt_descriptors);

    for (size_t i = 0; i < ALONEOS_TOTAL_INTERRUPTS; i++) {
        idt_set(i, reinterpret_cast<void*>(no_interrupt));
    }

    idt_set(0, reinterpret_cast<void*>(isr0));
    idt_set(0x21, reinterpret_cast<void*>(int21h));   // keyboard = IRQ1 = 0x21

    idt_load(&idtr_descriptor);
}