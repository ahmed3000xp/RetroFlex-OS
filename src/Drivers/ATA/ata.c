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

// WARNING: This code might be beyond repair. Here’s a comprehensive waste-time tracker:
// Total Centuries wasted = 0 
// Total Decades wasted = 0
// Total Years wasted = 0
// Total Months wasted = 0
// Total Days wasted = 4 (Because sleep is for the weak)
// Total Hours wasted = 10 (Might be the same as the hours spent wondering why this code even exists)
// Total Minutes wasted = 30 (Equivalent to 30 minutes of your life you’ll never get back)
// Total Seconds wasted = 5 (Blink and you missed it, except you didn't)
// Total Milliseconds wasted = 504 (That's like 1/2 a second, not even close to getting it right)
// Total Microseconds wasted = 64 (At least it’s not a full second... yet)
// Total Nanoseconds wasted = 643 (That’s a lot of tiny regrets)
// Total Picoseconds wasted = 865 (We’re getting into the realm of “why do I even bother”)
// Total Amount of furries tried to fix this Code = 1 (Yes, even furries give up eventually)

#include "ata.h"

// ATA ports and commands for primary controller
static uint16_t ATA_PRIMARY_COMMAND_PORT = 0x1F7;
static uint16_t ATA_PRIMARY_CONTROL_PORT = 0x3F6;
static uint16_t ATA_PRIMARY_DATA_PORT = 0x1F0;
static uint16_t ATA_PRIMARY_DRIVE_SELECT_PORT = 0x1F6;

// ATA ports and commands for secondary controller
static uint16_t ATA_SECONDARY_COMMAND_PORT = 0x177;
static uint16_t ATA_SECONDARY_CONTROL_PORT = 0x376;
static uint16_t ATA_SECONDARY_DATA_PORT = 0x170;
static uint16_t ATA_SECONDARY_DRIVE_SELECT_PORT = 0x176;

void select_drive(bool is_secondary, bool is_slave) {
    uint16_t drive_select_port = is_secondary ? ATA_SECONDARY_DRIVE_SELECT_PORT : ATA_PRIMARY_DRIVE_SELECT_PORT;
    uint8_t drive_select_value = is_slave ? 0xB0 : 0xA0;
    outb(drive_select_port, drive_select_value);
}

void wait_for_ready(bool is_secondary) {
    uint16_t command_port = is_secondary ? ATA_SECONDARY_COMMAND_PORT : ATA_PRIMARY_COMMAND_PORT;
    while (true) {
        uint8_t status = inb(command_port);
        if ((status & 0x80) == 0) break; // Drive not busy
        if (status & 0x01) {
            dbg_printf("[%d] Drive error occurred.\n", ticks);
            return;
        }
    }
}

void identify_drive(struct DriveInfo *drive_info, bool is_slave, bool is_secondary) {
    (void)ATA_PRIMARY_CONTROL_PORT; (void)ATA_SECONDARY_CONTROL_PORT;
    
    memset(drive_info, 0, sizeof(struct DriveInfo));

    uint16_t identify_data[256] = {0};

    select_drive(is_secondary, is_slave);

    // Send IDENTIFY command
    uint16_t command_port = is_secondary ? ATA_SECONDARY_COMMAND_PORT : ATA_PRIMARY_COMMAND_PORT;
    outb(command_port, ATA_IDENTIFY_COMMAND);

    // Polling until the drive is ready
    wait_for_ready(is_secondary);

    // Read the IDENTIFY data
    uint16_t data_port = is_secondary ? ATA_SECONDARY_DATA_PORT : ATA_PRIMARY_DATA_PORT;
    insw(data_port, identify_data, 256);

    // Check if the device is a drive
    if (identify_data[0] == 0x0000 || identify_data[0] == 0xFFFF) {
        drive_info->detected = false;
        return;
    }
    drive_info->detected = true;

    // Extract model number
    for (int i = 0; i < 20; ++i) {
        drive_info->model[i * 2] = identify_data[27 + i] >> 8;
        drive_info->model[i * 2 + 1] = (identify_data[27 + i] & 0xFF);
    }
    drive_info->model[40] = '\0';

    // Check for LBA48 support
    drive_info->lba_48 = (identify_data[83] & (1 << 10)) != 0;

    // Check for supported UDMA modes
    drive_info->supported_udma_modes = identify_data[88] & 0xFF;

    // Check for active UDMA mode
    uint8_t active_udma_mode = (uint8_t)(identify_data[88] >> 8) & 0xFF;
    drive_info->supported_udma_modes |= active_udma_mode << 8;

    // Check for 80 conductor cable
    drive_info->wire_80_cable = (identify_data[93] & (1 << 11)) != 0;

    // Get the number of LBA28 sectors
    drive_info->number_lba_28_sectors = identify_data[60] | (identify_data[61] << 16);

    // Get the number of LBA48 sectors
    drive_info->number_lba_48_sectors = (uint64_t)identify_data[100] |
    ((uint64_t)identify_data[101] << 16) |
    ((uint64_t)identify_data[102] << 32) |
    ((uint64_t)identify_data[103] << 48);

    // Determine addressing mode
    if (drive_info->number_lba_48_sectors > 0) {
        drive_info->lba_48 = true; // LBA48
    } 
    if (drive_info->number_lba_28_sectors > 0) {
        drive_info->lba_28 = true; // LBA28
    } 
    else {
        drive_info->chs = true; // CHS
    }

    drive_info->sector_size = identify_data[106]; // Default sector size

    // Set drive type
    drive_info->drive_type = identify_data[0];
}

bool check_ata_controller() {
    struct DriveInfo drive_info;

    // Try identifying a drive on the primary master
    identify_drive(&drive_info, false, false);
    if (drive_info.detected) {
        dbg_printf("[%d] Primary master ATA drive detected\n", ticks);
        return true;
    }

    // Try identifying a drive on the primary slave
    identify_drive(&drive_info, true, false);
    if (drive_info.detected) {
        dbg_printf("[%d] Primary slave ATA drive detected\n", ticks);
        return true;
    }

    // Try identifying a drive on the secondary master
    identify_drive(&drive_info, false, true);
    if (drive_info.detected) {
        dbg_printf("[%d] Secondary master ATA drive detected\n", ticks);
        return true;
    }

    // Try identifying a drive on the secondary slave
    identify_drive(&drive_info, true, true);
    if (drive_info.detected) {
        dbg_printf("[%d] Secondary slave ATA drive detected\n",ticks);
        return true;
    }

    // If no ATA drive is detected, proceed with PCI detection
    dbg_printf("[%d] No ATA controller detected. Checking PCI\n", ticks);

    bool pci_detected = false;
    // Implement your PCI detection logic here and set pci_detected accordingly

    if (pci_detected) {
        dbg_printf("[%d] PCI ATA controller detected\n", ticks);
        return true;
    }

    // If no ATA controller is detected via PCI
    return false;
}

void write_command(uint16_t command_port, uint8_t command, uint64_t lba, uint32_t sector_count) {
    // Send the command
    outb(command_port, command);

    // Send the LBA address
    outb(command_port, (uint8_t)(lba & 0xFF));
    outb(command_port, (uint8_t)((lba >> 8) & 0xFF));
    outb(command_port, (uint8_t)((lba >> 16) & 0xFF));
    if (sector_count > 1) {
        outb(command_port, (uint8_t)((lba >> 24) & 0xFF));
    }
}

void read_sector_lba48(uint64_t lba, void *buffer, uint32_t buffer_size, struct DriveInfo *drive_info) {
    if (buffer == NULL || buffer_size == 0 || buffer_size % SECTOR_SIZE != 0) {
        dbg_printf("[%d] Invalid buffer or buffer size.\n", ticks);
        return;
    }

    uint32_t sector_count = buffer_size / SECTOR_SIZE;
    uint16_t command_port = drive_info->is_secondary ? ATA_SECONDARY_COMMAND_PORT : ATA_PRIMARY_COMMAND_PORT;
    uint16_t data_port = drive_info->is_secondary ? ATA_SECONDARY_DATA_PORT : ATA_PRIMARY_DATA_PORT;

    select_drive(drive_info->is_secondary, drive_info->is_slave);

    for (uint32_t i = 0; i < sector_count; i++) {
        outb(command_port + 1, (sector_count >> 8) & 0xFF);
        outb(command_port + 2, sector_count & 0xFF);
        outb(command_port + 3, (lba >> 24) & 0xFF);
        outb(command_port + 4, (lba >> 32) & 0xFF);
        outb(command_port + 5, (lba >> 40) & 0xFF);
        outb(command_port + 6, 0x40); // LBA mode
        outb(command_port + 7, 0x24); // READ SECTOR(S) EXT command

        wait_for_ready(drive_info->is_secondary);

        insw(data_port, buffer, SECTOR_SIZE / 2);
        buffer = (uint8_t *)buffer + SECTOR_SIZE;
        lba++;
    }
}

void write_sector_lba48(uint64_t lba, const void *buffer, uint32_t buffer_size, struct DriveInfo *drive_info) {
    if (buffer == NULL || buffer_size == 0 || buffer_size % SECTOR_SIZE != 0) {
        dbg_printf("[%d] Invalid buffer or buffer size.\n", ticks);
        return;
    }

    uint32_t sector_count = buffer_size / SECTOR_SIZE;
    uint16_t command_port = drive_info->is_secondary ? ATA_SECONDARY_COMMAND_PORT : ATA_PRIMARY_COMMAND_PORT;
    uint16_t data_port = drive_info->is_secondary ? ATA_SECONDARY_DATA_PORT : ATA_PRIMARY_DATA_PORT;

    select_drive(drive_info->is_secondary, drive_info->is_slave);
retursector_numn;
    for (uint32_t i = 0; i < sector_count; i++) {
        outb(command_port + 1, (sector_count >> 8) & 0xFF);
        outb(command_port + 2, sector_count & 0xFF);
        outb(command_port + 3, (lba >> 24) & 0xFF);
        outb(command_port + 4, (lba >> 32) & 0xFF);
        outb(command_port + 5, (lba >> 40) & 0xFF);
        outb(command_port + 6, 0x40); // LBA mode
        outb(command_port + 7, 0x34); // WRITE SECTOR(S) EXT command

        wait_for_ready(drive_info->is_secondary);

        outsw(data_port, buffer, SECTOR_SIZE / 2);
        buffer = (const uint8_t *)buffer + SECTOR_SIZE;
        lba++;
    }
}

void read_sector_lba28(uint32_t lba, void *buffer, uint32_t buffer_size, struct DriveInfo *drive_info) {
    if (buffer == NULL || buffer_size == 0 || buffer_size % drive_info->sector_size != 0) {
        dbg_printf("[%d] Invalid buffer or buffer size.\n", ticks);
        return;
    }

    uint32_t sector_count = buffer_size / drive_info->sector_size;
    uint16_t command_port = drive_info->is_secondary ? ATA_SECONDARY_COMMAND_PORT : ATA_PRIMARY_COMMAND_PORT;
    uint16_t data_port = drive_info->is_secondary ? ATA_SECONDARY_DATA_PORT : ATA_PRIMARY_DATA_PORT;

    select_drive(drive_info->is_secondary, drive_info->is_slave);

    for (uint32_t i = 0; i < sector_count; i++) {
        write_command(command_port, 0x20, lba, sector_count);

        wait_for_ready(drive_info->is_secondary);

        insw(data_port, buffer, drive_info->sector_size / 2);
        buffer = (uint8_t *)buffer + drive_info->sector_size;
        lba++;
    }
}

void write_sector_lba28(uint32_t lba, const void *buffer, uint32_t buffer_size, struct DriveInfo *drive_info) {
    if (buffer == NULL || buffer_size == 0 || buffer_size % drive_info->sector_size != 0) {
        dbg_printf("[%d] Invalid buffer or buffer size.\n", ticks);
        return;
    }

    uint32_t sector_count = buffer_size / drive_info->sector_size;
    uint16_t command_port = drive_info->is_secondary ? ATA_SECONDARY_COMMAND_PORT : ATA_PRIMARY_COMMAND_PORT;
    uint16_t data_port = drive_info->is_secondary ? ATA_SECONDARY_DATA_PORT : ATA_PRIMARY_DATA_PORT;

    select_drive(drive_info->is_secondary, drive_info->is_slave);

    for (uint32_t i = 0; i < sector_count; i++) {
        write_command(command_port, 0x30, lba, sector_count);

        wait_for_ready(drive_info->is_secondary);

        outsw(data_port, buffer, drive_info->sector_size / 2);
        buffer = (const uint8_t *)buffer + drive_info->sector_size;
        lba++;
    }
}

void read_sector_chs(uint16_t cylinder, uint8_t head, uint8_t sector, void *buffer, uint32_t buffer_size, struct DriveInfo *drive_info) {
    if (buffer == NULL || buffer_size == 0 || buffer_size % drive_info->sector_size != 0) {
        dbg_printf("[%d] Invalid buffer or buffer size.\n", ticks);
        return;
    }

    uint32_t sector_count = buffer_size / drive_info->sector_size;
    uint16_t command_port = drive_info->is_secondary ? ATA_SECONDARY_COMMAND_PORT : ATA_PRIMARY_COMMAND_PORT;
    uint16_t data_port = drive_info->is_secondary ? ATA_SECONDARY_DATA_PORT : ATA_PRIMARY_DATA_PORT;

    select_drive(drive_info->is_secondary, drive_info->is_slave);

    for (uint32_t i = 0; i < sector_count; i++) {
        // Write the head and drive number
        outb(command_port + 6, 0xA0 | (drive_info->is_slave << 4) | head);

        // Write the cylinder number
        outb(command_port + 2, (uint8_t)(cylinder >> 8));
        outb(command_port + 3, (uint8_t)(cylinder & 0xFF));

        // Write the sector number
        outb(command_port + 4, sector);

        // Write the sector count
        outb(command_port + 5, (uint8_t)sector_count);

        // Write the command
        outb(command_port + 7, 0x21); // Read sectors command

        wait_for_ready(drive_info->is_secondary);

        insw(data_port, buffer, drive_info->sector_size / 2);
        buffer = (uint8_t *)buffer + drive_info->sector_size;
    }
}

void write_sector_chs(uint16_t cylinder, uint8_t head, uint8_t sector, const void *buffer, uint32_t buffer_size, struct DriveInfo *drive_info) {
    if (buffer == NULL || buffer_size == 0 || buffer_size % drive_info->sector_size != 0) {
        dbg_printf("[%d] Invalid buffer or buffer size.\n", ticks);
    }

    uint32_t sector_count = buffer_size / drive_info->sector_size;
    uint16_t command_port = drive_info->is_secondary ? ATA_SECONDARY_COMMAND_PORT : ATA_PRIMARY_COMMAND_PORT;
    uint16_t data_port = drive_info->is_secondary ? ATA_SECONDARY_DATA_PORT : ATA_PRIMARY_DATA_PORT;

    select_drive(drive_info->is_secondary, drive_info->is_slave);

    for (uint32_t i = 0; i < sector_count; i++) {
        // Write the head and drive number
        outb(command_port + 6, 0xA0 | (drive_info->is_slave << 4) | head);

        // Write the cylinder number
        outb(command_port + 2, (uint8_t)(cylinder >> 8));
        outb(command_port + 3, (uint8_t)(cylinder & 0xFF));

        // Write the sector number
        outb(command_port + 4, sector);

        // Write the sector count
        outb(command_port + 5, (uint8_t)sector_count);

        // Write the command
        outb(command_port + 7, 0x31); // Write sectors command

        wait_for_ready(drive_info->is_secondary);

        outsw(data_port, buffer, drive_info->sector_size / 2);
        buffer = (const uint8_t *)buffer + drive_info->sector_size;
    }
}

void read_sector(uint32_t sector, void *buffer, uint32_t buffer_size, struct DriveInfo *drive_info) {
    if (drive_info->lba_48) {
        uint64_t lba_48 = (uint64_t)sector;
        read_sector_lba48(lba_48, buffer, buffer_size, drive_info);
    } else if (drive_info->lba_28) {
        read_sector_lba28(sector, buffer, buffer_size, drive_info);
    } else if (drive_info->chs) {
        uint16_t cylinder = (uint16_t)(sector / (drive_info->heads_per_cylinder * drive_info->sectors_per_track));
        uint8_t head = (uint8_t)((sector / drive_info->sectors_per_track) % drive_info->heads_per_cylinder);
        uint8_t sector_num = (uint8_t)(((sector % drive_info->sectors_per_track) + 1) & 0xFF);
        read_sector_chs(cylinder, head, sector_num, buffer, buffer_size, drive_info);
    } else {
        dbg_printf("[%d] Unsupported addressing mode.\n", ticks);
    }
}

void write_sector(uint32_t sector, const void *buffer, uint32_t buffer_size, struct DriveInfo *drive_info) {
    if (drive_info->lba_48) {
        uint64_t lba_48 = (uint64_t)sector;
        write_sector_lba48(lba_48, buffer, buffer_size, drive_info);
    } else if (drive_info->lba_28) {
        write_sector_lba28(sector, buffer, buffer_size, drive_info);
    } else if (drive_info->chs) {
        uint16_t cylinder = (uint16_t)(sector / (drive_info->heads_per_cylinder * drive_info->sectors_per_track));
        uint8_t head = (uint8_t)((sector / drive_info->sectors_per_track) % drive_info->heads_per_cylinder);
        uint8_t sector_num = (uint8_t)(((sector % drive_info->sectors_per_track) + 1) & 0xFF);
        write_sector_chs(cylinder, head, sector_num, buffer, buffer_size, drive_info);
    } else {
        dbg_printf("[%d] Unsupported addressing mode.\n", ticks);
    }
}

void print_drive_info(struct DriveInfo *drive_info) {
    dbg_printf("[%d] Detected: %s\n", ticks, drive_info->detected ? "Yes" : "No");
    dbg_printf("[%d] Model: %s\n", ticks, drive_info->model);
    dbg_printf("[%d] LBA28: %s\n", ticks, drive_info->lba_28 ? "Yes" : "No");
    dbg_printf("[%d] LBA48: %s\n", ticks, drive_info->lba_48 ? "Yes" : "No");
    dbg_printf("[%d] CHS: %s\n", ticks, drive_info->chs ? "Yes" : "No");
    dbg_printf("[%d] Number of LBA28 sectors: %u\n", ticks, drive_info->number_lba_28_sectors);
    dbg_printf("[%d] Number of LBA48 sectors: %llu\n", ticks, drive_info->number_lba_48_sectors);
    dbg_printf("[%d] Sector size: %u\n", ticks, drive_info->sector_size);
    dbg_printf("[%d] Supported UDMA modes: %02X\n", ticks, drive_info->supported_udma_modes);
    dbg_printf("[%d] Active UDMA mode: %02X\n", ticks, drive_info->supported_udma_modes);
    dbg_printf("[%d] 80-wire cable: %s\n", ticks, drive_info->wire_80_cable ? "Yes" : "No");
}