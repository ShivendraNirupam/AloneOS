#pragma once
#include <stdint.h>
#include <stddef.h>


namespace VGA {
    constexpr size_t WIDTH = 80;
    constexpr size_t HEIGHT = 20;
}



extern "C" void kernel_start();
