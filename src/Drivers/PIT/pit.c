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

#include "pit.h"

volatile uint32_t ticks = 0;
volatile uint32_t frequency = 0;

void init_PIT(uint32_t freq) {
    frequency = freq;
    uint32_t divisor = __udivdi3(PIT_FREQUENCY, freq);

    outb(PIT_COMMAND, PIT_CMD_BINARY | PIT_CMD_MODE3);

    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));
}

void PIT_irq_handler(struct InterruptRegisters *r) {
    ticks++;
}

void install_PIT_irq() {
    irq_install_handler(0, PIT_irq_handler);
}

void wait(uint32_t tick){
    uint32_t end_ticks = ticks + tick;       
    while(ticks != end_ticks);
}