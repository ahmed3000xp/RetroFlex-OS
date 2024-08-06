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

#pragma once

#include "../../Headers/stdint.h"
#include "../../Headers/util.h"
#include "../../IDT/idt.h"

#define PIT_CHANNEL0 0x40
#define PIT_COMMAND  0x43

#define PIT_CMD_BINARY  0x00
#define PIT_CMD_MODE3   0x06

#define PIT_FREQUENCY 1193182

extern volatile uint32_t ticks;
extern volatile uint32_t frequency;

void init_PIT(uint32_t frequency);
void PIT_irq_handler(struct InterruptRegisters *r);
void install_PIT_irq();
void wait(uint32_t tick);