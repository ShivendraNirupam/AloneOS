#include "idt.h"
#include "config.h"
#include "memory/memory.h"
#include "kernel.h"

struct idt_desc idt_descriptors[ALONEOS_TOTAL_INTERRUPTS];

struct idtr_desc idtr_descriptor;

extern "C" void idt_load(void* ptr);

void idt_zero() {
    print("Divide by zero error \n");
}

void idt_set(int interrupt_no, auto address) {
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

    idt_set(0, idt_zero);

    // Load the interrupt descriptor table
    idt_load(&idtr_descriptor);

}