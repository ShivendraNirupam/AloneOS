#pragma once
#include <stddef.h>
#include <stdint.h>

constexpr uint16_t KERNEL_CODE_SELECTOR = 0x08;
constexpr uint16_t KERNEL_DATA_SELECTION = 0X10;
constexpr size_t ALONEOS_TOTAL_INTERRUPTS = 512;
constexpr size_t ALONEOS_HEAP_SIZE_BYTES = 104'857'600; // 100 MB for heap 100 * 1024 * 1024
constexpr size_t ALONEOS_HEAP_BLOCK_SIZE = 4096;
constexpr uintptr_t ALONEOS_HEAP_ADDRESS = 0x01000000;
constexpr uintptr_t ALONEOS_HEAP_TABLE_ADDRESS = 0x00007E00;