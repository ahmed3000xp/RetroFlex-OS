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

extern uint32_t current_year;
extern int32_t century_register;
extern volatile uint8_t second;
extern volatile uint8_t minute;
extern volatile uint8_t hour;
extern volatile uint8_t day;
extern volatile uint8_t month;
extern volatile uint32_t year;

enum {
      cmos_address = 0x70,
      cmos_data    = 0x71
};

int32_t get_update_in_progress_flag();
uint8_t get_RTC_register(int reg);
void read_rtc();
void update_cmos();