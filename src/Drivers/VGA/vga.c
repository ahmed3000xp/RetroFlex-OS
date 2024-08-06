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

#include "vga.h"
#include <stdarg.h>

volatile uint16_t *text_memory = (uint16_t *)0xb8000;
volatile uint8_t *vga_memory = (uint8_t *)0xa0000;

uint32_t width = 80;
uint32_t height = 25;
volatile uint32_t cursor_x = 0;
volatile uint32_t cursor_y = 0;
volatile uint8_t current_color = DEFAULT_COLOR;

uint8_t current_mode = 0x3;
uint8_t tap_len = 4;

void wait_for_retrace() {
    while ((inb(0x3DA) & 0x08));
    while (!(inb(0x3DA) & 0x08));
}

void set_palette(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
    outb(0x3C8, index);  // Set the color index
    outb(0x3C9, r);      // Set the red component
    outb(0x3C9, g);      // Set the green component
    outb(0x3C9, b);      // Set the blue component
}

bool set_gpu_mode(uint8_t mode) {
    bool valid = false;
    switch(mode){
        case 0x13:
            //NOT working, it makes the GPU crash
            disable_interrupts();

            outb(0x3C2, 0x63);        
            outw(0x3D4, 0x0E11);      
    
            outb(0x3C4, 0x00); outb(0x3C5, 0x03);  
            outb(0x3C4, 0x01); outb(0x3C5, 0x01);  
            outb(0x3C4, 0x02); outb(0x3C5, 0x0F);  
            outb(0x3C4, 0x03); outb(0x3C5, 0x00);  
            outb(0x3C4, 0x04); outb(0x3C5, 0x0E);

            outb(0x3D4, 0x11); outb(0x3D5, inb(0x3D5) & 0x7F);  
            outb(0x3D4, 0x00); outb(0x3D5, 0x5F);
            outb(0x3D4, 0x01); outb(0x3D5, 0x4F);
            outb(0x3D4, 0x02); outb(0x3D5, 0x50);
            outb(0x3D4, 0x03); outb(0x3D5, 0x82);
            outb(0x3D4, 0x04); outb(0x3D5, 0x55);
            outb(0x3D4, 0x05); outb(0x3D5, 0x81);
            outb(0x3D4, 0x06); outb(0x3D5, 0xBF);
            outb(0x3D4, 0x07); outb(0x3D5, 0x1F);
            outb(0x3D4, 0x08); outb(0x3D5, 0x00);
            outb(0x3D4, 0x09); outb(0x3D5, 0x41);
            outb(0x3D4, 0x0A); outb(0x3D5, 0x00);
            outb(0x3D4, 0x0B); outb(0x3D5, 0x00);
            outb(0x3D4, 0x0C); outb(0x3D5, 0x00);
            outb(0x3D4, 0x0D); outb(0x3D5, 0x00);
            outb(0x3D4, 0x0E); outb(0x3D5, 0x00);
            outb(0x3D4, 0x0F); outb(0x3D5, 0x00);
            outb(0x3D4, 0x10); outb(0x3D5, 0x9C);
            outb(0x3D4, 0x11); outb(0x3D5, 0x8E);
            outb(0x3D4, 0x12); outb(0x3D5, 0x8F);
            outb(0x3D4, 0x13); outb(0x3D5, 0x28);
            outb(0x3D4, 0x14); outb(0x3D5, 0x40);
            outb(0x3D4, 0x15); outb(0x3D5, 0x96);
            outb(0x3D4, 0x16); outb(0x3D5, 0xB9);
            outb(0x3D4, 0x17); outb(0x3D5, 0xA3);

            outb(0x3CE, 0x00); outb(0x3CF, 0x00);
            outb(0x3CE, 0x01); outb(0x3CF, 0x00);
            outb(0x3CE, 0x02); outb(0x3CF, 0x00);
            outb(0x3CE, 0x03); outb(0x3CF, 0x00);
            outb(0x3CE, 0x04); outb(0x3CF, 0x00);
            outb(0x3CE, 0x05); outb(0x3CF, 0x40);
            outb(0x3CE, 0x06); outb(0x3CF, 0x05);
            outb(0x3CE, 0x07); outb(0x3CF, 0x0F);
            outb(0x3CE, 0x08); outb(0x3CF, 0xFF);

            for (uint8_t i = 0; i < 16; i++) {
                outb(0x3C0, i); outb(0x3C0, i);
            }
            for (uint8_t i = 16; i < 32; i++) {
                outb(0x3C0, i); outb(0x3C0, 0);
            }

            outb(0x3C0, 0x20);

            // Set the VGA Mode Register to Mode 0x13
            outb(0x03D4, 0x0E); outb(0x03D5, 0x63);  // Set Mode Register to 0x13

            memset((void*)0xA0000, 0, 320 * 200);

            enable_interrupts();
            width = 320;
            height = 200;
            current_mode = mode;
            valid = true;
            break;
    }
    return valid;
}

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void disable_cursor() {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void update_cursor(uint32_t x, uint32_t y) {
    uint32_t pos = y * width + x;

    // Output low byte of pos
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));

    // Output high byte of pos
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}
uint16_t get_cursor_position(void) {
    uint16_t pos = 0;
    outb(0x3D4, 0x0F);
    pos |= inb(0x3D5);
    outb(0x3D4, 0x0E);
    pos |= ((uint16_t)inb(0x3D5)) << 8;
    return pos;
}

void clear_screen() {
    if(current_mode == 0x3){
        uint16_t blank = ' ' | (current_color << 8); 
        for (uint32_t y = 0; y < height; y++) {
            for (uint32_t x = 0; x < width; x++) {
                text_memory[x + y * width] = blank;
            }
        }
        cursor_x = 0;
        cursor_y = 0;
        update_cursor(cursor_x, cursor_y);
    }
    if(current_mode == 0x13){
        for(uint32_t y = 0; y < width; y++){
            for(uint32_t x = 0; x < height; x++){
                vga_memory[x + y * width] = 0;
            }
        }
    }
}

void scroll_up() {
    for (uint32_t row = 1; row < height; ++row) {
        for (uint32_t col = 0; col < width; ++col) {
            text_memory[col + (row - 1) * width] = text_memory[col + row * width];
        }
    }

    uint16_t blank = ' ' | (current_color << 8);
    for (uint32_t col = 0; col < width; ++col) {
        text_memory[col + (height - 1) * width] = blank;
    }
}

void putc(char c) {
    switch (c) {
        case '\r':
            cursor_x = 0;
            break;
        case '\n':
            if (cursor_y == height - 1) {
                scroll_up();
            } else {
                cursor_y++;
            }
            cursor_x = 0;
            break;
        case '\t':
            for(char i = 0; i < tap_len; i++){
                cursor_x++;
                if (cursor_x >= width) {
                    cursor_x = 0;
                    cursor_y++;
                }
            }
            break;
        case '\b':
            if (cursor_x > 0) {
                cursor_x--;
            } else if (cursor_y > 0) {
                cursor_y--;
                cursor_x = width - 1;
            }
            break;
        default:
            text_memory[cursor_x + cursor_y * width] = (uint8_t)c | (current_color << 8);
            cursor_x++;
            if (cursor_x >= width) {
                cursor_x = 0;
                cursor_y++;
                if (cursor_y >= height) {
                    scroll_up();
                    cursor_y = height - 1;
                }
            }
            break;
    }
    update_cursor(cursor_x, cursor_y);
}

void puts(const char *s){
    while(*s){
        putc(*s);
        s++;
    }
}

const char g_HexChars[] = "0123456789abcdef";

void printf_unsigned(unsigned long long number, int radix)
{
    char buffer[32];
    int pos = 0;

    // convert number to ASCII
    do 
    {
        unsigned long long rem = number % (unsigned long long)radix;
        number /= (unsigned long long)radix;
        buffer[pos++] = g_HexChars[rem];
    } while (number > 0);

    // print number in reverse order
    while (--pos >= 0)
        putc(buffer[pos]);
}

void printf_signed(long long number, int radix)
{
    if (number < 0)
    {
        putc('-');
        printf_unsigned((long long unsigned)-number, radix);
    }
    else printf_unsigned((long long unsigned)number, radix);
}

#define PRINTF_STATE_NORMAL         0
#define PRINTF_STATE_LENGTH         1
#define PRINTF_STATE_LENGTH_SHORT   2
#define PRINTF_STATE_LENGTH_LONG    3
#define PRINTF_STATE_SPEC           4

#define PRINTF_LENGTH_DEFAULT       0
#define PRINTF_LENGTH_SHORT_SHORT   1
#define PRINTF_LENGTH_SHORT         2
#define PRINTF_LENGTH_LONG          3
#define PRINTF_LENGTH_LONG_LONG     4

void printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int state = PRINTF_STATE_NORMAL;
    int length = PRINTF_LENGTH_DEFAULT;
    int radix = 10;
    bool sign = false;
    bool number = false;

    while (*fmt)
    {
        switch (state)
        {
            case PRINTF_STATE_NORMAL:
                switch (*fmt)
                {
                    case '%':   state = PRINTF_STATE_LENGTH;
                                break;
                    default:    putc(*fmt);
                                break;
                }
                break;

            case PRINTF_STATE_LENGTH:
                switch (*fmt)
                {
                    case 'h':   length = PRINTF_LENGTH_SHORT;
                                state = PRINTF_STATE_LENGTH_SHORT;
                                break;
                    case 'l':   length = PRINTF_LENGTH_LONG;
                                state = PRINTF_STATE_LENGTH_LONG;
                                break;
                    default:    goto PRINTF_STATE_SPEC_;
                }
                break;

            case PRINTF_STATE_LENGTH_SHORT:
                if (*fmt == 'h')
                {
                    length = PRINTF_LENGTH_SHORT_SHORT;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_LENGTH_LONG:
                if (*fmt == 'l')
                {
                    length = PRINTF_LENGTH_LONG_LONG;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_SPEC:
            PRINTF_STATE_SPEC_:
                switch (*fmt)
                {
                    case 'c':   putc((char)va_arg(args, int));
                                break;

                    case 's':   
                                puts(va_arg(args, const char*));
                                break;

                    case '%':   putc('%');
                                break;

                    case 'd':
                    case 'i':   radix = 10; sign = true; number = true;
                                break;

                    case 'u':   radix = 10; sign = false; number = true;
                                break;

                    case 'X':
                    case 'x':
                    case 'p':   radix = 16; sign = false; number = true;
                                break;

                    case 'o':   radix = 8; sign = false; number = true;
                                break;

                    // ignore invalid spec
                    default:    break;
                }

                if (number)
                {
                    if (sign)
                    {
                        switch (length)
                        {
                        case PRINTF_LENGTH_SHORT_SHORT:
                        case PRINTF_LENGTH_SHORT:
                        case PRINTF_LENGTH_DEFAULT:     printf_signed(va_arg(args, int), radix);
                                                        break;

                        case PRINTF_LENGTH_LONG:        printf_signed(va_arg(args, long), radix);
                                                        break;

                        case PRINTF_LENGTH_LONG_LONG:   printf_signed(va_arg(args, long long), radix);
                                                        break;
                        }
                    }
                    else
                    {
                        switch (length)
                        {
                        case PRINTF_LENGTH_SHORT_SHORT:
                        case PRINTF_LENGTH_SHORT:
                        case PRINTF_LENGTH_DEFAULT:     printf_unsigned(va_arg(args, unsigned int), radix);
                                                        break;
                                                        
                        case PRINTF_LENGTH_LONG:        printf_unsigned(va_arg(args, unsigned  long), radix);
                                                        break;

                        case PRINTF_LENGTH_LONG_LONG:   printf_unsigned(va_arg(args, unsigned  long long), radix);
                                                        break;
                        }
                    }
                }

                // reset state
                state = PRINTF_STATE_NORMAL;
                length = PRINTF_LENGTH_DEFAULT;
                radix = 10;
                sign = false;
                number = false;
                break;
        }

        fmt++;
    }

    va_end(args);
}

void print_buffer(const char* msg, const void* buffer, uint32_t count)
{
    const uint8_t* u8Buffer = (const uint8_t*)buffer;
    
    puts(msg);
    for (uint16_t i = 0; i < count; i++)
    {
        putc(g_HexChars[u8Buffer[i] >> 4]);
        putc(g_HexChars[u8Buffer[i] & 0xF]);
    }
    puts("\n");
}

void dbg_putc(char c){
    switch (c) {
        case '\r':
            outb(0xe9, '\r');
            break;
        case '\n':
            outb(0xe9, '\n');
            break;
        case '\t':
            for(char i = 0; i < tap_len; i++){
                outb(0xe9, '\033');
                outb(0xe9, '[');
                outb(0xe9, 'C');
            }
            break;
        case '\b':
            outb(0xe9, '\b');
            break;
        default:
            outb(0xe9, (uint8_t)c);
            break;
    }
}

void dbg_puts(const char *str){
    while(*str){
        dbg_putc(*str);
        str++;
    }
}

void dbg_printf_unsigned(unsigned long long number, int radix)
{
    char buffer[32];
    int pos = 0;

    // convert number to ASCII
    do 
    {
        unsigned long long rem = number % (unsigned long long)radix;
        number /= (unsigned long long)radix;
        buffer[pos++] = g_HexChars[rem];
    } while (number > 0);

    // print number in reverse order
    while (--pos >= 0)
        dbg_putc(buffer[pos]);
}

void dbg_printf_signed(long long number, int radix)
{
    if (number < 0)
    {
        dbg_putc('-');
        dbg_printf_unsigned((unsigned long long)-number, radix);
    }
    else dbg_printf_unsigned((unsigned long long)number, radix);
}

void dbg_printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int state = PRINTF_STATE_NORMAL;
    int length = PRINTF_LENGTH_DEFAULT;
    int radix = 10;
    bool sign = false;
    bool number = false;

    while (*fmt)
    {
        switch (state)
        {
            case PRINTF_STATE_NORMAL:
                switch (*fmt)
                {
                    case '%':   state = PRINTF_STATE_LENGTH;
                                break;
                    default:    dbg_putc(*fmt);
                                break;
                }
                break;

            case PRINTF_STATE_LENGTH:
                switch (*fmt)
                {
                    case 'h':   length = PRINTF_LENGTH_SHORT;
                                state = PRINTF_STATE_LENGTH_SHORT;
                                break;
                    case 'l':   length = PRINTF_LENGTH_LONG;
                                state = PRINTF_STATE_LENGTH_LONG;
                                break;
                    default:    goto PRINTF_STATE_SPEC_;
                }
                break;

            case PRINTF_STATE_LENGTH_SHORT:
                if (*fmt == 'h')
                {
                    length = PRINTF_LENGTH_SHORT_SHORT;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_LENGTH_LONG:
                if (*fmt == 'l')
                {
                    length = PRINTF_LENGTH_LONG_LONG;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_SPEC:
            PRINTF_STATE_SPEC_:
                switch (*fmt)
                {
                    case 'c':   dbg_putc((char)va_arg(args, int));
                                break;

                    case 's':   
                                dbg_puts(va_arg(args, const char*));
                                break;

                    case '%':   dbg_putc('%');
                                break;

                    case 'd':
                    case 'i':   radix = 10; sign = true; number = true;
                                break;

                    case 'u':   radix = 10; sign = false; number = true;
                                break;

                    case 'X':
                    case 'x':
                    case 'p':   radix = 16; sign = false; number = true;
                                break;

                    case 'o':   radix = 8; sign = false; number = true;
                                break;

                    // ignore invalid spec
                    default:    break;
                }

                if (number)
                {
                    if (sign)
                    {
                        switch (length)
                        {
                        case PRINTF_LENGTH_SHORT_SHORT:
                        case PRINTF_LENGTH_SHORT:
                        case PRINTF_LENGTH_DEFAULT:     dbg_printf_signed(va_arg(args, int), radix);
                                                        break;

                        case PRINTF_LENGTH_LONG:        dbg_printf_signed(va_arg(args, long), radix);
                                                        break;

                        case PRINTF_LENGTH_LONG_LONG:   dbg_printf_signed(va_arg(args, long long), radix);
                                                        break;
                        }
                    }
                    else
                    {
                        switch (length)
                        {
                        case PRINTF_LENGTH_SHORT_SHORT:
                        case PRINTF_LENGTH_SHORT:
                        case PRINTF_LENGTH_DEFAULT:     dbg_printf_unsigned(va_arg(args, unsigned int), radix);
                                                        break;
                                                        
                        case PRINTF_LENGTH_LONG:        dbg_printf_unsigned(va_arg(args, unsigned  long), radix);
                                                        break;

                        case PRINTF_LENGTH_LONG_LONG:   dbg_printf_unsigned(va_arg(args, unsigned  long long), radix);
                                                        break;
                        }
                    }
                }

                // reset state
                state = PRINTF_STATE_NORMAL;
                length = PRINTF_LENGTH_DEFAULT;
                radix = 10;
                sign = false;
                number = false;
                break;
        }

        fmt++;
    }

    va_end(args);
}

void dbg_print_buffer(const char* msg, const void* buffer, uint32_t count)
{
    const uint8_t* u8Buffer = (const uint8_t*)buffer;
    
    dbg_puts(msg);
    for (uint16_t i = 0; i < count; i++)
    {
        dbg_putc(g_HexChars[u8Buffer[i] >> 4]);
        dbg_putc(g_HexChars[u8Buffer[i] & 0xF]);
    }
    dbg_puts("\n");
}