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

#include "stdint.h"

void memset(void *dest, char val, uint32_t count);
void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);
void outw(uint16_t port, uint16_t value);
uint16_t inw(uint16_t port);
void outdw(uint16_t port, uint32_t value);
uint32_t indw(uint16_t port);
void disable_interrupts();
void enable_interrupts();
unsigned long long __udivdi3(unsigned long long a, unsigned long long b);
unsigned long long __umoddi3(unsigned long long a, unsigned long long b);

#define CEIL_DIV(a,b) (((a + b) - 1)/b)