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

#include "pci.h"

static uint16_t vendorID = 0;
static uint8_t headerType = 0;

void checkBus(uint8_t bus);

// Function to read a 32-bit Double word from a PCI configuration space
uint32_t pciConfigReadDWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
  
    // Construct the configuration address
    address = (lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | 0x80000000;
  
    // Write the address to the configuration address port
    outdw(0xCF8, address);
  
    return indw(0xCFC);
}

// Function to read a 16-bit word from a PCI configuration space
uint16_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
  
    // Construct the configuration address
    address = (lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | 0x80000000;
  
    // Write the address to the configuration address port
    outdw(0xCF8, address);
  
    return (uint16_t)indw(0xCFC);
}

// Function to read a 8-bit byte from a PCI configuration space
uint8_t pciConfigReadByte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
  
    // Construct the configuration address
    address = (lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | 0x80000000;
  
    // Write the address to the configuration address port
    outdw(0xCF8, address);
  
    return (uint8_t)indw(0xCFC);
}

// Function to write a 32-bit Double word from a PCI configuration space
void pciConfigWriteDword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t data){
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
  
    // Construct the configuration address
    address = (lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | 0x80000000;
  
    // Write the address to the configuration address port
    outdw(0xCF8, address);
  
    // Read the 32-bit data from the configuration data port
    outdw(0xCFC, data);
}

// Function to check the Vendor ID of a PCI device
uint16_t pciCheckVendor(uint8_t bus, uint8_t slot) {
    uint16_t vendor;

    // Read the vendor ID from the first configuration register
    vendor = pciConfigReadWord(bus, slot, 0, 0);

    // Check if the vendor ID indicates a non-existent device
    if (vendor != 0xFFFF) {
        // Read the device ID (just as an example, not used in this function)
        uint16_t device = pciConfigReadWord(bus, slot, 0, 2);
        // Use the device variable if needed in future
        (void)device; // Prevent unused variable warning
    }

    // Return the vendor ID (or some other meaningful value based on your needs)
    return vendor;
}

// Function to read and write the PCI command register
void pciConfigSetCommand(uint8_t bus, uint8_t slot, uint8_t func, uint16_t command) {
    pciConfigWriteDword(bus, slot, func, 0x4, (uint16_t)command);
}

// Function to read the PCI command register
uint16_t pciConfigReadCommand(uint8_t bus, uint8_t slot, uint8_t func) {
    // Read the command value from the PCI configuration data port
    return pciConfigReadWord(bus, slot, func, 0x4);
}

uint16_t getVendorID(uint8_t bus, uint8_t slot, uint8_t func) {
    return (uint16_t)(pciConfigReadDWord(bus, slot, func, 0x00) & 0xFFFF);
}

uint8_t getHeaderType(uint8_t bus, uint8_t slot, uint8_t func) {
    return (uint8_t)((pciConfigReadDWord(bus, slot, func, 0x0C) >> 16) & 0xFF);
}

uint8_t getBaseClass(uint8_t bus, uint8_t device, uint8_t function) {
    return pciConfigReadByte(bus, device, function, 0x0B);
}

uint8_t getSubClass(uint8_t bus, uint8_t device, uint8_t function) {
    return pciConfigReadByte(bus, device, function, 0x0A);
}

uint8_t getSecondaryBus(uint8_t bus, uint8_t device, uint8_t function) {
    // Only applicable for devices with header type 0x01 (PCI-PCI Bridge)
    uint8_t headerType = pciConfigReadByte(bus, device, function, 0x0E);
    if ((headerType & 0x80) == 0x80) { // Multi-function device
        return pciConfigReadByte(bus, device, function, 0x19);
    }
    return 0xFF; // Return a default value indicating not applicable
}

// Function to read PCI configuration space based on header type
void* readPCIConfig(uint8_t bus, uint8_t slot, uint8_t func) {
    uint8_t headerType = getHeaderType(bus, slot, func);
    
    if (headerType == 0x00) {
        struct header_0* h0;
        
        memset(&h0, 0, sizeof(struct header_0));

        h0->vendor_id = pciConfigReadWord(bus, slot, func, 0x00);
        h0->device_id = pciConfigReadWord(bus, slot, func, 0x02);
        h0->command = pciConfigReadWord(bus, slot, func, 0x04);
        h0->status = pciConfigReadWord(bus, slot, func, 0x06);
        h0->revision_id = pciConfigReadByte(bus, slot, func, 0x08);
        h0->latency_timer = pciConfigReadByte(bus, slot, func, 0x09);
        h0->header_type = pciConfigReadByte(bus, slot, func, 0x0A);
        h0->bist = pciConfigReadByte(bus, slot, func, 0x0B);
        h0->bar0 = pciConfigReadDWord(bus, slot, func, 0x10);
        h0->bar1 = pciConfigReadDWord(bus, slot, func, 0x14);
        h0->bar2 = pciConfigReadDWord(bus, slot, func, 0x18);
        h0->bar3 = pciConfigReadDWord(bus, slot, func, 0x1C);
        h0->bar4 = pciConfigReadDWord(bus, slot, func, 0x20);
        h0->bar5 = pciConfigReadDWord(bus, slot, func, 0x24);
        h0->cardbus_cis_ptr = pciConfigReadDWord(bus, slot, func, 0x28);
        h0->subsystem_vendor_id = pciConfigReadWord(bus, slot, func, 0x2C);
        h0->subsystem_id = pciConfigReadWord(bus, slot, func, 0x2E);
        h0->expansion_rom_base_addr = pciConfigReadDWord(bus, slot, func, 0x30);
        h0->capabilities_ptr = pciConfigReadByte(bus, slot, func, 0x34);
        h0->reserved = pciConfigReadWord(bus, slot, func, 0x36);
        h0->reserved1 = pciConfigReadByte(bus, slot, func, 0x38);
        h0->reserved2 = pciConfigReadDWord(bus, slot, func, 0x3C);
        h0->int_line = pciConfigReadByte(bus, slot, func, 0x3F);
        h0->int_pin = pciConfigReadByte(bus, slot, func, 0x3E);
        h0->min_grant = pciConfigReadByte(bus, slot, func, 0x3F);
        h0->max_latency = pciConfigReadByte(bus, slot, func, 0x40);
        
        return h0;
        
    } else if (headerType == 0x01) {
        struct header_1* h1;

        memset(&h1, 0, sizeof(struct header_1));
        
        h1->vendor_id = pciConfigReadWord(bus, slot, func, 0x00);
        h1->device_id = pciConfigReadWord(bus, slot, func, 0x02);
        h1->command = pciConfigReadWord(bus, slot, func, 0x04);
        h1->status = pciConfigReadWord(bus, slot, func, 0x06);
        h1->revision_id = pciConfigReadByte(bus, slot, func, 0x08);
        h1->prog_if = pciConfigReadByte(bus, slot, func, 0x09);
        h1->subclass = pciConfigReadByte(bus, slot, func, 0x0A);
        h1->class_code = pciConfigReadByte(bus, slot, func, 0x0B);
        h1->cache_line_size = pciConfigReadByte(bus, slot, func, 0x0C);
        h1->latency_timer = pciConfigReadByte(bus, slot, func, 0x0D);
        h1->header_type = pciConfigReadByte(bus, slot, func, 0x0E);
        h1->bist = pciConfigReadByte(bus, slot, func, 0x0F);
        h1->bar0 = pciConfigReadDWord(bus, slot, func, 0x10);
        h1->bar1 = pciConfigReadDWord(bus, slot, func, 0x14);
        h1->primary_bus_num = pciConfigReadByte(bus, slot, func, 0x18);
        h1->secondary_bus_num = pciConfigReadByte(bus, slot, func, 0x19);
        h1->subordinate_bus_num = pciConfigReadByte(bus, slot, func, 0x1A);
        h1->secondary_latency_timer = pciConfigReadByte(bus, slot, func, 0x1B);
        h1->io_base = pciConfigReadByte(bus, slot, func, 0x1C);
        h1->io_limit = pciConfigReadByte(bus, slot, func, 0x1D);
        h1->secondary_status = pciConfigReadWord(bus, slot, func, 0x1E);
        h1->memory_base = pciConfigReadWord(bus, slot, func, 0x20);
        h1->memory_limit = pciConfigReadWord(bus, slot, func, 0x22);
        h1->prefetchable_memory_base = pciConfigReadWord(bus, slot, func, 0x24);
        h1->prefetchable_memory_limit = pciConfigReadWord(bus, slot, func, 0x26);
        h1->prefetchable_base_upper_32_bits = pciConfigReadDWord(bus, slot, func, 0x28);
        h1->prefetchable_limit_upper_32_bits = pciConfigReadDWord(bus, slot, func, 0x2C);
        h1->io_base_upper_16_bits = pciConfigReadWord(bus, slot, func, 0x30);
        h1->io_limit_upper_16_bits = pciConfigReadWord(bus, slot, func, 0x32);
        h1->capability_ptr = pciConfigReadByte(bus, slot, func, 0x34);
        h1->reserved = pciConfigReadWord(bus, slot, func, 0x36);
        h1->reserved1 = pciConfigReadWord(bus, slot, func, 0x38);
        h1->expansion_rom_base_addr = pciConfigReadDWord(bus, slot, func, 0x3C);
        h1->int_line = pciConfigReadByte(bus, slot, func, 0x3F);
        h1->int_pin = pciConfigReadByte(bus, slot, func, 0x3E);
        h1->bridge_control = pciConfigReadWord(bus, slot, func, 0x3F);
        
        return h1;
        
    } else if (headerType == 0x02) {
        struct header_2* h2;

        memset(&h2, 0, sizeof(struct header_2));
        
        h2->vendor_id = pciConfigReadWord(bus, slot, func, 0x00);
        h2->device_id = pciConfigReadWord(bus, slot, func, 0x02);
        h2->command = pciConfigReadWord(bus, slot, func, 0x04);
        h2->status = pciConfigReadWord(bus, slot, func, 0x06);
        h2->revision_id = pciConfigReadByte(bus, slot, func, 0x08);
        h2->prog_if = pciConfigReadByte(bus, slot, func, 0x09);
        h2->latency_timer = pciConfigReadByte(bus, slot, func, 0x0A);
        h2->header_type = pciConfigReadByte(bus, slot, func, 0x0B);
        h2->bist = pciConfigReadByte(bus, slot, func, 0x0C);
        h2->cardbus_sock_exca_base_addr = pciConfigReadDWord(bus, slot, func, 0x10);
        h2->offset_capabilities_list = pciConfigReadByte(bus, slot, func, 0x14);
        h2->reserved = pciConfigReadByte(bus, slot, func, 0x15);
        h2->secondary_status = pciConfigReadWord(bus, slot, func, 0x16);
        h2->pci_bus_number = pciConfigReadByte(bus, slot, func, 0x18);
        h2->cardbus_bus_num = pciConfigReadByte(bus, slot, func, 0x19);
        h2->subordinate_bus_number = pciConfigReadByte(bus, slot, func, 0x1A);
        h2->cardbus_latency_timer = pciConfigReadByte(bus, slot, func, 0x1B);
        h2->memory_base_addr0 = pciConfigReadDWord(bus, slot, func, 0x1C);
        h2->memory_limit0 = pciConfigReadDWord(bus, slot, func, 0x20);
        h2->memory_base_addr1 = pciConfigReadDWord(bus, slot, func, 0x24);
        h2->memory_limit1 = pciConfigReadDWord(bus, slot, func, 0x28);
        h2->io_base_addr0 = pciConfigReadDWord(bus, slot, func, 0x2C);
        h2->io_limit0 = pciConfigReadDWord(bus, slot, func, 0x30);
        h2->io_base_addr1 = pciConfigReadDWord(bus, slot, func, 0x34);
        h2->io_limit1 = pciConfigReadDWord(bus, slot, func, 0x38);
        h2->int_line = pciConfigReadByte(bus, slot, func, 0x3C);
        h2->int_pin = pciConfigReadByte(bus, slot, func, 0x3D);
        h2->bridge_control = pciConfigReadWord(bus, slot, func, 0x3E);
        h2->subsystem_device_id = pciConfigReadWord(bus, slot, func, 0x40);
        h2->subsystem_vendor_id = pciConfigReadWord(bus, slot, func, 0x42);
        h2->pc_card_legacy_mode_base_addr = pciConfigReadDWord(bus, slot, func, 0x44);
        
        return h2;
        
    } else {
        return NULL;
    }
}

void checkFunction(uint8_t bus, uint8_t device, uint8_t function) {
    uint8_t baseClass;
    uint8_t subClass;
    uint8_t secondaryBus;

    baseClass = getBaseClass(bus, device, function);
    subClass = getSubClass(bus, device, function);
    if ((baseClass == 0x6) && (subClass == 0x4)) {
        secondaryBus = getSecondaryBus(bus, device, function);
        checkBus(secondaryBus);
    }
}

void checkDevice(uint8_t bus, uint8_t device) {
    uint8_t function = 0;

    vendorID = getVendorID(bus, device, function);
    if (vendorID == 0xFFFF) return; // Device doesn't exist
    checkFunction(bus, device, function);
    headerType = getHeaderType(bus, device, function);
    if( (headerType & 0x80) != 0) {
        // It's a multi-function device, so check remaining functions
        for (function = 1; function < 8; function++) {
            if (getVendorID(bus, device, function) != 0xFFFF) {
                checkFunction(bus, device, function);
            }
        }
    }
}

void checkBus(uint8_t bus) {
    uint8_t device;

    for (device = 0; device < 32; device++) {
        checkDevice(bus, device);
    }
}

void checkAllBuses(void) {
    uint8_t function;
    uint8_t bus;

    headerType = getHeaderType(0, 0, 0);
    if ((headerType & 0x80) == 0) {
        // Single PCI host controller
        checkBus(0);
    } else {
        // Multiple PCI host controllers
        for (function = 0; function < 8; function++) {
            if (getVendorID(0, 0, function) != 0xFFFF) break;
            bus = function;
            checkBus(bus);
        }
    }
}

uint64_t arch_msi_address(uint64_t *data, uint32_t vector, uint32_t processor, uint8_t edgetrigger, uint8_t deassert) {
	*data = (vector & 0xFF) | (edgetrigger == 1 ? 0 : (1 << 15)) | (deassert == 1 ? 0 : (1 << 14));
	return (0xFEE00000 | (processor << 12));
}