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
    ESCAPE = 0x01,
    BACKSPACE = 0x0E,
    TAB = 0x0F,
    ENTER = 0x1C,
    SPACE = 0x20,
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
    RIGHT_CTRL = 0x9D,
    RIGHT_SHIFT = 0x36,
    RIGHT_ALT = 0xB8,
    LEFT_GUI = 0x5B,
    RIGHT_GUI = 0x5C,
    APPS = 0x5D,
    POWER = 0x5E,
    SLEEP = 0x5F,
    WAKE = 0x63,
    KEYPAD_ENTER = 0xE01C,  // Using the extended byte sequence
    KEYPAD_SLASH = 0xE035,
    KEYPAD_STAR = 0xE037,
    KEYPAD_MINUS = 0xE04A,
    KEYPAD_PLUS = 0xE04E,
    KEYPAD_DOT = 0xE053,
    KEYPAD_0 = 0xE052,
    KEYPAD_1 = 0xE04F,
    KEYPAD_2 = 0xE050,
    KEYPAD_3 = 0xE051,
    KEYPAD_4 = 0xE04B,
    KEYPAD_5 = 0xE04C,
    KEYPAD_6 = 0xE04D,
    KEYPAD_7 = 0xE047,
    KEYPAD_8 = 0xE048,
    KEYPAD_9 = 0xE049,
    HOME = 0xE047,
    CURSOR_UP = 0x48,
    PAGE_UP = 0xE049,
    CURSOR_LEFT = 0x4B,
    CURSOR_RIGHT = 0x4D,
    END = 0xE04F,
    CURSOR_DOWN = 0x50,
    PAGE_DOWN = 0xE051,
    INSERT = 0xE052,
    DELETE = 0xE053,
    PRINT_SCREEN = 0xE037,
    PAUSE = 0x45,
    MULTIMEDIA_PREVIOUS_TRACK = 0xE010,
    MULTIMEDIA_NEXT_TRACK = 0xE019,
    MULTIMEDIA_MUTE = 0xE020,
    MULTIMEDIA_CALCULATOR = 0xE021,
    MULTIMEDIA_PLAY = 0xE022,
    MULTIMEDIA_STOP = 0xE024,
    MULTIMEDIA_VOLUME_DOWN = 0xE02E,
    MULTIMEDIA_VOLUME_UP = 0xE030,
    MULTIMEDIA_WWW_HOME = 0xE032,
    MULTIMEDIA_WWW_SEARCH = 0xE065,
    MULTIMEDIA_WWW_FAVORITES = 0xE066,
    MULTIMEDIA_WWW_REFRESH = 0xE067,
    MULTIMEDIA_WWW_STOP = 0xE068,
    MULTIMEDIA_WWW_FORWARD = 0xE069,
    MULTIMEDIA_WWW_BACK = 0xE06A,
    MULTIMEDIA_MY_COMPUTER = 0xE06B,
    MULTIMEDIA_EMAIL = 0xE06C,
    MULTIMEDIA_MEDIA_SELECT = 0xE06D
};

// Keyboard scan code set
static const char keymap[256] = {
    0, 0x1, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', BACKSPACE,
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
static bool extended_key_sequence_1 = false; // Flags to track extended key sequence
static bool extended_key_sequence_2 = false; 

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
