#pragma once

extern "C" {

unsigned char insb(unsigned short port);
unsigned short insw(unsigned short port);

void outb(unsigned short port, unsigned short value);
void outw(unsigned short port, unsigned short value);

}