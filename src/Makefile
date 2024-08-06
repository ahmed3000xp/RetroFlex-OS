# Copyright (C) 2024 Ahmed
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

IMG_DIR=..
TOOLS_DIR=../tools
BUILD_DIR=../build

QFLAGS=-debugcon stdio
QNFLAGS=-enable-kvm -cpu host -debugcon stdio

CFLAGS=-m32 -Wall -Wextra -Werror -Wpedantic -ffreestanding -fno-stack-protector -c
ASMFLAGS=-f elf
LDFLAGS=-m elf_i386 -T linker.ld

CC=gcc
AS=nasm
LD=ld

kernel:
	$(CC) $(CFLAGS) kernel.c -o $(BUILD_DIR)/kernelc.o
	$(CC) $(CFLAGS) Headers/util.c -o $(BUILD_DIR)/util.o
	$(CC) $(CFLAGS) Drivers/VGA/vga.c -o $(BUILD_DIR)/vga.o
	$(CC) $(CFLAGS) GDT/gdt.c -o $(BUILD_DIR)/gdtc.o
	$(CC) $(CFLAGS) IDT/idt.c -o $(BUILD_DIR)/idtc.o
	$(CC) $(CFLAGS) Drivers/Speaker/speaker.c -o $(BUILD_DIR)/speaker.o
	$(CC) $(CFLAGS) Drivers/PIT/pit.c -o $(BUILD_DIR)/pit.o
	$(CC) $(CFLAGS) Drivers/CMOS/cmos.c -o $(BUILD_DIR)/cmos.o
	$(CC) $(CFLAGS) Drivers/PS2/ps2.c -o $(BUILD_DIR)/ps2.o
	$(CC) $(CFLAGS) Headers/multiboot.c -o $(BUILD_DIR)/multiboot.o
	$(CC) $(CFLAGS) Paging/paging.c -o $(BUILD_DIR)/pagingc.o
	$(CC) $(CFLAGS) Drivers/PCI/pci.c -o $(BUILD_DIR)/pci.o
	$(CC) $(CFLAGS) Drivers/ATA/ata.c -o $(BUILD_DIR)/ata.o

	$(AS) $(ASMFLAGS) Boot/boot.asm -o $(BUILD_DIR)/boot.o
	$(AS) $(ASMFLAGS) kernel.asm -o $(BUILD_DIR)/kernelasm.o
	$(AS) $(ASMFLAGS) GDT/gdt.asm -o $(BUILD_DIR)/gdtasm.o
	$(AS) $(ASMFLAGS) IDT/idt.asm -o $(BUILD_DIR)/idtasm.o
	$(AS) $(ASMFLAGS) Paging/paging.asm -o $(BUILD_DIR)/pagingasm.o

	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/kernel $(BUILD_DIR)/boot.o $(BUILD_DIR)/kernelc.o $(BUILD_DIR)/kernelasm.o $(BUILD_DIR)/gdtc.o $(BUILD_DIR)/gdtasm.o $(BUILD_DIR)/idtc.o $(BUILD_DIR)/idtasm.o $(BUILD_DIR)/pagingc.o $(BUILD_DIR)/pagingasm.o $(BUILD_DIR)/util.o $(BUILD_DIR)/vga.o $(BUILD_DIR)/speaker.o $(BUILD_DIR)/pit.o $(BUILD_DIR)/cmos.o $(BUILD_DIR)/ps2.o $(BUILD_DIR)/multiboot.o $(BUILD_DIR)/pci.o $(BUILD_DIR)/ata.o

disk:
	dd if=/dev/zero of=$(IMG_DIR)/disk.img bs=512 count=2048000

	sudo parted $(IMG_DIR)/disk.img mklabel msdos
	sudo parted $(IMG_DIR)/disk.img mkpart primary fat32 0% 100%
	sudo parted $(IMG_DIR)/disk.img set 1 lba on

	sudo losetup -fP $(IMG_DIR)/disk.img
	sudo mkfs.vfat -F 32 /dev/loop0p1

	sudo mkdir -p /mnt/os
	sudo mount /dev/loop0p1 /mnt/os

	sudo grub-install --target=i386-pc --root-directory=/mnt/os/ --boot-directory=/mnt/os/boot /dev/loop0

	sudo cp $(BUILD_DIR)/kernel /mnt/os/boot/kernel
	sudo cp $(BUILD_DIR)/grub.cfg /mnt/os/boot/grub/grub.cfg

	sudo umount /mnt/os
	sudo rmdir /mnt/os
	sudo losetup -d /dev/loop0

qemu:
	qemu-system-x86_64 $(QFLAGS) -hda $(IMG_DIR)/disk.img

qemu_native:
	qemu-system-x86_64 $(QNFLAGS) -hda $(IMG_DIR)/disk.img

clean:
	rm -f $(BUILD_DIR)/*.o $(IMG_DIR)/disk.img $(BUILD_DIR)/kernel