#pragma once
#include "stdint.h"
#include "stddef.h"

namespace paging {
    inline constexpr uint8_t PAGING_IS_PRESENT      = 1u << 0;
    inline constexpr uint8_t PAGING_IS_WRITEABLE    = 1u << 1;
    inline constexpr uint8_t PAGING_ACCESS_FROM_ALL = 1u << 2;
    inline constexpr uint8_t PAGING_WRITE_THROUGH   = 1u << 3;
    inline constexpr uint8_t PAGING_CACHE_DISABLED  = 1u << 4;
    inline constexpr uint8_t PAGING_ACCESSED        = 1u << 5;
    inline constexpr uint8_t PAGING_PAGE_SIZE_4MB   = 1u << 7;

    inline constexpr size_t PAGING_TOTAL_ENTRIES_PER_TABLE = 1024;
    inline constexpr size_t PAGING_PAGE_SIZE               = 4096;
}

struct paging_4gb_chunk {
    uint32_t* directory_entry;

};

uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk);
struct paging_4gb_chunk* paging_new_4gb(uint8_t flags);
void paging_switch(uint32_t* directory);
extern "C" void enable_paging();