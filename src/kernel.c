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

#include "Drivers/ATA/ata.h"
#include "Headers/multiboot.h"
#include "GDT/gdt.h"
#include "Paging/paging.h"

extern void test_ints();

void main(uint32_t magic, struct multiboot_info* mb_info) {
    uint8_t kc = 0;
    struct DriveInfo drive_info;
    char buffer[512];

    printf("RetroFlex OS  Copyright (C) 2024 Ahmed\n");
    printf("This program comes with ABSOLUTELY NO WARRANTY; for details type 'show w'\n");
    printf("This is free software, and you are welcome to redistribute it\n");
    printf("under certain conditions; type 'show c' for details.\n\n");

    dbg_printf("RetroFlex OS  Copyright (C) 2024 Ahmed\n");
    dbg_printf("This program comes with ABSOLUTELY NO WARRANTY; for details type 'show w'\n");
    dbg_printf("This is free software, and you are welcome to redistribute it\n");
    dbg_printf("under certain conditions; type 'show c' for details.\n\n");

    dbg_printf("[%d] Initializing GDT\n",ticks);
    init_GDT();

    dbg_printf("[%d] Initializing Paging\n",ticks);
    init_paging();

    dbg_printf("[%d] Initializing IDT\n",ticks);
    init_IDT();

    dbg_printf("[%d] Initializing PIT\n",ticks);
    init_PIT(1000);

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

    dbg_printf("[%d] Checking ATA controller\n", ticks);
    if(!check_ata_controller())
	    dbg_printf("[%d] Didn't Find ATA controller\n", ticks);

    identify_drive(&drive_info, false, false);
    read_sector(0, buffer, 512, &drive_info);
    print_drive_info(&drive_info);
    for(int i = 0; i < 512; i++){
        printf("%x ", buffer[i]);
    }
    
    return;
}
