#pragma once

#include "config.h"
#include "stdint.h"
#include "stddef.h"


constexpr uint8_t HEAP_BLOCK_TABLE_TAKEN = 0x01;
constexpr uint8_t HEAP_BLOCK_TABLE_FREE = 0X00;

constexpr uint8_t HEAP_BLOCK_HAS_NEXT = 0b10000000;
constexpr uint8_t HEAP_BLOCK_IS_FREE = 0b01000000;

typedef unsigned char HEAP_BLOCK_TABLE_ENTRY;

struct heap_table {
    HEAP_BLOCK_TABLE_ENTRY* entries;
    size_t total;
};

struct heap {
    struct heap_table* table;

    void* saddr; // Start address of the heap data pool

    size_t first_free_hint; // All the blocks befor this index are taken
};

int heap_create(struct heap* heap, void* ptr, void* end, struct heap_table* table);
void* heap_malloc(struct heap* heap, size_t size);
void heap_free(struct heap* heap, void* ptr);