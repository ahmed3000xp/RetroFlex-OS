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

void main(uint32_t magic, struct multiboot_info* mb_info) {
    uint8_t kc = 0;

    printf("RetroFlex OS  Copyright (C) 2024 The Developer\n");
    printf("This program comes with ABSOLUTELY NO WARRANTY; for details type 'show w'\n");
    printf("This is free software, and you are welcome to redistribute it\n");
    printf("under certain conditions; type 'show c' for details.\n\n");

    dbg_printf("[%d] Initializing GDT\n",ticks);
    init_GDT();

    dbg_printf("[%d] Initializing Paging\n",ticks);
    init_paging();

    dbg_printf("[%d] Initializing IDT\n",ticks);
    init_IDT();

    dbg_printf("[%d] Initializing PIT\n",ticks);
    init_PIT(60);

    dbg_printf("[%d] Installing PIT IRQ\n",ticks);
    install_PIT_irq();

    dbg_printf("[%d] Initializing PS/2 Controller\n",ticks);
    ps2_init();

    dbg_printf("[%d] Installing PS/2 Controller IRQ\n",ticks);
    install_keyboard_irq();

    dbg_printf("[%d] Reading RTC\n",ticks);
    read_rtc();

    printf("Time %d:%d:%d\n", hour, minute, second);

    printf("Date %d/%d/%d\n", day, month, current_year);
    
    while(kc != 0x1b) {
        if(kbhit()) {
            kc = getch();
            putc(kc);
        }
    }
    
    printf("\nIt is safe to turn off or reset your PC\n");
    return;
}
