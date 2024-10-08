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

#include "ps2.h"

static bool shift_pressed = false;
static bool ctrl_pressed = false;
static bool alt_pressed = false;
static bool extended_key_sequence_1 = false; // Flags to track extended key sequence
static bool extended_key_sequence_2 = false; 

static bool numlock = false;
static bool scroll_lock = false;
static bool capslock = false;

static bool key_released = false;
static bool new_key_press = false;

static uint8_t key = 0;

// PS/2 initialization
void ps2_init() {
    // Disable interrupts
    disable_interrupts();
    
    // Enable keyboard
    ps2_write_command(PS2_CMD_ENABLE_KEYBOARD);
    scroll_lock = false;
    numlock = false;
    capslock = false;
    
    // Re-enable interrupts
    enable_interrupts();
}

// PS/2 write command
void ps2_write_command(uint8_t command) {
    outb(PS2_COMMAND_PORT, command);
}

// PS/2 read data
uint8_t ps2_read_data() {
    return inb(PS2_DATA_PORT);
}

void ps2_write_data(uint8_t data) {
    outb(PS2_DATA_PORT, data);
}

// Wait for PS/2 output buffer to be full
void ps2_wait_output() {
    while (!(inb(PS2_STATUS_PORT) & PS2_STATUS_OUTPUT_BUFFER));
}

// Keyboard interrupt handler
void keyboard_irq_handler(struct InterruptRegisters *r) {
    (void)r;
    uint8_t scan_code = ps2_read_data();
    process_scan_code(scan_code);
}

void install_keyboard_irq() {
    irq_install_handler(1, keyboard_irq_handler);
}

uint8_t keycodes_in_uppercase(uint8_t k){
    switch(k){
        case '1': k = '!'; break;
        case '2': k = '@'; break;
        case '3': k = '#'; break;
        case '4': k = '$'; break;
        case '5': k = '%'; break;
        case '6': k = '^'; break;
        case '7': k = '&'; break;
        case '8': k = '*'; break;
        case '9': k = '('; break;
        case '0': k = ')'; break;
        case '-': k = '_'; break;
        case '=': k = '+'; break;
        case 'q': k = 'Q'; break;
        case 'w': k = 'W'; break;
        case 'e': k = 'E'; break;
        case 'r': k = 'R'; break;
        case 't': k = 'T'; break;
        case 'y': k = 'Y'; break;
        case 'u': k = 'U'; break;
        case 'i': k = 'I'; break;
        case 'o': k = 'O'; break;
        case 'p': k = 'P'; break;
        case 'a': k = 'A'; break;
        case 's': k = 'S'; break;
        case 'd': k = 'D'; break;
        case 'f': k = 'F'; break;
        case 'g': k = 'G'; break;
        case 'h': k = 'H'; break;
        case 'j': k = 'J'; break;
        case 'k': k = 'K'; break;
        case 'l': k = 'L'; break;
        case 'z': k = 'Z'; break;
        case 'x': k = 'X'; break;
        case 'c': k = 'C'; break;
        case 'v': k = 'V'; break;
        case 'b': k = 'B'; break;
        case 'n': k = 'N'; break;
        case 'm': k = 'M'; break;
        case '[': k = '{'; break;
        case ']': k = '}'; break;
        case '\\': k = '|'; break;
        case ';': k = ':'; break;
        case '\'': k = '"'; break;
        case ',': k = '<'; break;
        case '.': k = '>'; break;
        case '/': k = '?'; break;
    }
    return k;
}

void process_scan_code(uint8_t scan_code) {
    // Check for extended key sequence (0xE0)
    if (scan_code == 0xE0) {
        extended_key_sequence_1 = true;
        return; // Wait for the next byte
    }

    else if (scan_code == 0xE1) {
        extended_key_sequence_2 = true;
        return; // Wait for the next byte
    }

    if (extended_key_sequence_1) {
        scan_code = ps2_read_data();
        extended_key_sequence_1 = false; // Reset the flag

        switch (scan_code) {
            case CURSOR_UP:
                key = scan_code;
                break;
            case CURSOR_DOWN:
                key = scan_code;
                break;
            case CURSOR_LEFT:
                key = scan_code;
                break;
            case CURSOR_RIGHT:
                key = scan_code;
                break;
            default:
                break;
        }
    } else {
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
                        key = (uint8_t)keymap[scan_code];
                        new_key_press = true; // Set new key press flag

                        switch (key) {
                            case ESCAPE:
                                key = 0x1b;
                                break;
                            default:
                                if (shift_pressed) {
                                    key = keycodes_in_uppercase(key);
                                }
                                break;
                        }
                    }
                    break;
            }
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