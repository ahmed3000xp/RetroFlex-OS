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

#include "cmos.h"

uint32_t current_year = 2024;
int32_t century_register = 0x00;
volatile uint8_t second;
volatile uint8_t minute;
volatile uint8_t hour;
volatile uint8_t day;
volatile uint8_t month;
volatile uint32_t year;

int32_t get_update_in_progress_flag() {
      outb(cmos_address, 0x0A);
      return (inb(cmos_data) & 0x80);
}

uint8_t get_RTC_register(int reg) {
      outb(cmos_address, reg);
      return inb(cmos_data);
}

void write_RTC_register(int reg, uint8_t value) {
      outb(cmos_address, reg);
      outb(cmos_data, value);
}

void read_rtc() {
      uint8_t century;
      uint8_t last_second;
      uint8_t last_minute;
      uint8_t last_hour;
      uint8_t last_day;
      uint8_t last_month;
      uint8_t last_year;
      uint8_t last_century;
      uint8_t registerB;

      while (get_update_in_progress_flag());
      second = get_RTC_register(0x00);
      minute = get_RTC_register(0x02);
      hour = get_RTC_register(0x04);
      day = get_RTC_register(0x07);
      month = get_RTC_register(0x08);
      year = get_RTC_register(0x09);
      if (century_register != 0) {
            century = get_RTC_register(century_register);
      }

      do {
            last_second = second;
            last_minute = minute;
            last_hour = hour;
            last_day = day;
            last_month = month;
            last_year = year;
            last_century = century;

            while (get_update_in_progress_flag());
            second = get_RTC_register(0x00);
            minute = get_RTC_register(0x02);
            hour = get_RTC_register(0x04);
            day = get_RTC_register(0x07);
            month = get_RTC_register(0x08);
            year = get_RTC_register(0x09);
            if (century_register != 0) {
                  century = get_RTC_register(century_register);
            }
      } while ((last_second != second) || (last_minute != minute) || (last_hour != hour) ||
               (last_day != day) || (last_month != month) || (last_year != year) ||
               (last_century != century));

      registerB = get_RTC_register(0x0B);

      if (!(registerB & 0x04)) {
            second = (second & 0x0F) + ((second / 16) * 10);
            minute = (minute & 0x0F) + ((minute / 16) * 10);
            hour = ((hour & 0x0F) + (((hour & 0x70) / 16) * 10)) | (hour & 0x80);
            day = (day & 0x0F) + ((day / 16) * 10);
            month = (month & 0x0F) + ((month / 16) * 10);
            year = (year & 0x0F) + ((year / 16) * 10);
            if (century_register != 0) {
                  century = (century & 0x0F) + ((century / 16) * 10);
            }
      }

      if (!(registerB & 0x02) && (hour & 0x80)) {
            hour = ((hour & 0x7F) + 12) % 24;
      }

      if (century_register != 0) {
            year += century * 100;
      } else {
            year += (current_year / 100) * 100;
            if (year < current_year)
                  year += 100;
      }

      // Update CMOS with current values
      update_cmos();
}

void update_cmos() {
      // Ensure CMOS update is not in progress
      while (get_update_in_progress_flag());

      // Set the RTC registers with the current date and time values
      outb(cmos_address, 0x00);
      outb(cmos_data, second);
      
      outb(cmos_address, 0x02);
      outb(cmos_data, minute);
      
      outb(cmos_address, 0x04);
      outb(cmos_data, hour);
      
      outb(cmos_address, 0x07);
      outb(cmos_data, day);
      
      outb(cmos_address, 0x08);
      outb(cmos_data, month);
      
      outb(cmos_address, 0x09);
      outb(cmos_data, year & 0xFF);

      if (century_register != 0) {
            outb(cmos_address, century_register);
            outb(cmos_data, (year / 100) & 0xFF);
      }
}