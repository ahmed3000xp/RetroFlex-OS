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

#include "../Headers/stdint.h"
#include "../Headers/util.h"
#include "../Drivers/VGA/vga.h"
#include "idt.h"

struct IDT_entry_struct IDT_entries[256];
struct IDT_ptr_struct IDT_ptr;

extern void IDT_flush(uint32_t);
extern void handle_div_zero();

void init_IDT(){
    IDT_ptr.limit = sizeof(struct IDT_entry_struct) * 256 - 1;
    IDT_ptr.base = (uint32_t) &IDT_entries;

    memset(&IDT_entries, 0, sizeof(struct IDT_entry_struct) * 256);

    //0x20 commands and 0x21 data
    //0xA0 commands and 0xA1 data
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    outb(0x21,0x04);
    outb(0xA1,0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    set_IDT_gate(0, (uint32_t)isr0,0x08, 0x8E);
    set_IDT_gate(1, (uint32_t)isr1,0x08, 0x8E);
    set_IDT_gate(2, (uint32_t)isr2,0x08, 0x8E);
    set_IDT_gate(3, (uint32_t)isr3,0x08, 0x8E);
    set_IDT_gate(4, (uint32_t)isr4, 0x08, 0x8E);
    set_IDT_gate(5, (uint32_t)isr5, 0x08, 0x8E);
    set_IDT_gate(6, (uint32_t)isr6, 0x08, 0x8E);
    set_IDT_gate(7, (uint32_t)isr7, 0x08, 0x8E);
    set_IDT_gate(8, (uint32_t)isr8, 0x08, 0x8E);
    set_IDT_gate(9, (uint32_t)isr9, 0x08, 0x8E);
    set_IDT_gate(10, (uint32_t)isr10, 0x08, 0x8E);
    set_IDT_gate(11, (uint32_t)isr11, 0x08, 0x8E);
    set_IDT_gate(12, (uint32_t)isr12, 0x08, 0x8E);
    set_IDT_gate(13, (uint32_t)isr13, 0x08, 0x8E);
    set_IDT_gate(14, (uint32_t)isr14, 0x08, 0x8E);
    set_IDT_gate(15, (uint32_t)isr15, 0x08, 0x8E);
    set_IDT_gate(16, (uint32_t)isr16, 0x08, 0x8E);
    set_IDT_gate(17, (uint32_t)isr17, 0x08, 0x8E);
    set_IDT_gate(18, (uint32_t)isr18, 0x08, 0x8E);
    set_IDT_gate(19, (uint32_t)isr19, 0x08, 0x8E);
    set_IDT_gate(20, (uint32_t)isr20, 0x08, 0x8E);
    set_IDT_gate(21, (uint32_t)isr21, 0x08, 0x8E);
    set_IDT_gate(22, (uint32_t)isr22, 0x08, 0x8E);
    set_IDT_gate(23, (uint32_t)isr23, 0x08, 0x8E);
    set_IDT_gate(24, (uint32_t)isr24, 0x08, 0x8E);
    set_IDT_gate(25, (uint32_t)isr25, 0x08, 0x8E);
    set_IDT_gate(26, (uint32_t)isr26, 0x08, 0x8E);
    set_IDT_gate(27, (uint32_t)isr27, 0x08, 0x8E);
    set_IDT_gate(28, (uint32_t)isr28, 0x08, 0x8E);
    set_IDT_gate(29, (uint32_t)isr29, 0x08, 0x8E);
    set_IDT_gate(30, (uint32_t)isr30, 0x08, 0x8E);
    set_IDT_gate(31, (uint32_t)isr31, 0x08, 0x8E);

    set_IDT_gate(32, (uint32_t)irq0, 0x08, 0x8E);
    set_IDT_gate(33, (uint32_t)irq1, 0x08, 0x8E);
    set_IDT_gate(34, (uint32_t)irq2, 0x08, 0x8E);
    set_IDT_gate(35, (uint32_t)irq3, 0x08, 0x8E);
    set_IDT_gate(36, (uint32_t)irq4, 0x08, 0x8E);
    set_IDT_gate(37, (uint32_t)irq5, 0x08, 0x8E);
    set_IDT_gate(38, (uint32_t)irq6, 0x08, 0x8E);
    set_IDT_gate(39, (uint32_t)irq7, 0x08, 0x8E);
    set_IDT_gate(40, (uint32_t)irq8, 0x08, 0x8E);
    set_IDT_gate(41, (uint32_t)irq9, 0x08, 0x8E);
    set_IDT_gate(42, (uint32_t)irq10, 0x08, 0x8E);
    set_IDT_gate(43, (uint32_t)irq11, 0x08, 0x8E);
    set_IDT_gate(44, (uint32_t)irq12, 0x08, 0x8E);
    set_IDT_gate(45, (uint32_t)irq13, 0x08, 0x8E);
    set_IDT_gate(46, (uint32_t)irq14, 0x08, 0x8E);
    set_IDT_gate(47, (uint32_t)irq15, 0x08, 0x8E);


    set_IDT_gate(128, (uint32_t)isr128, 0x08, 0x8E); //System calls
    set_IDT_gate(177, (uint32_t)isr177, 0x08, 0x8E); //System calls

    IDT_flush((uint32_t)&IDT_ptr);

}

void set_IDT_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags){

    IDT_entries[num].base_low = base & 0xFFFF;
    IDT_entries[num].base_high = (base >> 16) & 0xFFFF;
    IDT_entries[num].sel = sel;
    IDT_entries[num].always0 = 0;
    IDT_entries[num].flags = flags | 0x60;

}

unsigned char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Fault",
    "Machine Check", 
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_handler(struct InterruptRegisters* regs){
    if (regs->int_no < 32){
        switch(regs->int_no){
            case 0:
                handle_div_zero();
                break;
            default:
                printf(exception_messages[regs->int_no]);
                printf("Registers\nEAX = 0x%x EBX = 0x%x ECX = 0x%x EDX = 0x%x");
                printf("Exception! System Halted\n");
                for(;;);
                break;
        }
    }
    else{
        switch(regs->int_no){
            case 128:
                switch(regs->eax){
                    case 0:
                        putc((uint8_t)regs->ebx);
                        break;
                    case 1:
                        puts((const char *)regs->esi);
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
}

void *irq_routines[16] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
};

void irq_install_handler (int irq, void (*handler)(struct InterruptRegisters *r)){
    irq_routines[irq] = handler;
}

void irq_uninstall_handler(int irq){
    irq_routines[irq] = 0;
}

void irq_handler(struct InterruptRegisters* regs){
    void (*handler)(struct InterruptRegisters *regs);

    handler = irq_routines[regs->int_no - 32];

    if (handler){
        handler(regs);
    }

    if (regs->int_no >= 40){
        outb(0xA0, 0x20);
    }

    outb(0x20,0x20);
}
