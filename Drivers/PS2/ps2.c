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

#include "ps2.h"

// PS/2 initialization
void ps2_init() {
    // Disable interrupts
    disable_interrupts();
    
    // Enable keyboard
    ps2_write_command(PS2_CMD_ENABLE_KEYBOARD);
    
    // Re-enable interrupts
    enable_interrupts();
}

// PS/2 write command
void ps2_write_command(uint8_t command) {
    ps2_wait_output();
    outb(PS2_COMMAND_PORT, command);
}

// PS/2 read data
uint8_t ps2_read_data() {
    ps2_wait_output();
    return inb(PS2_DATA_PORT);
}

// Wait for PS/2 output buffer to be full
void ps2_wait_output() {
    while (!(inb(PS2_STATUS_PORT) & PS2_STATUS_OUTPUT_BUFFER));
}

// Keyboard interrupt handler
void keyboard_irq_handler(struct InterruptRegisters *r) {
    uint8_t scan_code = ps2_read_data();
    process_scan_code(scan_code);
}

void install_keyboard_irq() {
    irq_install_handler(1, keyboard_irq_handler);
}

// Process keyboard scan code
void process_scan_code(uint8_t scan_code) {
    key_released = scan_code & 0x80;
    scan_code &= 0x7F; // Mask out the release bit
    
    if (key_released) {
        switch (scan_code) {
            case 0x2A: shift_pressed = false; break; // Left Shift
            case 0x36: shift_pressed = false; break; // Right Shift
            case 0x1D: ctrl_pressed = false; break;  // Ctrl
            case 0x38: alt_pressed = false; break;   // Alt
        }
    } else {
        switch (scan_code) {
            case 0x2A: shift_pressed = true; break; // Left Shift
            case 0x36: shift_pressed = true; break; // Right Shift
            case 0x1D: ctrl_pressed = true; break;  // Ctrl
            case 0x38: alt_pressed = true; break;   // Alt
            default:
                if (keymap[scan_code]) {
                    key = keymap[scan_code];
                    new_key_press = true; // Set new key press flag
                    
                    switch (key) {
                        case ESCAPE:
                            key = 0x1b;
                            break;
                        case CAPSLOCK:
                            capslock = !capslock;
                            outb(PS2_COMMAND_PORT, 0xed);
                            outb(PS2_DATA_PORT, capslock << 2 | numlock << 1 | scroll_lock);
                            key = 0;
                            break;
                        case NUMLOCK:
                            numlock = !numlock;
                            outb(PS2_COMMAND_PORT, 0xed);
                            outb(PS2_DATA_PORT, capslock << 2 | numlock << 1 | scroll_lock);
                            key = 0;
                            break;
                        case SCROLLLOCK:
                            scroll_lock = !scroll_lock;
                            outb(PS2_COMMAND_PORT, 0xed);
                            outb(PS2_DATA_PORT, capslock << 2 | numlock << 1 | scroll_lock);
                            key = 0;
                            break;
                        default:
                            break;
                    }
                }
                break;
        }
    }
}

uint8_t getch() {
    new_key_press = false; // Reset new key press flag
    return key;
}

bool kbhit() {
    return new_key_press;
}