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

struct header_0 {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t revision_id;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
    uint32_t bar0;
    uint32_t bar1;
    uint32_t bar2;
    uint32_t bar3;
    uint32_t bar4;
    uint32_t bar5;
    uint32_t cardbus_cis_ptr;
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_id;
    uint32_t expansion_rom_base_addr;
    uint8_t capabilities_ptr;
    uint16_t reserved;
    uint8_t reserved1;
    uint32_t reserved2;
    uint8_t int_line;
    uint8_t int_pin;
    uint8_t min_grant;
    uint8_t max_latency;
}__attribute__((packed));

struct header_1 {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t revision_id;
    uint8_t prog_if;
    uint8_t subclass;
    uint8_t class_code;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
    uint32_t bar0;
    uint32_t bar1;
    uint8_t primary_bus_num;
    uint8_t secondary_bus_num;
    uint8_t subordinate_bus_num;
    uint8_t secondary_latency_timer;
    uint8_t io_base;
    uint8_t io_limit;
    uint16_t secondary_status;
    uint16_t memory_base;
    uint16_t memory_limit;
    uint16_t prefetchable_memory_base;
    uint16_t prefetchable_memory_limit;
    uint32_t prefetchable_base_upper_32_bits;
    uint32_t prefetchable_limit_upper_32_bits;
    uint16_t io_base_upper_16_bits;
    uint16_t io_limit_upper_16_bits;
    uint8_t capability_ptr;
    uint16_t reserved;
    uint16_t reserved1;
    uint32_t expansion_rom_base_addr;
    uint8_t int_line;
    uint8_t int_pin;
    uint16_t bridge_control;
}__attribute__((packed));

struct header_2 {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t revision_id;
    uint8_t prog_if;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
    uint32_t cardbus_sock_exca_base_addr;
    uint8_t offset_capabilities_list;
    uint8_t reserved;
    uint16_t secondary_status;
    uint8_t pci_bus_number;
    uint8_t cardbus_bus_num;
    uint8_t subordinate_bus_number;
    uint8_t cardbus_latency_timer;
    uint32_t memory_base_addr0;
    uint32_t memory_limit0;
    uint32_t memory_base_addr1;
    uint32_t memory_limit1;
    uint32_t io_base_addr0;
    uint32_t io_limit0;
    uint32_t io_base_addr1;
    uint32_t io_limit1;
    uint8_t int_line;
    uint8_t int_pin;
    uint16_t bridge_control;
    uint16_t subsystem_device_id;
    uint16_t subsystem_vendor_id;
    uint32_t pc_card_legacy_mode_base_addr;
}__attribute__((packed));

static uint16_t vendorID = 0;
static uint8_t headerType = 0;

// Function prototypes
uint32_t pciConfigReadDWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint8_t pciConfigReadByte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void pciConfigWriteDword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t data);
uint16_t pciCheckVendor(uint8_t bus, uint8_t slot);
void pciConfigSetCommand(uint8_t bus, uint8_t slot, uint8_t func, uint16_t command);
uint16_t pciConfigReadCommand(uint8_t bus, uint8_t slot, uint8_t func);

uint16_t getVendorID(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t getHeaderType(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t getBaseClass(uint8_t bus, uint8_t device, uint8_t function);
uint8_t getSubClass(uint8_t bus, uint8_t device, uint8_t function);
uint8_t getSecondaryBus(uint8_t bus, uint8_t device, uint8_t function);

void* readPCIConfig(uint8_t bus, uint8_t slot, uint8_t func);

void checkFunction(uint8_t bus, uint8_t device, uint8_t function);
void checkDevice(uint8_t bus, uint8_t device);
