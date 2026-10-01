#pragma once
#include <stdint.h>

using ALONEOS_DISK_TYPE = unsigned int;

constexpr uint32_t ALONEOS_DISK_TYPE_REAL = 0; // Represent real physical hard disk

struct disk {
    ALONEOS_DISK_TYPE type;
    int sector_size;
};

void disk_search_and_init();
struct disk* disk_get(int index);
int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buff);