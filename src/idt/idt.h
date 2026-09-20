#pragma once
#include <stdint.h>

struct idt_desc {

    uint16_t offset_1; // Offset bits 0- 15
    uint16_t selector; // Selector thats in our GDT
    uint16_t zero; // Does nothing
    uint16_t type_attr; // Descriptor type and attributes
    uint16_t offset_2; // Offset bits 16 - 31

} __attribute__((packed));

struct idtr_desc {
    uint16_t limit; // Size of the descriptor table -1
    uint32_t base; // Base address of the start of the interrupt descriptor table 
} __attribute__((packed));

void idt_init();