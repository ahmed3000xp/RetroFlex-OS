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

COMPILER_FLAGS=-m32 -Werror -ffreestanding -fno-stack-protector -c
ASSEMBLER_FLAGS=-f elf
LINKER_FLAGS=-m elf_i386 -T linker.ld
QEMU_FLAGS=-debugcon stdio
QEMU_NATIVE_FLAGS=-enable-kvm -cpu host -debugcon stdio

kernel:
	gcc $(COMPILER_FLAGS) kernel.c -o build/kernelc.o
	gcc $(COMPILER_FLAGS) Headers/util.c -o build/util.o
	gcc $(COMPILER_FLAGS) Drivers/VGA/vga.c -o build/vga.o
	gcc $(COMPILER_FLAGS) GDT/gdt.c -o build/gdtc.o
	gcc $(COMPILER_FLAGS) IDT/idt.c -o build/idtc.o
	gcc $(COMPILER_FLAGS) Drivers/Speaker/speaker.c -o build/speaker.o
	gcc $(COMPILER_FLAGS) Drivers/PIT/pit.c -o build/pit.o
	gcc $(COMPILER_FLAGS) Drivers/CMOS/cmos.c -o build/cmos.o
	gcc $(COMPILER_FLAGS) Drivers/PS2/ps2.c -o build/ps2.o
	gcc $(COMPILER_FLAGS) Headers/multiboot.c -o build/multiboot.o
	gcc $(COMPILER_FLAGS) Paging/paging.c -o build/pagingc.o
	gcc $(COMPILER_FLAGS) Drivers/PCI/pci.c -o build/pci.o
	gcc $(COMPILER_FLAGS) Drivers/ATA/ata.c -o build/ata.o

	nasm $(ASSEMBLER_FLAGS) Boot/boot.asm -o build/boot.o
	nasm $(ASSEMBLER_FLAGS) kernel.asm -o build/kernelasm.o
	nasm $(ASSEMBLER_FLAGS) GDT/gdt.asm -o build/gdtasm.o
	nasm $(ASSEMBLER_FLAGS) IDT/idt.asm -o build/idtasm.o
	nasm $(ASSEMBLER_FLAGS) Paging/paging.asm -o build/pagingasm.o

	ld $(LINKER_FLAGS) -o build/kernel build/*.o

	mv build/kernel build/iso/boot/kernel

disk:
	grub-mkrescue -o build/os.iso build/iso/

qemu:
	qemu-system-x86_64 $(QEMU_FLAGS) build/os.iso

qemu_native:
	qemu-system-x86_64 $(QEMU_NATIVE_FLAGS) build/os.iso

clean: build/*.o build/*.iso build/iso/boot/kernel
	rm build/*.o build/*.iso build/iso/boot/kernel