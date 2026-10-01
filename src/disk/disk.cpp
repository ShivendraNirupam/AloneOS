#include "io/io.h"

int disk_read_sector(int lba, int total, void* buf) {

    outb(0x1f6, (lba >> 24) | 0xe0);
    outb(0x1f2, total);
    outb(0x1f3, static_cast<unsigned char>(lba & 0xff));
    outb(0x1f4, static_cast<unsigned char>(lba >> 8));
    outb(0x1f5, static_cast<unsigned char>(lba >> 16));
    outb(0x1f7, 0x20);

    unsigned short* ptr = reinterpret_cast<unsigned short*>(buf);

    for(int i = 0; i < total; i++) {

        // Wait for the buffer to be ready
        char c = insb(0x1f7);

        while(!(c & 0x08)) {
            c = insb(0x1f7);
        }

        // Copy from hard disk
        for(int j = 0; j < 256; j++) {
            *ptr = insw(0x1f0);
            ptr++;
        }
    }

    return 0;
}