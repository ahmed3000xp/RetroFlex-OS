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

#include "speaker.h"

void beep(uint32_t frequencey) {
 	uint8_t tmp;

 	tmp = inb(0x61);

  	if (tmp != (tmp | 3)) {
 		outb(0x61, tmp | 3);
 	}
}
 
void stop_beep() {
 	uint8_t tmp = inb(0x61) & 0xFC;
 	outb(0x61, tmp);
}