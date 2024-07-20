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
            case 0xe0:
            ps2_wait_output();
            switch (ps2_read_data())
            {
            case CURSOR_UP:
                if(cursor_y != 0){
                    cursor_y--;
                }
                update_cursor(cursor_x, cursor_y);
                break;
            case CURSOR_DOWN:
                if(cursor_y != 24){
                    cursor_y++;
                }
                update_cursor(cursor_x, cursor_y);
                break;
            case CURSOR_LEFT:
                if(cursor_x != 0){
                    cursor_x--;
                }
                update_cursor(cursor_x, cursor_y);
                break;
            case CURSOR_RIGHT:
                if(cursor_x != 79){
                    cursor_x++;
                }
                update_cursor(cursor_x, cursor_y);
                break;
            default:
                break;
            }
            break;
            default:
                ps2_handle_special(scan_code);
                if (keymap[scan_code]) {
                    key = keymap[scan_code];
                    new_key_press = true; // Set new key press flag
                    
                    switch (key) {
                        case ESCAPE:
                            key = 0x1b;
                            break;
                        default:
                            if(shift_pressed){
                                key = keycodes_in_uppercase(key);
                            }
                            break;
                    }
                }
                break;
        }
    }
}

void ps2_handle_special(unsigned char scancode) {
    // Handle key presses to update LED state
    switch (scancode) {
        case 0x3A:  // CapsLock pressed
            leds_state ^= PS2_LED_CAPS_LOCK;
            ps2_set_leds(leds_state);
            break;
        case 0x45:  // NumLock pressed
            leds_state ^= PS2_LED_NUM_LOCK;
            ps2_set_leds(leds_state);
            break;
        case 0x46:  // ScrollLock pressed
            leds_state ^= PS2_LED_SCROLL_LOCK;
            ps2_set_leds(leds_state);
            break;
        default:
            // Handle other key presses if necessary
            break;
    }
}

void ps2_set_leds(unsigned char leds) {
    // Send command to set keyboard LEDs
    ps2_write_command(PS2_CMD_SET_LED);
    ps2_write_data(leds);
}

uint8_t getch() {
    new_key_press = false; // Reset new key press flag
    return key;
}

bool kbhit() {
    return new_key_press;
}
