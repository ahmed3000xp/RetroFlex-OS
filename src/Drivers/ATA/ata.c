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

void select_drive(bool is_secondary, bool is_slave) {
    uint16_t drive_select_port = is_secondary ? ATA_SECONDARY_DRIVE_SELECT_PORT : ATA_PRIMARY_DRIVE_SELECT_PORT;
    uint8_t drive_select_value = is_slave ? 0xB0 : 0xA0;
    outb(drive_select_port, drive_select_value);
}

void wait_for_ready() {
    while ((inb(ATA_PRIMARY_COMMAND_PORT) & 0x80) != 0);
}

void identify_drive(struct DriveInfo *drive_info, bool is_slave, bool is_secondary) {
    memset(drive_info, 0, sizeof(struct DriveInfo));

    uint16_t identify_data[256] = {0};

    if(!is_secondary){
        // Select the drive (0xA0 for master, 0xB0 for slave)
        outb(ATA_PRIMARY_DRIVE_SELECT_PORT, is_slave ? 0xB0 : 0xA0);

        // Send IDENTIFY command
        outb(ATA_PRIMARY_COMMAND_PORT, ATA_IDENTIFY_COMMAND);

        // Polling until the drive is ready
        while (inb(ATA_PRIMARY_COMMAND_PORT) & 0x80);

        // Read the IDENTIFY data
        insw(ATA_PRIMARY_DATA_PORT, identify_data, 256);
    }
    else {
        // Select the drive (0xA0 for master, 0xB0 for slave)
        outb(ATA_SECONDARY_DRIVE_SELECT_PORT, is_slave ? 0xB0 : 0xA0);

        // Send IDENTIFY command
        outb(ATA_SECONDARY_COMMAND_PORT, ATA_IDENTIFY_COMMAND);

        // Polling until the drive is ready
        while (inb(ATA_SECONDARY_COMMAND_PORT) & 0x80);

        // Read the IDENTIFY data
        insw(ATA_SECONDARY_DATA_PORT, identify_data, 256);
    }

    // Check if the device is a drive
    if (identify_data[0] == 0x0000 || identify_data[0] == 0xFFFF) {
        drive_info->detected = false;
        return;
    }
    drive_info->detected = true;

    // Extract model number
    for (int i = 0; i < 20; ++i) {
        drive_info->model[i * 2] = identify_data[27 + i] >> 8;
        drive_info->model[i * 2 + 1] = identify_data[27 + i] & 0xFF;
    }
    drive_info->model[40] = '\0';

    // Check for LBA48 support
    drive_info->lba_48 = (identify_data[83] & (1 << 10)) != 0;

    // Check for supported UDMA modes
    drive_info->supported_udma_modes = identify_data[88] & 0xFF;

    // Check for active UDMA mode
    uint8_t active_udma_mode = (identify_data[88] >> 8) & 0xFF;
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
        drive_info->lba_48= true; // LBA48
    } 
    if (drive_info->number_lba_28_sectors > 0) {
        drive_info->lba_28 = true; // LBA28
    } 
    else {
        drive_info->chs = true; // CHS
    }

    drive_info->sector_size = identify_data[106] * SECTOR_SIZE;

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
        write_command(command_port, 0x24, lba, sector_count);

        wait_for_ready();

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

    for (uint32_t i = 0; i < sector_count; i++) {
        write_command(command_port, 0x34, lba, sector_count);

        wait_for_ready();

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

    uint32_t sector_size = drive_info->sector_size;
    uint32_t sector_count = buffer_size / sector_size;
    uint16_t command_port = drive_info->is_secondary ? ATA_SECONDARY_COMMAND_PORT : ATA_PRIMARY_COMMAND_PORT;
    uint16_t data_port = drive_info->is_secondary ? ATA_SECONDARY_DATA_PORT : ATA_PRIMARY_DATA_PORT;
    uint16_t drive_select_port = drive_info->is_secondary ? ATA_SECONDARY_DRIVE_SELECT_PORT : ATA_PRIMARY_DRIVE_SELECT_PORT;

    select_drive(drive_info->is_secondary, drive_info->is_slave);

    for (uint32_t i = 0; i < sector_count; i++) {
        outb(command_port, 0x20); // READ SECTOR(S) command

        outb(drive_select_port, 0xA0 | ((lba >> 24) & 0x0F));
        outb(command_port, lba & 0xFF);
        outb(command_port, (lba >> 8) & 0xFF);
        outb(command_port, (lba >> 16) & 0xFF);

        wait_for_ready();

        insw(data_port, buffer, sector_size / 2);
        buffer = (uint8_t *)buffer + sector_size;
        lba++;
    }
}

void write_sector_lba28(uint32_t lba, const void *buffer, uint32_t buffer_size, struct DriveInfo *drive_info) {
    if (buffer == NULL || buffer_size == 0 || buffer_size % drive_info->sector_size != 0) {
        dbg_printf("[%d] Invalid buffer or buffer size.\n", ticks);
        return;
    }

    uint32_t sector_size = drive_info->sector_size;
    uint32_t sector_count = buffer_size / sector_size;
    uint16_t command_port = drive_info->is_secondary ? ATA_SECONDARY_COMMAND_PORT : ATA_PRIMARY_COMMAND_PORT;
    uint16_t data_port = drive_info->is_secondary ? ATA_SECONDARY_DATA_PORT : ATA_PRIMARY_DATA_PORT;
    uint16_t drive_select_port = drive_info->is_secondary ? ATA_SECONDARY_DRIVE_SELECT_PORT : ATA_PRIMARY_DRIVE_SELECT_PORT;

    select_drive(drive_info->is_secondary, drive_info->is_slave);

    for (uint32_t i = 0; i < sector_count; i++) {
        outb(command_port, 0x30); // WRITE SECTOR(S) command

        outb(drive_select_port, 0xA0 | ((lba >> 24) & 0x0F));
        outb(command_port, lba & 0xFF);
        outb(command_port, (lba >> 8) & 0xFF);
        outb(command_port, (lba >> 16) & 0xFF);

        wait_for_ready();

        outsw(data_port, buffer, sector_size / 2);
        buffer = (const uint8_t *)buffer + sector_size;
        lba++;
    }
}

void read_sector(uint32_t sector, void *buffer, uint32_t buffer_size, struct DriveInfo *drive_info) {
    if (drive_info == NULL || buffer == NULL) {
        dbg_printf("[%d] Invalid drive_info or buffer.\n", ticks);
        return;
    }

    // Select the drive based on drive_info
    select_drive(drive_info->is_secondary, drive_info->is_slave);

    if (drive_info->lba_48) {
        read_sector_lba48(sector, buffer, buffer_size, drive_info);
    } else if (drive_info->lba_28) {
        read_sector_lba28(sector, buffer, buffer_size, drive_info);
    } else if (drive_info->chs) {
        return;
    } else {
        dbg_printf("Unsupported addressing mode.\n");
    }
}

void write_sector(uint32_t sector, const void *buffer, uint32_t buffer_size, struct DriveInfo *drive_info) {
    if (drive_info == NULL || buffer == NULL) {
        dbg_printf("[%d] Invalid drive_info or buffer.\n", ticks);
        return;
    }

    // Select the drive based on drive_info
    select_drive(drive_info->is_secondary, drive_info->is_slave);

    if (drive_info->lba_48) {
        write_sector_lba48(sector, buffer, buffer_size, drive_info);
    } else if (drive_info->lba_28) {
        write_sector_lba28(sector, buffer, buffer_size, drive_info);
    } else if (drive_info->chs) {
        return;
    } else {
        dbg_printf("[%d] Unsupported addressing mode.\n", ticks);
    }
}

void print_drive_info(struct DriveInfo *drive_info) {
    if (!drive_info->detected) {
        printf("Drive not detected.\n");
        return;
    }

    dbg_printf("Drive Model: %s\n", drive_info->model);
    dbg_printf("Drive Type: %s\n", drive_info->is_sata ? "SATA" : "PATA");
    dbg_printf("Drive is %s\n", drive_info->is_slave ? "Slave" : "Master");
    dbg_printf("Drive is on %s\n", drive_info->is_secondary ? "Secondary Channel" : "Primary Channel");
    dbg_printf("Drive uses %s\n", drive_info->wire_80_cable ? "80-wire cable" : "40-wire cable");
    dbg_printf("Sector Size: %u\n", drive_info->sector_size);
    dbg_printf("Supports CHS: %s\n", drive_info->chs ? "Yes" : "No");
    dbg_printf("Supports LBA28: %s\n", drive_info->lba_28 ? "Yes" : "No");
    dbg_printf("Supports LBA48: %s\n", drive_info->lba_48 ? "Yes" : "No");
    dbg_printf("LBA28 Sector Count: %u\n", drive_info->number_lba_28_sectors);
    dbg_printf("LBA48 Sector Count: %llu\n", drive_info->number_lba_48_sectors);
    dbg_printf("Sectors per Track: %u\n", drive_info->sectors_per_track);
    dbg_printf("Heads per Cylinder: %u\n", drive_info->heads_per_cylinder);
    dbg_printf("Supported UDMA Modes: 0x%X\n", drive_info->supported_udma_modes);
}