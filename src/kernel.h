#pragma once
#include <stdint.h>
#include <stddef.h>


namespace VGA {
    constexpr size_t WIDTH = 80;
    constexpr size_t HEIGHT = 20;
}


void   print(const char* str);
size_t strlen(const char* str);



extern "C" void kernel_start();
