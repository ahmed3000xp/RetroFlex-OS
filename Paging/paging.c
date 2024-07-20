// Copyright (C) 2024 The RetroFlex OS Project
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

#include "paging.h"

page_entry_t page_directory[PAGE_DIRECTORY_ENTRIES] __attribute__((aligned(PAGE_SIZE)));
page_entry_t page_table[PAGE_TABLE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));

// Function to set page directory and page table entries
void set_page_directory_entry(uint32_t index, uint32_t base_addr, uint32_t flags) {
    page_directory[index] = (base_addr & 0xFFFFF000) | (flags & 0xFFF);
}

void set_page_table_entry(uint32_t index, uint32_t base_addr, uint32_t flags) {
    page_table[index] = (base_addr & 0xFFFFF000) | (flags & 0xFFF);
}

// Function to map a virtual address to a physical address with specified flags
void map_page(uint32_t physical_addr, uint32_t virtual_addr, uint32_t flags) {
    uint32_t dir_index = (virtual_addr >> 22) & 0x3FF;
    uint32_t table_index = (virtual_addr >> 12) & 0x3FF;

    // Set up the page table to point to the physical address with specified flags
    set_page_table_entry(table_index, physical_addr, flags);
}

// Function to initialize paging
void init_paging() {
    // Clear the page directory and page table
    memset(page_directory, 0, sizeof(page_directory));
    memset(page_table, 0, sizeof(page_table));

    // Identity map the first 4MB of memory (0x00000000 to 0x00400000)
    for (uint32_t i = 0; i < PAGE_TABLE_ENTRIES; i++) {
        set_page_table_entry(i, i * PAGE_SIZE, 0x003); // Present, RW
    }

    // Map the page table into the page directory
    set_page_directory_entry(0, (uint32_t)page_table, 0x003); // Present, RW

    // Map virtual address 0xc0000000 to physical address 0x00100000
    map_page(0x00100000, 0xc0000000, 0x007); // Present, RW, U/S

    // Enable paging
    enable_paging((uint32_t)page_directory);
}