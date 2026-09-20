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

extern "C" void int21h_handler() {
    print("Keyboard pressed");
    outb(0x20, 0x20);
}

extern "C" void nointerrupt_handler() {
    outb(0x20, 0x20);
}

void idt_zero() {
    print("Divide by zero error \n");
}

void idt_set(int interrupt_no, void* address) {
    struct idt_desc* desc = &idt_descriptors[interrupt_no];
    desc->offset_1 = reinterpret_cast<uint32_t>(address) & 0x0000ffff;
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0x00;
    desc->type_attr = 0xEE;
    desc->offset_2 = reinterpret_cast<uint32_t>(address) >> 16;
}

void idt_init() {

    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) - 1;
    idtr_descriptor.base = static_cast<uint32_t>(
        reinterpret_cast<uint32_t>(idt_descriptors)
    );

    for(size_t i = 0; i < ALONEOS_TOTAL_INTERRUPTS; i++) {
        idt_set(i, reinterpret_cast<void*>(no_interrupt));
    }

    idt_set(0, reinterpret_cast<void*>(idt_zero));
    idt_set(0x20, reinterpret_cast<void*>(int21h));

    // Load the interrupt descriptor table
    idt_load(&idtr_descriptor);

}