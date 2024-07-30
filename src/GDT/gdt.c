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

#include "gdt.h"
#include "../Headers/util.h"


extern void GDT_flush(uint32_t);
extern void TSS_flush();

struct GDT_entry_struct GDT_entries[6];
struct GDT_ptr_struct GDT_ptr;
struct TSS_entry_struct TSS_entry;

void init_GDT(){
    GDT_ptr.limit = (sizeof(struct GDT_entry_struct) * 6) - 1;
    GDT_ptr.base = (uint32_t)&GDT_entries;

    set_GDT_gate(0,0,0,0,0); //Null segment
    set_GDT_gate(1,0,0xffffffff, 0x9a, 0xcf); //Kernel code segment
    set_GDT_gate(2,0,0xffffffff, 0x92, 0xcf); //Kernel data segment
    set_GDT_gate(3,0,0xffffffff, 0xfa, 0xcf); //User code segment
    set_GDT_gate(4,0,0xffffffff, 0xf2, 0xcf); //User data segment
    write_TSS(5,0x10, 0x0);

    GDT_flush((uint32_t)&GDT_ptr);
    TSS_flush();
}

void write_TSS(uint32_t num, uint16_t ss0, uint32_t esp0){
    uint32_t base = (uint32_t) &TSS_entry;
    uint32_t limit = base + sizeof(TSS_entry);

    set_GDT_gate(num, base, limit, 0xe9, 0x00);
    memset(&TSS_entry, 0, sizeof(TSS_entry));

    TSS_entry.ss0 = ss0;
    TSS_entry.esp0 = esp0;

    TSS_entry.cs = 0x08 | 0x3;
    TSS_entry.ss = TSS_entry.ds = TSS_entry.es = TSS_entry.fs = TSS_entry.gs = 0x10 | 0x3;
}

void set_GDT_gate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran){

    GDT_entries[num].base_low = (base & 0xFFFF);
    GDT_entries[num].base_middle = (base >> 16) & 0xFF;
    GDT_entries[num].base_high = (base >> 24) & 0xFF;

    GDT_entries[num].limit = (limit & 0xFFFF);
    GDT_entries[num].flags = (limit >> 16) & 0x0F;
    GDT_entries[num].flags |= (gran & 0xF0);

    GDT_entries[num].access = access;

}
