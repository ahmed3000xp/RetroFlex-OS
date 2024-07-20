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

#include "../../Headers/stdint.h"
#include "../../Headers/util.h"

<<<<<<< HEAD
extern uint16_t* text_memory;
extern uint8_t* vga_memory;
=======
extern volatile uint16_t* text_memory;
extern volatile uint8_t* vga_memory;

extern volatile uint32_t cursor_x;
extern volatile uint32_t cursor_y;
extern volatile uint8_t current_color;
>>>>>>> 28d8df4 (Just interagting with Vscode)
#define DEFAULT_COLOR 7

void wait_for_retrace();
void set_palette(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
bool set_gpu_mode(uint8_t mode);
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void disable_cursor();
void update_cursor(int x, int y);
uint16_t get_cursor_position(void);
void clear_screen();
void scroll_up();
void putc(char c);
void puts(const char *s);
void printf(const char* fmt, ...);
void print_buffer(const char* msg, const void* buffer, uint32_t count);