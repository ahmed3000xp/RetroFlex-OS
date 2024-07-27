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

#define IDE_PRIMARY_BASE 0x1F0
#define IDE_PRIMARY_CTRL 0x3F6
#define IDE_DATA_REG (IDE_PRIMARY_BASE + 0)
#define IDE_ERROR_REG (IDE_PRIMARY_BASE + 1)
#define IDE_SECT_CNT_REG (IDE_PRIMARY_BASE + 2)
#define IDE_SECT_NUM_REG (IDE_PRIMARY_BASE + 3)
#define IDE_CYL_LOW_REG (IDE_PRIMARY_BASE + 4)
#define IDE_CYL_HIGH_REG (IDE_PRIMARY_BASE + 5)
#define IDE_DRIVE_REG (IDE_PRIMARY_BASE + 6)
#define IDE_COMMAND_REG (IDE_PRIMARY_BASE + 7)
#define IDE_STATUS_REG (IDE_PRIMARY_BASE + 7)

#define ATA_IDENTIFY 0xEC

struct DriveInfo {
    bool detected;
    bool lba_48_capable;
    bool wire_80_cable;
    uint16_t drive_type;
    uint16_t supported_udma_modes;
    uint32_t number_lba_28_sectors;
    uint64_t number_lba_48_sectors;
}__attribute__((packed));