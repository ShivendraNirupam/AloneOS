#include "kernel.h"
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"


uint16_t* video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;
struct paging_4gb_chunk* kernel_chunk = 0;

uint16_t terminal_make_char(char c, char color) {
    return (color << 8) | c;
}

void terminal_putchar(int x, int y, char c, char color) {
    video_mem[(y * VGA::WIDTH) + x] = terminal_make_char(c, color);
}

void terminal_writechar(char c, char color) {
    if(c == '\n') {
        terminal_col = 0;
        terminal_row += 1;
        return;
    }
    terminal_putchar(terminal_col, terminal_row, c, color);
    terminal_col += 1;
    if(terminal_row >= VGA::WIDTH) {
        terminal_row += 1;
        terminal_col = 0;
    }
}

void terminal_initialize() {

    video_mem = reinterpret_cast<uint16_t*>(0xB8000);
    terminal_row = 0;
    terminal_col = 0;

    for(int y = 0; y < static_cast<int>(VGA::HEIGHT); y++) {
        for(int x = 0; x < static_cast<int>(VGA::WIDTH); x++) {
            terminal_putchar(x, y, ' ', 0);
        }
    }
}

size_t strlen(const char* str) {
    size_t len = 0;
    while(str[len]) {
        len++;
    }
    return len;
}

void print(const char* str) {
    size_t len = strlen(str);
    for(size_t i = 0; i < len; i++) {
        terminal_writechar(str[i], 15);
    }
}

extern "C" void kernel_main() {
    terminal_initialize();
    print(" Hello world\n");

    kheap_init(); // Initialise the heap 


    idt_init(); // Inititalise the interrupt descriptor table

    // --------------------------
    // Setup Paging
    //---------------------------

    kernel_chunk = paging_new_4gb(
        paging::PAGING_IS_WRITEABLE |
        paging::PAGING_IS_PRESENT |
        paging::PAGING_ACCESS_FROM_ALL
    ); 
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));
    enable_paging();

    //---------------------------
    // Paging setup above
    //---------------------------

    enable_interrupts(); // Enable the interrupts

}