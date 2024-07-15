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

#include "Drivers/CMOS/cmos.h"
#include "Drivers/PS2/ps2.h"
#include "Headers/multiboot.h"
#include "GDT/gdt.h"
#include "Paging/paging.h"

extern void test_ints();

void kmain(uint32_t magic, struct multiboot_info* mb_info) {
    uint8_t kc = 0;

    printf("Initializing GDT\n");
    init_GDT();

    printf("Initializing IDT\n");
    init_IDT();

    printf("Initializing PIT\n");
    init_PIT(60);

    printf("Installing PIT IRQ\n");
    install_PIT_irq();

    printf("Initializing PS/2 Controller\n");
    ps2_init();

    printf("Installing PS/2 Controller IRQ\n");
    install_keyboard_irq();

    printf("Creating Page Directory and Enabling Paging\n");
    enable_paging();

    printf("Reading RTC\n");
    read_rtc();

    printf("Time %d:%d:%d\n", hour, minute, second);

    printf("Date %d/%d/%d\n", day, month, current_year);
    while(kc != 0x1b){
        if(kbhit()){
            kc = getch();
            putc(kc);
        }
    }

    return;
}
