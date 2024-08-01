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
#include "../VGA/vga.h"
#include "../PCI/pci.h"
#include "../CMOS/cmos.h"
#include "../PS2/ps2.h"

struct DriveInfo {
    bool detected;
    bool is_sata;
    bool is_ssd;
    bool is_secondary;
    bool is_slave;
    bool chs;
    bool lba_28;
    bool lba_48;
    bool wire_80_cable;
    char model[41];
    uint32_t sector_size;
    uint16_t drive_type;
    uint16_t supported_udma_modes;
    uint32_t number_lba_28_sectors;
    uint64_t number_lba_48_sectors;
    uint32_t sectors_per_track; 
    uint32_t heads_per_cylinder;
}__attribute__((packed));

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

#define ATA_IDENTIFY_COMMAND 0xEC
#define SECTOR_SIZE 512

void select_drive(bool is_secondary, bool is_slave);
void wait_for_ready(bool is_secondary);
void identify_drive(struct DriveInfo *drive_info, bool is_slave, bool is_secondary);
bool check_ata_controller();
void write_command(uint16_t command_port, uint8_t command, uint64_t lba, uint32_t sector_count);
void read_sector_lba48(uint64_t lba, void *buffer, uint32_t buffer_size, struct DriveInfo *drive_info);
void write_sector_lba48(uint64_t lba, const void *buffer, uint32_t buffer_size, struct DriveInfo *drive_info);
void read_sector_lba28(uint32_t lba, void *buffer, uint32_t buffer_size, struct DriveInfo *drive_info);
void write_sector_lba28(uint32_t lba, const void *buffer, uint32_t buffer_size, struct DriveInfo *drive_info);
void read_sector_chs(uint16_t cylinder, uint8_t head, uint8_t sector, void *buffer, uint32_t buffer_size, struct DriveInfo *drive_info);
void write_sector_chs(uint16_t cylinder, uint8_t head, uint8_t sector, const void *buffer, uint32_t buffer_size, struct DriveInfo *drive_info);
void read_sector(uint32_t sector, void *buffer, uint32_t buffer_size, struct DriveInfo *drive_info);
void write_sector(uint32_t sector, const void *buffer, uint32_t buffer_size, struct DriveInfo *drive_info);
void print_drive_info(struct DriveInfo *drive_info);