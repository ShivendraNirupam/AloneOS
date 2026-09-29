# Page Directory Structure (x86, 32-bit Paging)

Each entry in the Page Directory is **32 bits (4 bytes)** wide. A Page Directory holds **1024 entries**, and each entry points to a Page Table (or, when `S = 1`, directly to a 4 MB page).

## Bit Layout

```
 31                                12 11     9   8   7   6   5   4   3   2   1   0
+------------------------------------+--------+---+---+---+---+---+---+---+---+---+
|   Page Table 4-KB Aligned Address  | Avail. | G | S | 0 | A | D | W | U | R | P |
+------------------------------------+--------+---+---+---+---+---+---+---+---+---+
```

| Bits  | Field | Short Name              |
|-------|-------|-------------------------|
| 31–12 | Addr  | Page Table Address      |
| 11–9  | Avail | Available for OS use    |
| 8     | G     | Global (ignored in PDE) |
| 7     | S     | Page Size               |
| 6     | 0     | Reserved (set to 0)     |
| 5     | A     | Accessed                |
| 4     | D     | Cache Disable (PCD)     |
| 3     | W     | Write-Through (PWT)     |
| 2     | U     | User / Supervisor       |
| 1     | R     | Read / Write            |
| 0     | P     | Present                 |

## Field Descriptions

| Name | Description |
|------|-------------|
| **Page Table 4-KB Aligned Address** | The physical 4096-byte aligned address of the start of a page table. Because it is 4 KB aligned, the lower 12 bits are always zero, so only bits 31–12 are stored. |
| **Avail.** | Free for the kernel to use for its own bookkeeping. The CPU ignores these bits. |
| **S** (Page Size) | Set to `0` for 4 KB pages, set to `1` for 4 MB pages. |
| **A** (Accessed) | Set to `1` by the CPU when this page is accessed. |
| **D** (Cache Disable) | Set to `1` to disable caching for this page. |
| **W** (Write-Through) | If set, write-through caching is enabled. If not set, write-back caching is used instead. |
| **U** (User/Supervisor) | If set, the page can be accessed by all privilege rings. If not set, only supervisor rings can access it. |
| **R** (Read/Write) | If set, the page is readable and writable. If not set, the page is read-only. Note: the `WP` bit in the `CR0` register controls whether the supervisor can still write to read-only pages. |
| **P** (Present) | Set if the page exists in real memory. If the page is not available, the kernel should clear this bit. Any access to a non-present page triggers a **Page Fault**, which the kernel is expected to handle. |

## Flag Values (C)

```c
#define PAGING_IS_PRESENT        0b00000001  // P
#define PAGING_IS_WRITEABLE      0b00000010  // R
#define PAGING_ACCESS_FROM_ALL   0b00000100  // U
#define PAGING_WRITE_THROUGH     0b00001000  // W
#define PAGING_CACHE_DISABLED    0b00010000  // D
#define PAGING_ACCESSED          0b00100000  // A
#define PAGING_PAGE_SIZE_4MB     0b10000000  // S
```

### Building an entry

```c
uint32_t entry = (page_table_address & 0xFFFFF000)
               | PAGING_IS_PRESENT
               | PAGING_IS_WRITEABLE
               | PAGING_ACCESS_FROM_ALL;
```

## Notes

- In this layout, **D means Cache Disable (PCD)**, not "Dirty". The Dirty bit only exists in Page Table entries (and in 4 MB page directory entries).
- The address field must be 4 KB aligned; masking with `0xFFFFF000` keeps the flag bits from corrupting it.
- Paging is enabled by loading the Page Directory address into `CR3` and setting the `PG` bit (bit 31) in `CR0`.