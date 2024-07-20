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
#include "../Speaker/speaker.h"
#include "../VGA/vga.h"

// PS/2 controller ports
#define PS2_DATA_PORT 0x60
#define PS2_STATUS_PORT 0x64
#define PS2_COMMAND_PORT 0x64

// PS/2 controller commands
#define PS2_CMD_READ_CONFIG 0x20
#define PS2_CMD_WRITE_CONFIG 0x60
#define PS2_CMD_ENABLE_KEYBOARD 0xAE
#define PS2_CMD_SET_LED 0xED

#define PS2_LED_NUM_LOCK 0x01
#define PS2_LED_CAPS_LOCK 0x02
#define PS2_LED_SCROLL_LOCK 0x04

// PS/2 controller status
#define PS2_STATUS_OUTPUT_BUFFER 0x01

// Special keys
enum SpecialKeys {
    ESCAPE = 0x1b,
    BACKSPACE = '\b',
    TAB = '\t',
    ENTER = '\n',
    SPACE = ' ',
    CAPSLOCK = 0x3A,
    F1 = 0x3B,
    F2 = 0x3C,
    F3 = 0x3D,
    F4 = 0x3E,
    F5 = 0x3F,
    F6 = 0x40,
    F7 = 0x41,
    F8 = 0x42,
    F9 = 0x43,
    F10 = 0x44,
    F11 = 0x57,
    F12 = 0x58,
    NUMLOCK = 0x45,
    SCROLLLOCK = 0x46,
    LEFT_CTRL = 0x1D,
    LEFT_SHIFT = 0x2A,
    LEFT_ALT = 0x38,
    RIGHT_CTRL = 0x1D | 0xE0,
    RIGHT_SHIFT = 0x36,
    RIGHT_ALT = 0x38 | 0xE0,
    LEFT_GUI = 0x5B | 0xE0,
    RIGHT_GUI = 0x5C | 0xE0,
    APPS = 0x5D | 0xE0,
    POWER = 0x5E | 0xE0,
    SLEEP = 0x5F | 0xE0,
    WAKE = 0x63 | 0xE0,
    KEYPAD_ENTER = 0x1C | 0xE0,
    KEYPAD_SLASH = 0x35 | 0xE0,
    KEYPAD_STAR = 0x37,
    KEYPAD_MINUS = 0x4A,
    KEYPAD_PLUS = 0x4E,
    KEYPAD_DOT = 0x53,
    KEYPAD_0 = 0x52,
    KEYPAD_1 = 0x4F,
    KEYPAD_2 = 0x50,
    KEYPAD_3 = 0x51,
    KEYPAD_4 = 0x4B,
    KEYPAD_5 = 0x4C,
    KEYPAD_6 = 0x4D,
    KEYPAD_7 = 0x47,
    KEYPAD_8 = 0x48,
    KEYPAD_9 = 0x49,
    HOME = 0x47 | 0xE0,
    CURSOR_UP = 0x48 | 0xE0,
    PAGE_UP = 0x49 | 0xE0,
    CURSOR_LEFT = 0x4B | 0xE0,
    CURSOR_RIGHT = 0x4D | 0xE0,
    END = 0x4F | 0xE0,
    CURSOR_DOWN = 0x50 | 0xE0,
    PAGE_DOWN = 0x51 | 0xE0,
    INSERT = 0x52 | 0xE0,
    DELETE = 0x53 | 0xE0,
    PRINT_SCREEN = 0x37 | 0xE0,
    PAUSE = 0x45 | 0xE1,
    MULTIMEDIA_PREVIOUS_TRACK = 0x10 | 0xE0,
    MULTIMEDIA_NEXT_TRACK = 0x19 | 0xE0,
    MULTIMEDIA_MUTE = 0x20 | 0xE0,
    MULTIMEDIA_CALCULATOR = 0x21 | 0xE0,
    MULTIMEDIA_PLAY = 0x22 | 0xE0,
    MULTIMEDIA_STOP = 0x24 | 0xE0,
    MULTIMEDIA_VOLUME_DOWN = 0x2E | 0xE0,
    MULTIMEDIA_VOLUME_UP = 0x30 | 0xE0,
    MULTIMEDIA_WWW_HOME = 0x32 | 0xE0,
    MULTIMEDIA_WWW_SEARCH = 0x40 | 0xE0,
    MULTIMEDIA_WWW_FAVORITES = 0x48 | 0xE0,
    MULTIMEDIA_WWW_REFRESH = 0x50 | 0xE0,
    MULTIMEDIA_WWW_STOP = 0x68 | 0xE0,
    MULTIMEDIA_WWW_FORWARD = 0x6A | 0xE0,
    MULTIMEDIA_WWW_BACK = 0x6C | 0xE0,
    MULTIMEDIA_MY_COMPUTER = 0x6F | 0xE0,
    MULTIMEDIA_EMAIL = 0x73 | 0xE0,
    MULTIMEDIA_MEDIA_SELECT = 0x74 | 0xE0
};

// Keyboard scan code set
static const char keymap[256] = {
    0, 0x1b, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', BACKSPACE,
    TAB, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', ENTER, 0, 'a',
    's', 'd', 'f', 'g', 'h', 'j', 'k','l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c',
    'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, SPACE, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0,
    '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static bool shift_pressed = false;
static bool ctrl_pressed = false;
static bool alt_pressed = false;

static bool numlock = false;
static bool scroll_lock = false;
static bool capslock = false;

static bool key_released = false;
static bool new_key_press = false;

static uint8_t key = 0;
static uint8_t leds_state = 0;

// Function prototypes
void ps2_init();
void keyboard_irq_handler(struct InterruptRegisters *r);
void install_keyboard_irq();
void ps2_wait_output();
uint8_t ps2_read_data();
void ps2_write_data(uint8_t data);
void ps2_write_command(uint8_t command);
uint8_t keycodes_in_uppercase(uint8_t k);
void process_scan_code(uint8_t scan_code);
void ps2_handle_special(unsigned char scancode);
void ps2_set_leds(unsigned char leds);
uint8_t getch();
bool kbhit();
