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

#include "ata.h"

void wait_for_drive_ready() {
    while (inb(IDE_STATUS_REG) & 0x80); // Wait for BSY bit to clear
}

void send_ata_command(uint8_t command) {
    outb(IDE_COMMAND_REG, command);
}

void read_ata_data(uint16_t* buffer, uint32_t size) {
    for (uint32_t i = 0; i < size / 2; i++) {
        buffer[i] = (uint16_t)inb(IDE_DATA_REG) | ((uint16_t)inb(IDE_DATA_REG) << 8);
    }
}

void identify_ide_drive(struct DriveInfo* drive_info) {
    // Wait for the controller to be ready
    wait_for_drive_ready();

    // Send the Identify Device command
    outb(IDE_DRIVE_REG, 0xA0); // Select the drive (0xA0 for master)
    outb(IDE_SECT_CNT_REG, 0); // Sector count
    outb(IDE_SECT_NUM_REG, 0); // Sector number
    outb(IDE_CYL_LOW_REG, 0);  // Cylinder low
    outb(IDE_CYL_HIGH_REG, 0); // Cylinder high
    outb(IDE_COMMAND_REG, ATA_IDENTIFY); // Send the command

    // Wait for the drive to respond
    wait_for_drive_ready();

    // Read the data
    uint16_t data[256];
    read_ata_data(data, sizeof(data));

    
}