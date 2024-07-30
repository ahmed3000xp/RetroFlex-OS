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

#include "../Headers/stdint.h"
#include "../Headers/util.h"

#define PAGE_SIZE 4096
#define PAGE_DIRECTORY_ENTRIES 1024
#define PAGE_TABLE_ENTRIES 1024

typedef uint32_t page_entry_t;

void set_page_directory_entry(uint32_t index, uint32_t base_addr, uint32_t flags);
void set_page_table_entry(uint32_t index, uint32_t base_addr, uint32_t flags);
void map_page(uint32_t physical_addr, uint32_t virtual_addr, uint32_t flags);
page_entry_t* get_page_dir_loc();
void init_paging();
extern void enable_paging(uint32_t page_directory);
