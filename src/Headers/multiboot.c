// Copyright (C) 2024 Ahmed
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "multiboot.h"

const char* multiboot_mem_type_2_text(uint32_t type){
    switch (type) {
        case MULTIBOOT_MEMORY_AVAILABLE: return "Available"; break;
        case MULTIBOOT_MEMORY_RESERVED: return "Reserved"; break;
        case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE: return "ACPI Reclaimable"; break;
        case MULTIBOOT_MEMORY_NVS: return "NVS"; break;
        case MULTIBOOT_MEMORY_BADRAM: return "Bad RAM"; break;
        default: return "Unknown"; break;
    }
}

void read_multiboot_header(uint32_t magic, struct multiboot_info* mb_info){
    if (magic != 0x2BADB002) {
        dbg_printf("Invalid magic number: 0x%x\n", magic);
        return;
    }

    if (mb_info->flags & 0x1) {
        uint32_t mem_lower = mb_info->mem_lower;
        uint32_t mem_upper = mb_info->mem_upper;
        dbg_printf("Lower memory: %u KB\n", mem_lower);
        dbg_printf("Upper memory: %u KB\n", mem_upper);
    }

    if (mb_info->flags & 0x40) {
        struct multiboot_mmap_entry* mmap = (struct multiboot_mmap_entry*)mb_info->mmap_addr;
        uint32_t mmap_length = mb_info->mmap_length;
        dbg_printf("Memory map:\n");
        for (struct multiboot_mmap_entry* entry = mmap;
             (uint8_t*)entry < (uint8_t*)mmap + mmap_length;
             entry = (struct multiboot_mmap_entry*)((uint8_t*)entry + entry->size + sizeof(entry->size))) {
            dbg_printf("Base address: 0x%x%x, Length: 0x%x%x, Type: %s (0x%x)\n",
                   entry->addr_high, entry->addr_low,
                   entry->len_high, entry->len_low,multiboot_mem_type_2_text(entry->type),
                   entry->type);
        }
    }
}